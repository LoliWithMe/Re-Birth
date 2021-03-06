#include "../../stdafx.h"
#include "SkinnedMesh.h"

SkinnedMesh::SkinnedMesh()
	: m_pRoot(NULL)
	, m_pAnimController(NULL)
	, m_fBlendTime(1.0f)
	, m_fPassedTime(0.0f)
	, m_fAlpha(1.0f)
	, m_fFadeCut(0.0f)
	, m_fRimPower(0.0f)
	, m_fOffset(0.0f)
	, m_vRimColor(D3DXVECTOR4(255, 255, 255, 1))
	, m_isHolo(false)
{
}

SkinnedMesh::~SkinnedMesh()
{
	Destroy(m_pRoot);
}

void SkinnedMesh::CloneAnimation(SkinnedMesh * orig)
{
	m_pRoot = orig->m_pRoot;
	m_animMap = orig->m_animMap;
	orig->m_pAnimController->CloneAnimationController(
		orig->m_pAnimController->GetMaxNumAnimationOutputs(),
		orig->m_pAnimController->GetMaxNumAnimationSets(),
		orig->m_pAnimController->GetMaxNumTracks(),
		orig->m_pAnimController->GetMaxNumEvents(),
		&m_pAnimController);

	m_pShaderEffect = Shader::LoadShader("Shader/Fade UV.fx");
}

AllocatedHierachy::AllocatedHierachy()
{
}

AllocatedHierachy::~AllocatedHierachy()
{
}

STDMETHODIMP AllocatedHierachy::CreateFrame(LPCSTR Name, LPD3DXFRAME * ppNewFrame)
{
	ST_BONE * pBone = new ST_BONE;
	ZeroMemory(pBone, sizeof(ST_BONE));

	if (Name)
	{
		UINT length = strlen(Name) + 1;
		pBone->Name = new char[length];
		strcpy_s(pBone->Name, length, Name);
	}

	D3DXMatrixIdentity(&pBone->CombinedTransformationMatrix);
	D3DXMatrixIdentity(&pBone->TransformationMatrix);

	*ppNewFrame = pBone;

	return S_OK;
}

STDMETHODIMP AllocatedHierachy::CreateMeshContainer(
	LPCSTR Name, 
	const D3DXMESHDATA * pMeshData, 
	const D3DXMATERIAL * pMaterials, 
	const D3DXEFFECTINSTANCE * pEffectInstances,
	DWORD NumMaterials,
	const DWORD * pAdjacency, 
	LPD3DXSKININFO pSkinInfo,
	LPD3DXMESHCONTAINER * ppNewMeshContainer)
{
	ST_BONE_MESH * pBoneMesh = new ST_BONE_MESH;
	ZeroMemory(pBoneMesh, sizeof(ST_BONE_MESH));

	for (int i = 0; i < NumMaterials; i++)
	{
		pBoneMesh->vecMtl.push_back(pMaterials[i].MatD3D);
		string sFullPath = m_sFolder;
		sFullPath += string(pMaterials[i].pTextureFilename);
		pBoneMesh->vecTex.push_back(TEXTUREMANAGER->AddTexture(sFullPath.c_str(), sFullPath.c_str()));
		int n = -1; 
		bool big = false;
		if (sFullPath.find("_D") != string::npos) { n = sFullPath.find("_D"); big = true; }
		if (sFullPath.find("_d") != string::npos) { n = sFullPath.find("_d"); big = false; }
		if (n > 0)
		{
			sFullPath = sFullPath.substr(0, n);
			sFullPath += big ? "_N.tga" : "_n.tga";
			struct stat buffer;
			if (stat(sFullPath.c_str(), &buffer) == 0)
				pBoneMesh->vecNor.push_back(TEXTUREMANAGER->AddTexture(sFullPath.c_str(), sFullPath.c_str()));
		}
	}

	pSkinInfo->AddRef();
	pBoneMesh->pSkinInfo = pSkinInfo;

	pMeshData->pMesh->AddRef();
	pBoneMesh->MeshData.pMesh = pMeshData->pMesh;
	pMeshData->pMesh->CloneMeshFVF(
		pMeshData->pMesh->GetOptions(),
		pMeshData->pMesh->GetFVF(),
		DEVICE,
		&pBoneMesh->pOrigMesh);

	DWORD dwNumBones = pSkinInfo->GetNumBones();
	pBoneMesh->ppBoneMatrixPtrs = new D3DXMATRIX*[dwNumBones];
	pBoneMesh->pBoneOffsetMatrices = new D3DXMATRIX[dwNumBones];
	pBoneMesh->pCurrentBoneMatrices = new D3DXMATRIX[dwNumBones];

	for (DWORD i = 0; i < dwNumBones; i++)
		pBoneMesh->pBoneOffsetMatrices[i] = *pSkinInfo->GetBoneOffsetMatrix(i);

	*ppNewMeshContainer = pBoneMesh;

	return S_OK;
}

