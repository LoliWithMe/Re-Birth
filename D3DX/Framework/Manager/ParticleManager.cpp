#include "../../stdafx.h"
#include "ParticleManager.h"


ParticleManager::ParticleManager()
{
}


ParticleManager::~ParticleManager()
{
}

ParticleSystem::ParticleSystem()
	: m_pTexture(NULL)
	, m_pVB(NULL)
{
}

ParticleSystem::~ParticleSystem()
{
	SAFE_RELEASE(m_pVB);
	for (auto a : m_lAttribute)
		SAFE_DELETE(a);
	SAFE_DELETE(m_pApplyAttribute);
	SAFE_DELETE(m_pOrigAttribute);
	SAFE_DELETE(m_pApplyVarAttribute);
	SAFE_DELETE(m_pOrigVarAttribute);
}

void ParticleSystem::Init(LPDIRECT3DTEXTURE9 texture, float size, int count,
	ST_PARTICLE_ATTRIBUTE * orig, ST_PARTICLE_ATTRIBUTE_VARIABLE * var)
{
	m_pTexture = texture;
	m_fParticleSize = size;
	m_nParticleCount = count;

	m_nVBSize = 2048;
	m_nVBBatchSize = m_nVBSize / 4;
	m_nVBOffset = 0;

	SAFE_DELETE(m_pVB);
	DEVICE->CreateVertexBuffer(
		m_nVBSize * sizeof(ST_PARTICLE),
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
		ST_PARTICLE::FVF,
		D3DPOOL_DEFAULT,
		&m_pVB, NULL);

	m_pOrigAttribute = orig;
	m_pApplyAttribute = new ST_PARTICLE_ATTRIBUTE;
	*m_pApplyAttribute = *orig;

	m_pOrigVarAttribute = var;
	m_pApplyVarAttribute = new ST_PARTICLE_ATTRIBUTE_VARIABLE;
	*m_pApplyVarAttribute = *var;

	m_isRegen = true;

	D3DXMatrixIdentity(&m_matWorld);

	for (int i = 0; i < m_nParticleCount; i++)
		m_lAttribute.push_back(new ST_PARTICLE_ATTRIBUTE(ResetParticle(0)));

	// 시간 재설정
	TimeReset();
}

ST_PARTICLE_ATTRIBUTE ParticleSystem::ResetParticle(int loop)
{
	ST_PARTICLE_ATTRIBUTE att = *m_pApplyAttribute;
	ST_PARTICLE_ATTRIBUTE_VARIABLE var = *m_pApplyVarAttribute;

	att.nLoop = loop;
	att.nLoop++;
	if (att.nLoop > att.nMaxLoop && att.nMaxLoop > 0 || !m_isRegen)
		att.isAlive = false;
	else
		att.isAlive = true;

	att.fLifeTime += FRand(-var.fLifeTimeVar, var.fLifeTimeVar);

	att.vPos.x += FRand(-var.vPosVar.x, var.vPosVar.x);
	att.vPos.y += FRand(-var.vPosVar.y, var.vPosVar.y);
	att.vPos.z += FRand(-var.vPosVar.z, var.vPosVar.z);
	
	att.vVelocity.x += FRand(-var.vVelocityVar.x, var.vVelocityVar.x);
	att.vVelocity.y += FRand(-var.vVelocityVar.y, var.vVelocityVar.y);
	att.vVelocity.z += FRand(-var.vVelocityVar.z, var.vVelocityVar.z);

	att.vAcceleration.x += FRand(-var.vAccelerationVar.x, var.vAccelerationVar.x);
	att.vAcceleration.y += FRand(-var.vAccelerationVar.y, var.vAccelerationVar.y);
	att.vAcceleration.z += FRand(-var.vAccelerationVar.z, var.vAccelerationVar.z);

	att.fStartRadius += FRand(-var.fStartRadiusVar, var.fStartRadiusVar);
	att.fEndRadius += FRand(-var.fEndRadiusVar, var.fEndRadiusVar);
	att.fRadiusSpeed += FRand(-var.fRadiusSpeedVar, var.fRadiusSpeedVar);

	return att;
}

