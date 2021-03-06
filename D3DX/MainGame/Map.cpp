#include "../stdafx.h"
#include "Map.h"



Map::Map()
{
}


Map::~Map()
{
	for (int i = 0; i < m_vecModel.size(); i++)
		SAFE_DELETE(m_vecModel[i]);
	SAFE_RELEASE(m_pTerrainMesh);
	SAFE_RELEASE(m_pSkyboxMesh);

	m_vecIndex.clear();
	m_vecSurface.clear();
	m_vecTerrain.clear();
	m_vecModel.clear();
}

void Map::Setup()
{
	for (int z = 0; z < MAPSIZE; z++)
	{
		for (int x = 0; x < MAPSIZE; x++)
		{
			ST_PNT_VERTEX vertex;
			vertex.n = D3DXVECTOR3(0, 1, 0);
			vertex.p = D3DXVECTOR3(x - MAPSIZE / 2.0f, 0, -z + MAPSIZE / 2.0f);
			vertex.t = D3DXVECTOR2(x / (float)MAPSIZE, z / (float)MAPSIZE);
			m_vecTerrain.push_back(vertex);
		}
	}

	for (int z = 0; z < MAPSIZE - 1; z++)
	{
		for (int x = 0; x < MAPSIZE - 1; x++)
		{
			int index = MAPSIZE * (z + 1) + x;
			m_vecIndex.push_back(index);
			index = MAPSIZE * z + x;
			m_vecIndex.push_back(index);
			index = MAPSIZE * z + x + 1;
			m_vecIndex.push_back(index);

			index = MAPSIZE * (z + 1) + x;
			m_vecIndex.push_back(index);
			index = MAPSIZE * z + x + 1;
			m_vecIndex.push_back(index);
			index = MAPSIZE * (z + 1) + x + 1;
			m_vecIndex.push_back(index);
		}
	}

	ZeroMemory(&m_mtl, sizeof(D3DMATERIAL9));
	m_mtl.Ambient = D3DXCOLOR(0.2, 0.2, 0.2, 1);
	m_mtl.Diffuse = D3DXCOLOR(0.8, 0.8, 0.8, 1);

	D3DXMatrixIdentity(&m_matWorld);

	DEVICE->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	DEVICE->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// 스카이박스
	TEXTUREMANAGER->AddTexture("Skybox", "Map/Skybox.png");
	D3DXIMAGE_INFO info = TEXTUREMANAGER->GetInfo("Skybox");
	m_pSkyboxTex = TEXTUREMANAGER->GetTexture("Skybox");

	float w = info.Width;
	float h = info.Height;
	float cw = info.Width / 4.0f;
	float ch = info.Height / 3.0f;

	float a = cw * 2;

	vector<ST_PT_VERTEX> vecVertex;
	ST_PT_VERTEX v;
	// 11
	v.p = D3DXVECTOR3(-1, -1, 1);
	v.t = D3DXVECTOR2(cw * 1 / w, ch * 2 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(-1, 1, 1);
	v.t = D3DXVECTOR2(cw * 1 / w, ch * 1 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(1, 1, 1);
	v.t = D3DXVECTOR2(cw * 2 / w, ch * 1 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(-1, -1, 1);
	v.t = D3DXVECTOR2(cw * 1 / w, ch * 2 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(1, 1, 1);
	v.t = D3DXVECTOR2(cw * 2 / w, ch * 1 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(1, -1, 1);
	v.t = D3DXVECTOR2(cw * 2 / w, ch * 2 / h);
	vecVertex.push_back(v);

	// 01
	v.p = D3DXVECTOR3(-1, -1, -1);
	v.t = D3DXVECTOR2(cw * 0 / w, ch * 2 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(-1, 1, -1);
	v.t = D3DXVECTOR2(cw * 0 / w, ch * 1 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(-1, 1, 1);
	v.t = D3DXVECTOR2(cw * 1 / w, ch * 1 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(-1, -1, -1);
	v.t = D3DXVECTOR2(cw * 0 / w, ch * 2 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(-1, 1, 1);
	v.t = D3DXVECTOR2(cw * 1 / w, ch * 1 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(-1, -1, 1);
	v.t = D3DXVECTOR2(cw * 1 / w, ch * 2 / h);
	vecVertex.push_back(v);

	// 21
	v.p = D3DXVECTOR3(1, -1, 1);
	v.t = D3DXVECTOR2(cw * 2 / w, ch * 2 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(1, 1, 1);
	v.t = D3DXVECTOR2(cw * 2 / w, ch * 1 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(1, 1, -1);
	v.t = D3DXVECTOR2(cw * 3 / w, ch * 1 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(1, -1, 1);
	v.t = D3DXVECTOR2(cw * 2 / w, ch * 2 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(1, 1, -1);
	v.t = D3DXVECTOR2(cw * 3 / w, ch * 1 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(1, -1, -1);
	v.t = D3DXVECTOR2(cw * 3 / w, ch * 2 / h);
	vecVertex.push_back(v);

	// 31
	v.p = D3DXVECTOR3(1, -1, -1);
	v.t = D3DXVECTOR2(cw * 3 / w, ch * 2 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(1, 1, -1);
	v.t = D3DXVECTOR2(cw * 3 / w, ch * 1 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(-1, 1, -1);
	v.t = D3DXVECTOR2(cw * 4 / w, ch * 1 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(1, -1, -1);
	v.t = D3DXVECTOR2(cw * 3 / w, ch * 2 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(-1, 1, -1);
	v.t = D3DXVECTOR2(cw * 4 / w, ch * 1 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(-1, -1, -1);
	v.t = D3DXVECTOR2(cw * 4 / w, ch * 2 / h);
	vecVertex.push_back(v);

	// 10
	v.p = D3DXVECTOR3(-1, 1, 1);
	v.t = D3DXVECTOR2(cw * 1 / w, ch * 1 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(-1, 1, -1);
	v.t = D3DXVECTOR2(cw * 1 / w, ch * 0 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(1, 1, -1);
	v.t = D3DXVECTOR2(cw * 2 / w, ch * 0 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(-1, 1, 1);
	v.t = D3DXVECTOR2(cw * 1 / w, ch * 1 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(1, 1, -1);
	v.t = D3DXVECTOR2(cw * 2 / w, ch * 0 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(1, 1, 1);
	v.t = D3DXVECTOR2(cw * 2 / w, ch * 1 / h);
	vecVertex.push_back(v);

	// 12
	v.p = D3DXVECTOR3(-1, -1, -1);
	v.t = D3DXVECTOR2(cw * 1 / w, ch * 3 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(-1, -1, 1);
	v.t = D3DXVECTOR2(cw * 1 / w, ch * 2 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(1, -1, 1);
	v.t = D3DXVECTOR2(cw * 2 / w, ch * 2 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(-1, -1, -1);
	v.t = D3DXVECTOR2(cw * 1 / w, ch * 3 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(1, -1, 1);
	v.t = D3DXVECTOR2(cw * 2 / w, ch * 2 / h);
	vecVertex.push_back(v);
	v.p = D3DXVECTOR3(1, -1, -1);
	v.t = D3DXVECTOR2(cw * 2 / w, ch * 3 / h);
	vecVertex.push_back(v);

	D3DXCreateMeshFVF(vecVertex.size() / 3, vecVertex.size(), D3DXMESH_MANAGED,
		ST_PT_VERTEX::FVF, DEVICE, &m_pSkyboxMesh);

	ST_PT_VERTEX * vtx = NULL;
	m_pSkyboxMesh->LockVertexBuffer(0, (void**)&vtx);
	memcpy(vtx, &vecVertex[0], sizeof(ST_PT_VERTEX) * vecVertex.size());
	m_pSkyboxMesh->UnlockVertexBuffer();

	WORD * idx = NULL;
	m_pSkyboxMesh->LockIndexBuffer(0, (void**)&idx);
	vector<WORD> vecIndex;
	for (int i = 0; i < vecVertex.size(); i++)
		vecIndex.push_back(i);
	memcpy(idx, &vecIndex[0], sizeof(WORD) * vecIndex.size());
	m_pSkyboxMesh->UnlockIndexBuffer();

	DWORD * adj = NULL;
	m_pSkyboxMesh->LockAttributeBuffer(0, &adj);
	ZeroMemory(adj, sizeof(DWORD) * vecIndex.size() / 3);
	m_pSkyboxMesh->UnlockAttributeBuffer();

	vector<DWORD> adjBuffer(vecIndex.size() * sizeof(DWORD));
	m_pSkyboxMesh->GenerateAdjacency(0.0f, &adjBuffer[0]);
	m_pSkyboxMesh->OptimizeInplace(
		D3DXMESHOPT_ATTRSORT |
		D3DXMESHOPT_COMPACT |
		D3DXMESHOPT_VERTEXCACHE,
		&adjBuffer[0],
		NULL, NULL, NULL);

	D3DXMatrixScaling(&m_matSkybox, MAPSIZE * 4, MAPSIZE * 4, MAPSIZE * 4);
}

void Map::Load(string mapPath)
{
	FILE* fp;
	fopen_s(&fp, mapPath.c_str(), "r");

	if (fp)
		Setup();

	char line[256];
	vector<Model*> vecModel;
	string sLayer[10];
	int nLayer;
	while (true)
	{
		fgets(line, 256, fp);
		if (feof(fp)) break;

		char * context;
		char * tok = strtok_s(line, "\t", &context);
		
		if (tok[0] == 'd')
		{
			tok = strtok_s(NULL, "\t", &context);
			sscanf_s(tok, "%d", &m_nTerrainDetail);
		}

		if (tok[0] == 'l' && !tok[1])
		{
			tok = strtok_s(NULL, "\t", &context);
			sscanf_s(tok, "%d", &nLayer);
		}
		else if (tok[0] == 'l' && tok[1])
		{
			char temp[128];
			int num = tok[1] - '0';
			tok = strtok_s(NULL, "\t", &context);
			sscanf_s(tok, "%s", &temp, 128);
			sLayer[num] = temp;
		}

		if (tok[0] == 'o')
		{
			tok = strtok_s(NULL, "\t", &context);
			string key = string(tok);
			string folder = GetFileFolder((char*)string(tok).c_str());
			string file = GetFileName((char*)string(tok).c_str());
			if (key[key.size() - 1] == 'x' || key[key.size() - 1] == 'X')
				MODELMANAGER->AddModel(key, folder, file, MODELTYPE_STATICX);
			else
				MODELMANAGER->AddModel(key, folder, file, MODELTYPE_OBJ);
			tok = strtok_s(NULL, "\t", &context);
			D3DXVECTOR3 p, r, s;
			int b;
			sscanf_s(tok, "%f %f %f %f %f %f %f %f %f %d",
				&p.x, &p.y, &p.z, &r.x, &r.y, &r.z, &s.x, &s.y, &s.z, &b);

			Model * model = NULL;
			if (key[key.size() - 1] == 'x' || key[key.size() - 1] == 'X')
				model = MODELMANAGER->GetModel(key, MODELTYPE_STATICX);
			else
				model = MODELMANAGER->GetModel(key, MODELTYPE_OBJ);
			model->SetKeyName(key);
			model->SetPosition(p);
			model->SetRotation(r);
			model->SetScale(s);
			model->SetBlend(b);
			model->World();
			vecModel.push_back(model);
		}

		if (tok[0] == 't')
		{
			tok = strtok_s(NULL, "\t\n", &context);
			string name = string(tok);
			TEXTUREMANAGER->AddTexture(name.c_str(), name.c_str());
			m_pTexture = TEXTUREMANAGER->GetTexture(name.c_str());
		}

		if (tok[0] == 'c')
		{
			tok = strtok_s(NULL, "\t", &context);
			D3DXVECTOR3 v0, v1, v2;
			sscanf_s(tok, "%f %f %f %f %f %f %f %f %f",
				&v0.x, &v0.y, &v0.z, &v1.x, &v1.y, &v1.z, &v2.x, &v2.y, &v2.z);
			m_vecSurface.push_back(v0);
			m_vecSurface.push_back(v1);
			m_vecSurface.push_back(v2);
		}

		if (tok[0] == 'w')
		{
			tok = strtok_s(NULL, "\t", &context);
			D3DXVECTOR3 wall;
			sscanf_s(tok, "%f %f %f %f %f %f %f %f %f",
				&wall.x, &wall.y, &wall.z);
			m_vecWall.push_back(wall);
		}

		if (tok[0] == 's')
		{
			if (tok[1] == 'P')
			{
				tok = strtok_s(NULL, "\t", &context);
				sscanf_s(tok, "%f %f %f", &m_vSpawnPlayer.x, &m_vSpawnPlayer.y, &m_vSpawnPlayer.z);
			}
			if (tok[1] == 'B')
			{
				tok = strtok_s(NULL, "\t", &context);
				sscanf_s(tok, "%f %f %f", &m_vSpawnBoss.x, &m_vSpawnBoss.y, &m_vSpawnBoss.z);
			}
			if (tok[1] == 'E')
			{
				D3DXVECTOR3 vE;
				tok = strtok_s(NULL, "\t", &context);
				sscanf_s(tok, "%f %f %f", &vE.x, &vE.y, &vE.z);
				m_vecSpawnEnemy.push_back(vE);
			}
		}
	}

	fclose(fp);

	string namePath = mapPath.substr(0, mapPath.size() - 4);
	for (int i = 0; i < nLayer; i++)
	{
		ST_MAP_LAYER layer;
		layer.pTexture = TEXTUREMANAGER->AddTexture(sLayer[i], sLayer[i]);
		layer.pAlpha = TEXTUREMANAGER->AddTexture(mapPath + "_L" + to_string(i), namePath + "_L" + to_string(i) + ".bmp");
		m_vecLayer.push_back(layer);
	}

	LPDIRECT3DTEXTURE9 heightMap;
	string bmp = mapPath;
	bmp[bmp.size() - 3] = 'b';
	bmp[bmp.size() - 2] = 'm';
	bmp[bmp.size() - 1] = 'p';
	D3DXCreateTextureFromFile(DEVICE, bmp.c_str(), &heightMap);

	D3DLOCKED_RECT lrc;
	heightMap->LockRect(0, &lrc, NULL, 0);
	DWORD* pColorSrc = (DWORD*)lrc.pBits;
	heightMap->UnlockRect(0);

	for (int z = 0; z < MAPSIZE; z++)
	{
		for (int x = 0; x < MAPSIZE; x++)
		{
			int index = z * MAPSIZE + x;
			D3DXCOLOR color = pColorSrc[index];
			int b = color.r * 255.0f;
			int f = color.g * 255.0f;
			int s = color.b * 255.0f;
			m_vecTerrain[index].p.y = b + f * 0.1 + s * 0.01;
		}
	}

	heightMap->Release();

	for (int z = 1; z < MAPSIZE - 1; z++)
	{
		for (int x = 1; x < MAPSIZE - 1; x++)
		{
			D3DXVECTOR3 vL = m_vecTerrain[z * MAPSIZE + x - 1].p;
			D3DXVECTOR3 vR = m_vecTerrain[z * MAPSIZE + x + 1].p;
			D3DXVECTOR3 vD = m_vecTerrain[(z + 1) * MAPSIZE + x].p;
			D3DXVECTOR3 vU = m_vecTerrain[(z - 1) * MAPSIZE + x].p;

			D3DXVECTOR3 vLR = vR - vL;
			D3DXVECTOR3 vDU = vU - vD;
			D3DXVECTOR3 vN;
			D3DXVec3Cross(&vN, &vDU, &vLR);
			D3DXVec3Normalize(&vN, &vN);
			m_vecTerrain[z * MAPSIZE + x].n = vN;
		}
	}

	D3DXCreateMeshFVF(m_vecIndex.size() / 3, m_vecTerrain.size(),
		D3DXMESH_32BIT | D3DXMESH_MANAGED, ST_PNT_VERTEX::FVF, DEVICE, &m_pTerrainMesh);

	ST_PNT_VERTEX * vtx = NULL;
	m_pTerrainMesh->LockVertexBuffer(0, (void**)&vtx);
	memcpy(vtx, &m_vecTerrain[0], sizeof(ST_PNT_VERTEX) * m_vecTerrain.size());
	m_pTerrainMesh->UnlockVertexBuffer();

	DWORD * idx = NULL;
	m_pTerrainMesh->LockIndexBuffer(0, (void**)&idx);
	memcpy(idx, &m_vecIndex[0], sizeof(DWORD) * m_vecIndex.size());
	m_pTerrainMesh->UnlockIndexBuffer();

	DWORD * att = NULL;
	m_pTerrainMesh->LockAttributeBuffer(0, &att);
	ZeroMemory(att, sizeof(DWORD) * m_vecIndex.size() / 3);
	m_pTerrainMesh->UnlockAttributeBuffer();

	vector<DWORD> adjBuffer(m_vecIndex.size());
	m_pTerrainMesh->GenerateAdjacency(0.0f, &adjBuffer[0]);
	m_pTerrainMesh->OptimizeInplace(
		D3DXMESHOPT_ATTRSORT |
		D3DXMESHOPT_COMPACT |
		D3DXMESHOPT_VERTEXCACHE,
		&adjBuffer[0],
		NULL, NULL, NULL);

	for (int i = 0; i < vecModel.size(); i++)
		if (!vecModel[i]->GetAlpha())
			m_vecModel.push_back(vecModel[i]);

	for (int i = 0; i < vecModel.size(); i++)
		if (vecModel[i]->GetAlpha())
			m_vecModel.push_back(vecModel[i]);
}

void Map::Render()
{
	Debug();

	D3DXMATRIX matS;
	D3DXMatrixScaling(&matS, m_nTerrainDetail, m_nTerrainDetail, 1.0f);
	DEVICE->SetTransform(D3DTS_TEXTURE0, &matS);

	DEVICE->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	DEVICE->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	DEVICE->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	DEVICE->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	DEVICE->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	DEVICE->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	DEVICE->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	DEVICE->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	DEVICE->SetRenderState(D3DRS_LIGHTING, true);
	DEVICE->SetTransform(D3DTS_WORLD, &m_matWorld);
	DEVICE->SetTexture(0, m_pTexture);
	DEVICE->SetMaterial(&m_mtl);
	m_pTerrainMesh->DrawSubset(0);

	D3DXMatrixScaling(&matS, 1, 1, 1.0f);
	DEVICE->SetTransform(D3DTS_TEXTURE0, &matS);

	DEVICE->SetRenderState(D3DRS_LIGHTING, false);
	DEVICE->SetTransform(D3DTS_WORLD, &m_matSkybox);
	DEVICE->SetTexture(0, m_pSkyboxTex);
	m_pSkyboxMesh->DrawSubset(0);

	Layer();
}

void Map::Layer()
{
	if (m_vecLayer.empty()) return;
	
	// 기존 값 백업
	DWORD prev[16];
	DEVICE->GetTextureStageState(0, D3DTSS_ALPHAOP, &prev[0]);
	DEVICE->GetTextureStageState(0, D3DTSS_ALPHAARG1, &prev[1]);
	DEVICE->GetTextureStageState(1, D3DTSS_COLOROP, &prev[2]);
	DEVICE->GetTextureStageState(1, D3DTSS_COLORARG1, &prev[3]);
	DEVICE->GetTextureStageState(1, D3DTSS_ALPHAOP, &prev[4]);
	DEVICE->GetTextureStageState(1, D3DTSS_ALPHAARG1, &prev[5]);
	DEVICE->GetSamplerState(1, D3DSAMP_MINFILTER, &prev[6]);
	DEVICE->GetSamplerState(1, D3DSAMP_MAGFILTER, &prev[7]);
	DEVICE->GetSamplerState(1, D3DSAMP_MIPFILTER, &prev[8]);
	DEVICE->GetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, &prev[9]);
	DEVICE->GetSamplerState(0, D3DSAMP_ADDRESSU, &prev[10]);
	DEVICE->GetSamplerState(0, D3DSAMP_ADDRESSV, &prev[11]);
	DEVICE->GetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, &prev[12]);
	DEVICE->GetTextureStageState(1, D3DTSS_TEXCOORDINDEX, &prev[13]);
	DEVICE->GetSamplerState(1, D3DSAMP_ADDRESSU, &prev[14]);
	DEVICE->GetSamplerState(1, D3DSAMP_ADDRESSV, &prev[15]);

	// 설정
	DEVICE->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	DEVICE->SetRenderState(D3DRS_LIGHTING, true);

	DEVICE->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	DEVICE->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	// Texture: take the color from the texture, take the alpha from the previous stage
	DEVICE->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	DEVICE->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	DEVICE->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	DEVICE->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);

	DEVICE->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	DEVICE->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	DEVICE->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	// 0번 인덱스 알파 텍스쳐는 전체에 한번만 그려져야 하므로
	// 타일화 셋팅 하지 않음
	DEVICE->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	DEVICE->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	DEVICE->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	DEVICE->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	DEVICE->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);
	DEVICE->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	DEVICE->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	DEVICE->SetTransform(D3DTS_WORLD, &m_matWorld);

	D3DXMATRIX matS;
	D3DXMatrixScaling(&matS, m_nTerrainDetail, m_nTerrainDetail, 1.0f);
	DEVICE->SetTransform(D3DTS_TEXTURE0, &matS);
	DEVICE->SetTransform(D3DTS_TEXTURE1, &matS);
	DEVICE->SetMaterial(&m_mtl);

	for (int i = 0; i< m_vecLayer.size(); i++)
	{
		DEVICE->SetTexture(0, m_vecLayer[i].pAlpha);
		DEVICE->SetTexture(1, m_vecLayer[i].pTexture);
		m_pTerrainMesh->DrawSubset(0);
	}

	DEVICE->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	// 1번 셋팅을 사용을 중지 한다.
	DEVICE->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	DEVICE->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	// 알파 사용을 중지 한다.
	DEVICE->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	// 설정 복구
	D3DXMatrixScaling(&matS, 1, 1, 1.0f);
	DEVICE->SetTransform(D3DTS_TEXTURE0, &matS);
	DEVICE->SetTransform(D3DTS_TEXTURE1, &matS);
	DEVICE->SetTexture(1, NULL);

	DEVICE->SetTextureStageState(0, D3DTSS_ALPHAOP, prev[0]);
	DEVICE->SetTextureStageState(0, D3DTSS_ALPHAARG1, prev[1]);
	DEVICE->SetTextureStageState(1, D3DTSS_COLOROP, prev[2]);
	DEVICE->SetTextureStageState(1, D3DTSS_COLORARG1, prev[3]);
	DEVICE->SetTextureStageState(1, D3DTSS_ALPHAOP, prev[4]);
	DEVICE->SetTextureStageState(1, D3DTSS_ALPHAARG1, prev[5]);
	DEVICE->SetSamplerState(1, D3DSAMP_MINFILTER, prev[6]);
	DEVICE->SetSamplerState(1, D3DSAMP_MAGFILTER, prev[7]);
	DEVICE->SetSamplerState(1, D3DSAMP_MIPFILTER, prev[8]);
	DEVICE->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, prev[9]);
	DEVICE->SetSamplerState(0, D3DSAMP_ADDRESSU, prev[10]);
	DEVICE->SetSamplerState(0, D3DSAMP_ADDRESSV, prev[11]);
	DEVICE->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, prev[12]);
	DEVICE->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, prev[13]);
	DEVICE->SetSamplerState(1, D3DSAMP_ADDRESSU, prev[14]);
	DEVICE->SetSamplerState(1, D3DSAMP_ADDRESSV, prev[15]);
}

void Map::ObjectRender()
{
	for (int i = 0; i < m_vecModel.size(); i++)
		m_vecModel[i]->Render();
}

void Map::Debug()
{
	if (!g_isDebug) return;

	vector<ST_PC_VERTEX> vecVertex;
	ST_PC_VERTEX v;
	// 네브
	v.c = D3DCOLOR_XRGB(65, 178, 244);
	for (int i = 0; i < m_vecSurface.size(); i++)
	{
		v.p = m_vecSurface[i];
		v.p.y += 0.1;
		vecVertex.push_back(v);
	}
	// 벽
	v.c = D3DCOLOR_XRGB(150, 88, 122);
	for (int i = 0; i < m_vecWall.size(); i++)
	{
		v.p = m_vecWall[i];
		vecVertex.push_back(v);
	}

	if (vecVertex.empty()) return;

	DEVICE->SetRenderState(D3DRS_LIGHTING, false);
	DEVICE->SetTexture(0, NULL);
	DEVICE->SetFVF(ST_PC_VERTEX::FVF);
	DEVICE->SetTransform(D3DTS_WORLD, &m_matWorld);
	DEVICE->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	DEVICE->DrawPrimitiveUP(D3DPT_TRIANGLELIST, vecVertex.size() / 3, &vecVertex[0], sizeof(ST_PC_VERTEX));
	DEVICE->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

float Map::GetHeight(float x, float z)
{
	for (int i = 0; i < m_vecSurface.size(); i += 3)
	{
		D3DXVECTOR3 v0 = m_vecSurface[i];
		D3DXVECTOR3 v1 = m_vecSurface[i + 1];
		D3DXVECTOR3 v2 = m_vecSurface[i + 2];

		float dist;
		if (D3DXIntersectTri(&v0, &v1, &v2, &D3DXVECTOR3(x, 300, z), &D3DXVECTOR3(0, -1, 0), NULL, NULL, &dist))
			return 300 - dist;
	}
	return -1;
}