STDMETHODIMP AllocatedHierachy::DestroyFrame(LPD3DXFRAME pFrameToFree)
{
	ST_BONE* pBone = (ST_BONE*)pFrameToFree;

	SAFE_DELETE_ARRAY(pBone->Name);
	SAFE_DELETE(pBone);

	return S_OK;
}

STDMETHODIMP AllocatedHierachy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerToFree)
{
	ST_BONE_MESH * pBoneMesh = (ST_BONE_MESH*)pMeshContainerToFree;

	SAFE_RELEASE(pBoneMesh->pSkinInfo);
	SAFE_RELEASE(pBoneMesh->MeshData.pMesh);
	SAFE_RELEASE(pBoneMesh->pOrigMesh);

	SAFE_DELETE_ARRAY(pBoneMesh->ppBoneMatrixPtrs);
	SAFE_DELETE_ARRAY(pBoneMesh->pBoneOffsetMatrices);
	SAFE_DELETE_ARRAY(pBoneMesh->pCurrentBoneMatrices);

	SAFE_DELETE(pBoneMesh);

	return S_OK;
}

void SkinnedMesh::Setup(string szFolder, string szFile)
{
	AllocatedHierachy ah;
	ah.SetFolder(szFolder);

	D3DXLoadMeshHierarchyFromX((szFolder + szFile).c_str(),
		D3DXMESH_32BIT | D3DXMESH_MANAGED,
		DEVICE,
		&ah,
		NULL,
		&m_pRoot,
		&m_pAnimController);

	SetupBoneMatrixPtrs(m_pRoot);

	for (int i = 0; i < m_pAnimController->GetNumAnimationSets(); i++)
	{
		LPD3DXANIMATIONSET animSet = NULL;
		m_pAnimController->GetAnimationSet(i, &animSet);
		m_animMap[animSet->GetName()] = i;
		SAFE_RELEASE(animSet);
	}
}

void SkinnedMesh::Update(bool dummy)
{
	Update(m_pRoot, NULL, dummy);
	UpdateSkinnedMesh(m_pRoot);
}

void SkinnedMesh::Animate()
{
	if (m_isBlend)
	{
		m_fPassedTime += TIME->GetElapsedTime();
		if (m_fPassedTime > m_fBlendTime)
		{
			m_pAnimController->SetTrackWeight(0, 1.0f);
			m_pAnimController->SetTrackEnable(1, false);
			m_isBlend = false;
		}
		else
		{
			float fWeight = m_fPassedTime / 1.0f;
			m_pAnimController->SetTrackWeight(0, fWeight);
			m_pAnimController->SetTrackWeight(1, m_fBlendTime - fWeight);
		}
	}

	m_pAnimController->AdvanceTime(TIME->GetElapsedTime(), NULL);
}

void SkinnedMesh::SetIndex(int index)
{
	LPD3DXANIMATIONSET animSet = NULL;
	m_pAnimController->GetAnimationSet(index, &animSet);
	m_pAnimController->SetTrackAnimationSet(0, animSet);
	m_pAnimController->SetTrackPosition(0, 0);
	animSet->Release();
}

void SkinnedMesh::SetBlendIndex(int index)
{
	LPD3DXANIMATIONSET prevAnim = NULL;
	LPD3DXANIMATIONSET nextAnim = NULL;

	m_isBlend = true;
	m_fPassedTime = 0;

	D3DXTRACK_DESC stTrackDesc;
	m_pAnimController->GetTrackDesc(0, &stTrackDesc);
	m_pAnimController->GetTrackAnimationSet(0, &prevAnim);

	// 복사한걸 1번으로 옮김
	m_pAnimController->SetTrackAnimationSet(1, prevAnim);
	m_pAnimController->SetTrackDesc(1, &stTrackDesc);

	// 현재 애니메이션 재설정
	m_pAnimController->GetAnimationSet(index, &nextAnim);
	m_pAnimController->SetTrackAnimationSet(0, nextAnim);

	// 가중치
	m_pAnimController->SetTrackWeight(0, 0.0f);
	m_pAnimController->SetTrackWeight(1, 1.0f);

	prevAnim->Release();
	nextAnim->Release();
}