void ParticleSystem::TimeReset()
{
	m_isDie = false;
	auto iter = m_lAttribute.begin();
	for (; iter != m_lAttribute.end(); iter++)
	{
		(*iter)->nLoop = 0;
		(*iter)->isAlive = true;
	}

	iter = m_lAttribute.begin();
	for (;; iter++)
	{
		if (iter == m_lAttribute.end())
			iter = m_lAttribute.begin();
		(*iter)->fAge += 0.1;
		if ((*iter)->fAge > (*iter)->fLifeTime)
			break;
	}
}

void ParticleSystem::Update()
{
	auto iter = m_lAttribute.begin();
	int count = 0;
	if (m_isDie) return;
	for (; iter != m_lAttribute.end(); iter++)
	{
		if (!(*iter)->isAlive) continue;
		count++;

		float dist = D3DXVec3Length(&((*iter)->vGravityPos - (*iter)->vPos));
		if (dist > 1.0f && dist < (*iter)->fGravityRadius)
		{
			D3DXVECTOR3 dir = (*iter)->vGravityPos - (*iter)->vPos;
			D3DXVec3Normalize(&dir, &dir);
			float f = (*iter)->fGravityForce * (pow(m_fParticleSize, 2) / pow(dist, 2));
			(*iter)->vGravity += dir * f;
		}

		(*iter)->vPos +=
			(*iter)->vVelocity * TIME->GetElapsedTime() +
			(*iter)->vAcceleration * TIME->GetElapsedTime() +
			(*iter)->vGravity * TIME->GetElapsedTime();

		(*iter)->vAcceleration += (*iter)->vAcceleration * TIME->GetElapsedTime();

		(*iter)->fCurrentRadiusSpeed += (*iter)->fRadiusSpeed * TIME->GetElapsedTime();

		(*iter)->fAge += TIME->GetElapsedTime();

		if ((*iter)->fAge > (*iter)->fLifeTime)
			*(*iter) = ResetParticle((*iter)->nLoop);
	}
	if (count == 0)
		m_isDie = true;
	else
		m_isDie = false;
}

void ParticleSystem::ForceUpdate(int count, float time)
{
	for (int i = 0; i < count; i++)
	{
		auto iter = m_lAttribute.begin();
		for (; iter != m_lAttribute.end(); iter++)
		{
			if (!(*iter)->isAlive) continue;

			float dist = D3DXVec3Length(&((*iter)->vGravityPos - (*iter)->vPos));
			if (dist > 1.0f && dist < (*iter)->fGravityRadius)
			{
				D3DXVECTOR3 dir = (*iter)->vGravityPos - (*iter)->vPos;
				D3DXVec3Normalize(&dir, &dir);
				float f = (*iter)->fGravityForce * (m_fParticleSize / pow(dist, 2));
				(*iter)->vGravity += dir * f;
			}

			(*iter)->vPos +=
				(*iter)->vVelocity * time +
				(*iter)->vAcceleration * time +
				(*iter)->vGravity * time;

			(*iter)->vAcceleration += (*iter)->vAcceleration * time;

			(*iter)->fCurrentRadiusSpeed += (*iter)->fRadiusSpeed * time;

			(*iter)->fAge += time;

			if ((*iter)->fAge > (*iter)->fLifeTime)
				*(*iter) = ResetParticle((*iter)->nLoop);
		}
	}
}

void ParticleSystem::PreRender()
{
	DEVICE->SetRenderState(D3DRS_LIGHTING, false);
	DEVICE->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	DEVICE->SetRenderState(D3DRS_POINTSCALEENABLE, true);
	DEVICE->SetRenderState(D3DRS_ZWRITEENABLE, false);
	DEVICE->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

	DEVICE->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	DEVICE->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	DEVICE->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	DEVICE->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	DEVICE->SetTextureStageState(0, D3DTSS_ALPHAARG0, D3DTA_DIFFUSE);
	DEVICE->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	DWORD b = FtoDw(m_fParticleSize);
	DEVICE->SetRenderState(D3DRS_POINTSIZE, b);

	DEVICE->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDw(0.0));
	DEVICE->SetRenderState(D3DRS_POINTSCALE_A, FtoDw(0.0));
	DEVICE->SetRenderState(D3DRS_POINTSCALE_B, FtoDw(0.0));
	DEVICE->SetRenderState(D3DRS_POINTSCALE_C, FtoDw(1.0));
}