void SkinnedMesh::SetAnimationSpeed(float speed)
{
	m_pAnimController->SetTrackSpeed(0, speed);
	m_pAnimController->SetTrackSpeed(1, speed);
}

void SkinnedMesh::SetAnimationPosition(float pos)
{
	LPD3DXANIMATIONSET anim = NULL;
	D3DXTRACK_DESC desc;
	m_pAnimController->GetTrackAnimationSet(0, &anim);
	m_pAnimController->GetTrackDesc(0, &desc);

	float period = anim->GetPeriod() / desc.Speed;

	desc.Position = period * pos;
	m_pAnimController->SetTrackDesc(0, &desc);
}

bool SkinnedMesh::IsAnimationPercent(float rate)
{
	if (rate < 0 || rate > 1) return false;

	LPD3DXANIMATIONSET anim = NULL;
	D3DXTRACK_DESC desc;
	m_pAnimController->GetTrackAnimationSet(0, &anim);
	m_pAnimController->GetTrackDesc(0, &desc);

	float period = anim->GetPeriod() / desc.Speed;
	float current = fmod(desc.Position / desc.Speed, period);
	float curRate = current / period;
	float elapseRate = TIME->GetElapsedTime() / period * 0.5;

	SAFE_RELEASE(anim);

	if (curRate - elapseRate < rate && curRate + elapseRate > rate)
		return true;
	else
		return false;
}

bool SkinnedMesh::IsAnimationEnd()
{
	LPD3DXANIMATIONSET anim = NULL;
	D3DXTRACK_DESC desc;
	m_pAnimController->GetTrackAnimationSet(0, &anim);
	m_pAnimController->GetTrackDesc(0, &desc);

	float period = anim->GetPeriod() / desc.Speed;
	float current = fmod(desc.Position / desc.Speed, period);
	float curRate = current / period;
	float elapseRate = TIME->GetElapsedTime() / period * 2;

	SAFE_RELEASE(anim);

	return (1 - curRate <= elapseRate);
}

float SkinnedMesh::GetCurrentAnimationPercent()
{
	LPD3DXANIMATIONSET anim = NULL;
	D3DXTRACK_DESC desc;
	m_pAnimController->GetTrackAnimationSet(0, &anim);
	m_pAnimController->GetTrackDesc(0, &desc);

	float period = anim->GetPeriod() / desc.Speed;
	float current = fmod(desc.Position / desc.Speed, period);
	float curRate = current / period;

	SAFE_RELEASE(anim);

	return curRate;
}

float SkinnedMesh::GetAnimationPeriod(int index)
{
	LPD3DXANIMATIONSET animSet = NULL;
	D3DXTRACK_DESC desc;
	m_pAnimController->GetAnimationSet(index, &animSet);
	m_pAnimController->GetTrackDesc(0, &desc);
	float period = animSet->GetPeriod() / desc.Speed;
	SAFE_RELEASE(animSet);
	return period;
}

void SkinnedMesh::Update(LPD3DXFRAME pFrame, LPD3DXFRAME pParent, bool dummy)
{
	ST_BONE * pBone = (ST_BONE*)pFrame;
	pBone->CombinedTransformationMatrix = pBone->TransformationMatrix;
	if (pParent)
		pBone->CombinedTransformationMatrix *=
			((ST_BONE*)pParent)->CombinedTransformationMatrix;

	string name = "";
	if (pBone->Name)
		name = pBone->Name;
	if (dummy && name.compare("Bip001") == 0)
		pBone->CombinedTransformationMatrix = ((ST_BONE*)pParent)->CombinedTransformationMatrix;

	if (pFrame->pFrameFirstChild)
		Update(pFrame->pFrameFirstChild, pFrame, dummy);

	if (pFrame->pFrameSibling)
		Update(pFrame->pFrameSibling, pParent, dummy);
}