void ParticleSystem::Render()
{
	if (m_isDie) return;
	PreRender();

	DEVICE->SetTransform(D3DTS_WORLD, &m_matWorld);
	DEVICE->SetTexture(0, m_pTexture);
	DEVICE->SetFVF(ST_PARTICLE::FVF);
	DEVICE->SetStreamSource(0, m_pVB, 0, sizeof(ST_PARTICLE));

	if (m_nVBOffset >= m_nVBSize)
		m_nVBOffset = 0;

	ST_PARTICLE * pV = NULL;
	m_pVB->Lock(
		m_nVBOffset * sizeof(ST_PARTICLE),
		m_nVBBatchSize * sizeof(ST_PARTICLE),
		(void**)&pV,
		m_nVBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

	int numParticleInBatch = 0;

	auto iter = m_lAttribute.begin();
	for (; iter != m_lAttribute.end(); iter++)
	{
		if ((*iter)->isAlive && (*iter)->nLoop > 0)
		{
			float rate = (*iter)->fAge / (*iter)->fLifeTime;

			float radius = (*iter)->fStartRadius + (*iter)->fEndRadius * rate;
			D3DXVECTOR3 vRad = D3DXVECTOR3(0, 0, 0);
			vRad.x = cos((*iter)->fCurrentRadiusSpeed) * radius;
			vRad.z = -sin((*iter)->fCurrentRadiusSpeed) * radius;
			pV->p = (*iter)->vPos + vRad;

			D3DXCOLOR currentColor = (*iter)->colorFade - (*iter)->color;
			currentColor *= rate;
			currentColor += (*iter)->color;
			pV->c = D3DCOLOR_ARGB(
				(int)(255 - 255 * rate), 
				(int)(currentColor.r * 255),
				(int)(currentColor.g * 255),
				(int)(currentColor.b * 255));

			pV++;

			numParticleInBatch++;

			if (numParticleInBatch == m_nVBBatchSize)
			{
				m_pVB->Unlock();

				DEVICE->DrawPrimitive(D3DPT_POINTLIST, m_nVBOffset, m_nVBBatchSize);
				
				m_nVBOffset += m_nVBBatchSize;

				if (m_nVBOffset >= m_nVBSize)
					m_nVBOffset = 0;

				m_pVB->Lock(
					m_nVBOffset * sizeof(ST_PARTICLE),
					m_nVBBatchSize * sizeof(ST_PARTICLE),
					(void**)&pV,
					m_nVBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

				numParticleInBatch = 0;
			}
		}
	}

	m_pVB->Unlock();

	if (numParticleInBatch)
		DEVICE->DrawPrimitive(D3DPT_POINTLIST, m_nVBOffset, numParticleInBatch);

	m_nVBOffset += m_nVBBatchSize;

	PostRender();
}

void ParticleSystem::PostRender()
{
	DEVICE->SetRenderState(D3DRS_LIGHTING, true);
	DEVICE->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	DEVICE->SetRenderState(D3DRS_POINTSCALEENABLE, false);
	DEVICE->SetRenderState(D3DRS_ZWRITEENABLE, true);
	DEVICE->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	DEVICE->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	DEVICE->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	DEVICE->SetTextureStageState(0, D3DTSS_ALPHAARG0, D3DTA_CURRENT);
	DEVICE->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
}

void ParticleManager::AddParticle(string keyName, LPDIRECT3DTEXTURE9 texture, float size, int count, 
	ST_PARTICLE_ATTRIBUTE orig, ST_PARTICLE_ATTRIBUTE_VARIABLE var)
{
	if (m_mapParticle.find(keyName) != m_mapParticle.end()) return;
	ST_PARTICLE_INFO info;
	info.fParticleSize = size;
	info.nParticleCount = count;
	info.origAttribute = orig;
	info.varAttribute = var;
	info.pTexture = texture;
	m_mapParticle.insert(make_pair(keyName, info));
}

void ParticleManager::AddParticle(string keyName, LPDIRECT3DTEXTURE9 texture, string fullPath)
{
	if (m_mapParticle.find(keyName) != m_mapParticle.end()) return;

	FILE * fp;
	fopen_s(&fp, fullPath.c_str(), "r");

	ST_PARTICLE_ATTRIBUTE orig;
	ST_PARTICLE_ATTRIBUTE_VARIABLE var;
	int count;
	float size;

	ZeroMemory(&orig, sizeof(ST_PARTICLE_ATTRIBUTE));
	ZeroMemory(&var, sizeof(ST_PARTICLE_ATTRIBUTE_VARIABLE));

	char line[256];
	while (true)
	{
		fgets(line, 256, fp);
		if (feof(fp)) break;

		char * context;
		char * tok = strtok_s(line, "\t\n", &context);
		if (strcmp(tok, "A_CNT") == 0)
		{
			tok = strtok_s(NULL, "\t\n", &context);
			sscanf_s(tok, "%d", &count);
		}
		if (strcmp(tok, "A_SIZ") == 0)
		{
			tok = strtok_s(NULL, "\t\n", &context);
			sscanf_s(tok, "%f", &size);
		}
		if (strcmp(tok, "A_POS") == 0)
		{
			tok = strtok_s(NULL, "\t\n", &context);
			sscanf_s(tok, "%f %f %f", &orig.vPos.x, &orig.vPos.y, &orig.vPos.z);
		}
		if (strcmp(tok, "A_VEL") == 0)
		{
			tok = strtok_s(NULL, "\t\n", &context);
			sscanf_s(tok, "%f %f %f", &orig.vVelocity.x, &orig.vVelocity.y, &orig.vVelocity.z);
		}
		if (strcmp(tok, "A_ACL") == 0)
		{
			tok = strtok_s(NULL, "\t\n", &context);
			sscanf_s(tok, "%f %f %f", &orig.vAcceleration.x, &orig.vAcceleration.y, &orig.vAcceleration.z);
		}
		if (strcmp(tok, "A_GRV") == 0)
		{
			tok = strtok_s(NULL, "\t\n", &context);
			sscanf_s(tok, "%f %f %f %f %f", &orig.vGravityPos.x, &orig.vGravityPos.y, &orig.vGravityPos.z,
				&orig.fGravityForce, &orig.fGravityRadius);
		}
		if (strcmp(tok, "A_STR") == 0)
		{
			tok = strtok_s(NULL, "\t\n", &context);
			sscanf_s(tok, "%f", &orig.fStartRadius);
		}
		if (strcmp(tok, "A_EDR") == 0)
		{
			tok = strtok_s(NULL, "\t\n", &context);
			sscanf_s(tok, "%f", &orig.fEndRadius);
		}
		if (strcmp(tok, "A_RSD") == 0)
		{
			tok = strtok_s(NULL, "\t\n", &context);
			sscanf_s(tok, "%f", &orig.fRadiusSpeed);
		}
		if (strcmp(tok, "A_LIF") == 0)
		{
			tok = strtok_s(NULL, "\t\n", &context);
			sscanf_s(tok, "%f", &orig.fLifeTime);
		}
		if (strcmp(tok, "A_COL") == 0)
		{
			tok = strtok_s(NULL, "\t\n", &context);
			DWORD temp;
			sscanf_s(tok, "%d", &temp);
			orig.color = D3DXCOLOR(temp);
		}
		if (strcmp(tok, "A_CLF") == 0)
		{
			tok = strtok_s(NULL, "\t\n", &context);
			DWORD temp;
			sscanf_s(tok, "%d", &temp);
			orig.colorFade = D3DXCOLOR(temp);
		}
		if (strcmp(tok, "A_LOP") == 0)
		{
			tok = strtok_s(NULL, "\t\n", &context);
			sscanf_s(tok, "%d", &orig.nMaxLoop);
		}

		if (strcmp(tok, "V_POS") == 0)
		{
			tok = strtok_s(NULL, "\t\n", &context);
			sscanf_s(tok, "%f %f %f", &var.vPosVar.x, &var.vPosVar.y, &var.vPosVar.z);
		}
		if (strcmp(tok, "V_VEL") == 0)
		{
			tok = strtok_s(NULL, "\t\n", &context);
			sscanf_s(tok, "%f %f %f", &var.vVelocityVar.x, &var.vVelocityVar.y, &var.vVelocityVar.z);
		}
		if (strcmp(tok, "V_ACL") == 0)
		{
			tok = strtok_s(NULL, "\t\n", &context);
			sscanf_s(tok, "%f %f %f", &var.vAccelerationVar.x, &var.vAccelerationVar.y, &var.vAccelerationVar.z);
		}
		if (strcmp(tok, "V_STR") == 0)
		{
			tok = strtok_s(NULL, "\t\n", &context);
			sscanf_s(tok, "%f", &var.fStartRadiusVar);
		}
		if (strcmp(tok, "V_EDR") == 0)
		{
			tok = strtok_s(NULL, "\t\n", &context);
			sscanf_s(tok, "%f", &var.fEndRadiusVar);
		}
		if (strcmp(tok, "V_RSD") == 0)
		{
			tok = strtok_s(NULL, "\t\n", &context);
			sscanf_s(tok, "%f", &var.fRadiusSpeedVar);
		}
		if (strcmp(tok, "V_LIF") == 0)
		{
			tok = strtok_s(NULL, "\t\n", &context);
			sscanf_s(tok, "%f", &var.fLifeTimeVar);
		}
	}

	fclose(fp);

	ST_PARTICLE_INFO info;
	info.fParticleSize = size;
	info.nParticleCount = count;
	info.pTexture = texture;
	info.origAttribute = orig;
	info.varAttribute = var;

	m_mapParticle.insert(make_pair(keyName, info));
}

Particle * ParticleManager::GetParticle(string keyName)
{
	if (m_mapParticle.find(keyName) == m_mapParticle.end())
		return NULL;

	Particle * particle = new Particle;
	particle->Init(&m_mapParticle[keyName]);
	return particle;
}

void ParticleManager::Release()
{
	m_mapParticle.clear();
}

Particle::Particle()
	: m_pParticleSystem(NULL)
{
	m_vPosition = D3DXVECTOR3(0, 0, 0);
	m_vRotation = D3DXVECTOR3(0, 0, 0);
	m_vScale = D3DXVECTOR3(1, 1, 1);
}

Particle::~Particle()
{
	SAFE_DELETE(m_pParticleSystem);
}

void Particle::Init(ST_PARTICLE_INFO * info)
{
	SAFE_DELETE(m_pParticleSystem);
	m_pParticleSystem = new ParticleSystem;
	ST_PARTICLE_ATTRIBUTE * orig = new ST_PARTICLE_ATTRIBUTE;
	ST_PARTICLE_ATTRIBUTE_VARIABLE * var = new ST_PARTICLE_ATTRIBUTE_VARIABLE;
	*orig = info->origAttribute;
	*var = info->varAttribute;
	m_pParticleSystem->Init(info->pTexture, info->fParticleSize, info->nParticleCount,
		orig, var);
}

void Particle::World()
{
	D3DXMATRIX matS, matR, matT;

	D3DXMatrixScaling(&matS, m_vScale.x, m_vScale.y, m_vScale.z);
	D3DXMatrixRotationYawPitchRoll(&matR, m_vRotation.y, m_vRotation.x, m_vRotation.z);
	D3DXMatrixTranslation(&matT, m_vPosition.x, m_vPosition.y, m_vPosition.z);

	m_pParticleSystem->SetWorld(matS * matR * matT);
	auto orig = m_pParticleSystem->GetOrig();
	auto var = m_pParticleSystem->GetVarOrig();
	m_pParticleSystem->SetApply(orig, var);
}

void Particle::ApplyWorld()
{
	D3DXMATRIX identity;
	D3DXMatrixIdentity(&identity);
	m_pParticleSystem->SetWorld(identity);

	D3DXMATRIX matS, matR, matT, matWorld;

	D3DXMatrixScaling(&matS, m_vScale.x, m_vScale.y, m_vScale.z);
	D3DXMatrixRotationYawPitchRoll(&matR, m_vRotation.y, m_vRotation.x, m_vRotation.z);
	D3DXMatrixTranslation(&matT, m_vPosition.x, m_vPosition.y, m_vPosition.z);
	matWorld = matS * matR * matT;

	auto orig = m_pParticleSystem->GetOrig();
	auto var = m_pParticleSystem->GetVarOrig();

	D3DXVec3TransformCoord(&orig.vPos, &orig.vPos, &matWorld);
	D3DXVec3TransformCoord(&orig.vVelocity, &orig.vVelocity, &matR);
	D3DXVec3TransformCoord(&orig.vAcceleration, &orig.vAcceleration, &matR);

	D3DXVec3TransformCoord(&var.vPosVar, &var.vPosVar, &matR);
	D3DXVec3TransformCoord(&var.vVelocityVar, &var.vVelocityVar, &matR);
	D3DXVec3TransformCoord(&var.vAccelerationVar, &var.vAccelerationVar, &matR);

	m_pParticleSystem->SetApply(orig, var);
}