void SkinnedMesh::UpdateSkinnedMesh(LPD3DXFRAME pFrame)
{
	ST_BONE * pBone = (ST_BONE*)pFrame;

	if (pBone->pMeshContainer)
	{
		ST_BONE_MESH * pBoneMesh = (ST_BONE_MESH*)pBone->pMeshContainer;

		int numBones = pBoneMesh->pSkinInfo->GetNumBones();
		for (int i = 0; i < numBones; i++)
			D3DXMatrixMultiply(&pBoneMesh->pCurrentBoneMatrices[i],
				&pBoneMesh->pBoneOffsetMatrices[i],
				pBoneMesh->ppBoneMatrixPtrs[i]);

		BYTE * src = NULL, *dest = NULL;
		pBoneMesh->pOrigMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&src);
		pBoneMesh->MeshData.pMesh->LockVertexBuffer(0, (void**)&dest);

		pBoneMesh->pSkinInfo->UpdateSkinnedMesh(
			pBoneMesh->pCurrentBoneMatrices,
			NULL, src, dest);

		pBoneMesh->MeshData.pMesh->UnlockVertexBuffer();
		pBoneMesh->pOrigMesh->UnlockVertexBuffer();
	}

	if (pFrame->pFrameFirstChild)
		UpdateSkinnedMesh(pFrame->pFrameFirstChild);

	if (pBone->pFrameSibling)
		UpdateSkinnedMesh(pBone->pFrameSibling);
}

void SkinnedMesh::Render(LPD3DXFRAME pFrame, D3DXMATRIX * matWorld)
{
	if (pFrame == NULL)
		pFrame = m_pRoot;

	ST_BONE* pBone = (ST_BONE*)pFrame;
	string name = "";
	if (pBone->Name)
		name = pBone->Name;

	if (matWorld)
		pBone->CombinedTransformationMatrix *= *matWorld;

	if (false)
	{
		/*if (name.compare("Bip001-Head") == 0 ||
			name.compare("Bip001-L-Hand") == 0 || name.compare("Bip001-R-Hand") == 0 ||
			name.compare("Bip001-L-Foot") == 0 || name.compare("Bip001-R-Foot") == 0 ||
			name.compare("Bip001-Spine") == 0 || name.compare("Bip001-Spine1") == 0 || name.compare("Bip001-Spine2") == 0)
		*/{
			D3DXVECTOR3 pos = D3DXVECTOR3(0, 0, 0);
			D3DXVec3TransformCoord(&pos, &pos, &pBone->CombinedTransformationMatrix);
			D3DXVECTOR2 conv = Convert3DTo2D(pos);
			TEXT->Add(name, conv.x, conv.y, 14, "", 0xFFFFFFFF);
		}
	}

	if (pBone->pMeshContainer)
	{
		ST_BONE_MESH * pBoneMesh = (ST_BONE_MESH*)pBone->pMeshContainer;
		if (pBoneMesh->MeshData.pMesh)
		{
			D3DXVECTOR4 vLightDir = D3DXVECTOR4(1, -1, 1, 1);
			D3DXVECTOR4 vLightPosition = -vLightDir * 500;
			UINT pass = 0;

			D3DXMATRIX matView, matProj, matWVP, matViewProj;

			DEVICE->GetTransform(D3DTS_VIEW, &matView);
			DEVICE->GetTransform(D3DTS_PROJECTION, &matProj);
			matViewProj = matView * matProj;

			D3DXVECTOR3 vCamPos = CAMERA->GetEye();

			// 쉐이더
			m_pShaderEffect->Begin(&pass, 0);

			for (UINT i = 0; i < pass; i++)
			{
				if (i == 0 && m_isHolo) continue;
				if (i == 1 && !m_isHolo) continue;

				m_pShaderEffect->BeginPass(i);

				matWVP = pBone->CombinedTransformationMatrix * matView * matProj;

				m_pShaderEffect->SetMatrix("gWorldViewProjectionMatrix", &matWVP);
				m_pShaderEffect->SetMatrix("gWorldMatrix", &pBone->CombinedTransformationMatrix);
				m_pShaderEffect->SetVector("gWorldLightDirection", &vLightDir);

				m_pShaderEffect->SetFloat("gAlpha", m_fAlpha);
				m_pShaderEffect->SetFloat("gFadeCut", m_fFadeCut);
				m_pShaderEffect->SetFloat("gOffset", m_fOffset);
				m_pShaderEffect->SetFloat("gRimPower", m_fRimPower);
				m_pShaderEffect->SetVector("gRimColor", &D3DXVECTOR4(m_vRimColor.x / 255.0f, m_vRimColor.y / 255.0f, m_vRimColor.z / 255.0f, 1));
				m_pShaderEffect->SetVector("gCamPos", &D3DXVECTOR4(vCamPos.x, vCamPos.y, vCamPos.z, 1));

				m_pShaderEffect->SetMatrix("Fade_UV_Pass_1_Vertex_Shader_gWorldViewProjectionMatrix", &matWVP);
				m_pShaderEffect->SetMatrix("Fade_UV_Pass_1_Vertex_Shader_gWorldMatrix", &pBone->CombinedTransformationMatrix);
				m_pShaderEffect->SetFloat("Fade_UV_Pass_1_Pixel_Shader_gRimPower", 1);
				m_pShaderEffect->SetVector("Fade_UV_Pass_1_Pixel_Shader_gRimColor", &D3DXVECTOR4(m_vRimColor.x / 255.0f, m_vRimColor.y / 255.0f, m_vRimColor.z / 255.0f, 1));
				m_pShaderEffect->SetVector("Fade_UV_Pass_1_Pixel_Shader_gCamPos", &D3DXVECTOR4(vCamPos.x, vCamPos.y, vCamPos.z, 1));

				for (DWORD i = 0; i < pBoneMesh->vecMtl.size(); i++)
				{
					pBoneMesh->vecMtl[i].Diffuse;
					D3DXVECTOR4 diffuse;
					diffuse.x = pBoneMesh->vecMtl[i].Diffuse.r;
					diffuse.y = pBoneMesh->vecMtl[i].Diffuse.g;
					diffuse.z = pBoneMesh->vecMtl[i].Diffuse.b;
					diffuse.w = 1;
					m_pShaderEffect->SetVector("gLightColor", &diffuse);
					m_pShaderEffect->SetTexture("DiffuseMap_Tex", pBoneMesh->vecTex[i]);
					if (pBoneMesh->vecNor.size() > i)
						m_pShaderEffect->SetTexture("NormalMap_Tex", pBoneMesh->vecNor[i]);
					m_pShaderEffect->CommitChanges();
					pBoneMesh->MeshData.pMesh->DrawSubset(i);
				}

				m_pShaderEffect->EndPass();
			}
			
			m_pShaderEffect->End();
		}
	}

	if (pFrame->pFrameFirstChild)
		Render(pFrame->pFrameFirstChild, matWorld);

	if (pFrame->pFrameSibling)
		Render(pFrame->pFrameSibling, matWorld);
}

void SkinnedMesh::FindAllChild(LPD3DXFRAME pFrame, vector<LPD3DXFRAME>& vecChild)
{
	if (pFrame == NULL)
		pFrame = m_pRoot;

	vecChild.push_back(pFrame);

	if (pFrame->pFrameFirstChild)
		FindAllChild(pFrame->pFrameFirstChild, vecChild);
	if (pFrame->pFrameSibling)
		FindAllChild(pFrame->pFrameSibling, vecChild);
}

D3DXMATRIX * SkinnedMesh::GetBoneMatrix(string boneName)
{
	vector<LPD3DXFRAME> vecChild;
	FindAllChild(NULL, vecChild);

	for (int i = 0; i < vecChild.size(); i++)
	{
		ST_BONE * bone = (ST_BONE*)vecChild[i];
		if (bone->Name)
			if (boneName.compare(bone->Name) == 0)
				return &bone->CombinedTransformationMatrix;
	}

	return NULL;
}

void SkinnedMesh::UpdateRender(D3DXMATRIX * matWorld, bool dummy)
{
	Animate();
	Update(dummy);
	Render(m_pRoot, matWorld);
}

void SkinnedMesh::SetupBoneMatrixPtrs(LPD3DXFRAME pFrame)
{
	if (pFrame && pFrame->pMeshContainer)
	{
		ST_BONE_MESH * pBoneMesh = (ST_BONE_MESH*)pFrame->pMeshContainer;
		if (pBoneMesh->pSkinInfo)
		{
			DWORD dwNumBones = pBoneMesh->pSkinInfo->GetNumBones();
			for (DWORD i = 0; i < dwNumBones; i++)
			{
				ST_BONE * pBone = (ST_BONE*)D3DXFrameFind(m_pRoot,
					pBoneMesh->pSkinInfo->GetBoneName(i));

				pBoneMesh->ppBoneMatrixPtrs[i] = &pBone->CombinedTransformationMatrix;
			}
		}
	}

	if (pFrame->pFrameFirstChild)
		SetupBoneMatrixPtrs(pFrame->pFrameFirstChild);

	if (pFrame->pFrameSibling)
		SetupBoneMatrixPtrs(pFrame->pFrameSibling);
}

void SkinnedMesh::Destroy(D3DXFRAME * pFrame)
{
	AllocatedHierachy ah;
	D3DXFrameDestroy(pFrame, &ah);
}

void SkinnedMesh::Release()
{
	m_pRoot = NULL;
	SAFE_RELEASE(m_pAnimController);
	SAFE_RELEASE(m_pShaderEffect);
}
