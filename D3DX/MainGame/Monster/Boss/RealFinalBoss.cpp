#include "../MonsterUseHeader.h"
#include "../Magic/MagicCircle.h"

#include "RealFinalboss.h"


RealFinalboss::RealFinalboss()
{
}


RealFinalboss::~RealFinalboss()
{
}

void RealFinalboss::SetupBoss(Map* map, D3DXVECTOR3 pos)
{
	//�� �ٲ����
	m_pModel = MODELMANAGER->GetModel("����", MODELTYPE_X);

	BossParent::SetupBoss(map, pos);

	m_eBossState = BS_ENTER;
	ChangeAni();
	//���� �ڽ� 
	ST_SIZEBOX box;
	box.highX = 50.0f;
	box.highY = 180.0f;
	box.highZ = 50.0f;
	box.lowX = -50.0f;
	box.lowY = 10.0f;
	box.lowZ = -50.0f;


	m_pModel->SetScale(D3DXVECTOR3(0.02f, 0.02f, 0.02f));

	m_pModel->CreateBound(box);
	m_pModel->SetBoundSphere(m_pModel->GetOrigBoundSphere().center, 100.0f);

	//���� �ٲ����
	m_pSkill = SKILL->GetSkill("Boss Skill1");
	//3Ÿ��ų �ٲ����
	m_pSkill2 = SKILL->GetSkill("Boss Skill2");

	//2��ų �¾�
	SetupSkill2();

	m_bSkill2Use = m_bUsingSkill = false;
	m_bIsTargeting = true;

}

void RealFinalboss::SetupStat()
{
	m_nMinMoveCount = 0;
	m_nMaxMoveCount = 0;

	ZeroMemory(m_pMonsterStat, sizeof(STATUS));

	CURRENTHP(m_pMonsterStat) = 1400;
	MAXHP(m_pMonsterStat) = 1500;
	ATK(m_pMonsterStat) = 20;
	PHYRATE(m_pMonsterStat) = 1.3f;
	MAGICRATE(m_pMonsterStat) = 0.8f;
	CHERATE(m_pMonsterStat) = 1.1f;
	ATKSPEED(m_pMonsterStat) = 0.75f;

	DEF(m_pMonsterStat) = 5;
	AGI(m_pMonsterStat) = 10.0f;
	HIT(m_pMonsterStat) = 10.0f;
	SPEED(m_pMonsterStat) = 0.9f;
	RANGE(m_pMonsterStat) = 6.0f;
}

void RealFinalboss::SetupSkill()
{
	ZeroMemory(&m_stSkill, sizeof(m_stSkill));

	m_stSkill.nMaxTarget = 1;
	m_stSkill.fMinLength = 0;
	m_stSkill.fMaxLength = 10;
	m_stSkill.fAngle = 360;

	m_stSkill.fDamage = 100; //v
	m_stSkill.nDamageCount = 5;
	m_stSkill.fDamageInterval =
		((m_pModel->GetAnimationPeriod("SKILL2") * 3) / 4) / m_stSkill.nDamageCount;
	m_stSkill.fDamageDelay = 0;

	m_stSkill.fBuffTime = -1;//<0;



							 //m_stSkill.fYOffset ;
							 //m_stSkill.isAutoRot;
							 //m_stSkill.fParticleTime;
							 //m_stSkill.fParticleSpeed;
	m_stSkill.fEffectTime = m_pModel->GetAnimationPeriod("SKILL2") / 4;

	m_stSkill.buffStatus.chr.nCurrentHP = 100; //���� �� ���ȷ� �Ǖ��� ���θ޸�;

	m_fSkillCoolTimeCount = 0;
	m_nSkillCooltime = 200;
}

void RealFinalboss::SetupSkill2()
{
	ZeroMemory(&m_stSkill2, sizeof(m_stSkill2));

	m_stSkill2.nMaxTarget = 1;
	m_stSkill2.fMinLength = 0;
	m_stSkill2.fMaxLength = 100;
	m_stSkill2.fAngle = 360;

	m_stSkill2.fDamage = 100; //v
	m_stSkill2.nDamageCount = 1;
	m_stSkill2.fDamageInterval = 0;
	m_stSkill2.fDamageDelay = 0;

	m_stSkill2.fBuffTime = -1;//<0;

	m_fSkillCoolTimeCount2 = 0;
	m_nSkillCooltime2 = 30;



	//m_stSkill.fYOffset ;
	//m_stSkill.isAutoRot;
	//m_stSkill.fParticleTime;
	//m_stSkill.fParticleSpeed;
	m_stSkill.fEffectTime = 0;

	//	m_stSkill.buffStatus.chr.nCurrentHP = 100; //���� �� ���ȷ� �Ǖ��� ���θ޸�;
}

//void RealFinalboss::ChangeAni()
//{
//	switch (m_eBossState)
//	{
//	case BS_ENTER:
//		m_pModel->SetAnimation("ENTER");
//		break;
//	case BS_IDLE:
//		m_pModel->SetAnimation("IDLE");
//		break;
//	case BS_RUN:
//		m_pModel->SetAnimation("RUN");
//		break;
//	case BS_PASSIVE:
//		m_pModel->SetAnimation("PASSIVE");
//		break;
//	case BS_ATTACK:
//		m_pModel->SetAnimation("ATTACK");
//		break;
//	case BS_SKILL1:
//		m_pModel->SetAnimation("SKILL2");
//		break;
//	case BS_SKILL2:
//		m_pModel->SetAnimation("SKILL1");
//		break;
//	case BS_CASTING:
//		m_pModel->SetAnimation("SKILL2_CASTING");
//		break;
//	case BS_DIE:
//		m_pModel->SetAnimation("DIE");
//		break;
//	case BS_NONE:
//		break;
//	default:
//		break;
//	}
//
//	if (m_eBossState == BS_CASTING)
//	{
//		m_pModel->SetAnimationSpeed(0.5f);
//	}
//	else m_pModel->SetAnimationSpeed(1.0f);
//}

//void RealFinalboss::Pattern()
//{
//	if (AbleSkill() && !m_bSkill2Use)
//	{
//		m_eBossState = BS_CASTING;
//		ChangeAni();
//	}
//
//	else if (AbleSkill2())
//	{
//		m_eBossState = BS_SKILL2;
//		ChangeAni();
//	}
//
//	switch (m_eBossState)
//	{
//	case BS_ENTER:
//	{
//		if (m_pModel->IsAnimationEnd())
//		{
//			m_eBossState = BS_RUN;
//			ChangeAni();
//		}
//	}
//	break;
//	case BS_RUN:
//	{
//		Move();
//	}
//	break;
//	case BS_PASSIVE:
//		if (m_pModel->IsAnimationEnd())
//		{
//			m_eBossState = BS_RUN;
//			ChangeAni();
//		}
//		break;
//	case BS_ATTACK:
//		Attack();
//		break;
//	case BS_SKILL1:
//		SkillUse();
//		break;
//	case BS_SKILL2:
//		Skill2();
//		break;
//	case BS_CASTING:
//		Casting();
//		break;
//	case BS_DIE:
//	{
//		if (m_pModel->IsAnimationEnd()) m_eBossState = BS_NONE;
//	}
//	default:
//		break;
//	}
//}

void RealFinalboss::Attack()
{
	if (PCHARACTER->GetIsDead())
	{
		m_eState = MS_IDLE;
		ChangeAni();
		return;
	}

	//if (AbleSummon())
	//{
	//	m_eBossState = BS_PASSIVE;
	//	ChangeAni();
	//	Passive();
	//
	//	//���� ��ŵ�ϰ� �нú� �ߵ�
	//	return;
	//}

	float length = GetDistance(*m_pModel->GetPosition(), *CHARACTER->GetPosition());

	//���� ����߿� �÷��̾ ����� ���ݸ�� �� ���������ض�
	if (length > RANGE(m_pMonsterStat) && !m_bIsAttack)
	{
		if (m_eBossState == BS_ATTACK)
		{
			m_eBossState = BS_RUN;
			ChangeAni();
		}
		return;

	}

	//��Ÿ� �ȿ� ���Դµ� ���� ������ �����ҷ��� �Ѵٸ�
	if (!m_bIsAttack)
	{
		//���� �� ������ ���ϰ�		
		ChangeRot();

		//ù ������ ���� ���������� �ٲ������μ� ������ ȸ���� ���´�.
		m_bIsAttack = true;

		//�÷��̾� ��ġ �������� �� ����
		m_pMagicCircle->SetPosAndRad(*CHARACTER->GetPosition(), 3);
	}

	//�÷��̾� ���ݱ�� ����
	if (m_pModel->IsAnimationPercent(ATKSPEED(m_pMonsterStat)))
	{
		if (m_pMagicCircle->PlayerCollision(
			*CHARACTER->GetPosition(),
			CHARACTER->GetBoundSphere().radius))
		{

			float tatalRate = PHYRATE(m_pMonsterStat) + MAGICRATE(m_pMonsterStat) + CHERATE(m_pMonsterStat);
			float tatalDamage = tatalRate * ATK(m_pMonsterStat);
			PCHARACTER->CalculDamage(tatalDamage);
		}


		//float tatalRate = PHYRATE(m_pMonsterStat) + MAGICRATE(m_pMonsterStat) + CHERATE(m_pMonsterStat);
		//float tatalDamage = tatalRate * ATK(m_pMonsterStat);
		//PCHARACTER->CalculDamage(tatalDamage);
	}
	if (m_pModel->IsAnimationEnd())
	{
		m_bIsAttack = false;
	}
}

void RealFinalboss::Move()
{
	//if (AbleSummon())
	//{
	//	m_eBossState = BS_PASSIVE;
	//	ChangeAni();
	//	Passive();
	//
	//	//�̵� ���߰� �����ȯ
	//	return;
	//}
	if (m_eBossState == BS_RUN)
	{
		m_vDir = *CHARACTER->GetPosition() - *m_pModel->GetPosition();
		D3DXVec3Normalize(&m_vDir, &m_vDir);
		D3DXVECTOR3 tempPos = *m_pModel->GetPosition() + m_vDir * SPEED(m_pMonsterStat);
		tempPos.y = m_pMap->GetHeight(tempPos.x, tempPos.z);

		ChangeRot();

		m_pModel->SetPosition(tempPos);

		float length = GetDistance(*m_pModel->GetPosition(), *CHARACTER->GetPosition());

		if (length < RANGE(m_pMonsterStat))
		{
			m_eBossState = BS_ATTACK;
			ChangeAni();
		}
	}
}

void RealFinalboss::Passive()
{

	//m_pMM->MakeMonster();


}

//bool RealFinalboss::AbleSummon()
//{
//	//if (summonCount <= 0) return false;
//	////���� HP ����
//	//float HPRatio = (float)CURRENTHP(m_pMonsterStat) / MAXHP(m_pMonsterStat);
//	//float summonRatio = (10 * summonCount / 100.0f);
//	////���� �� ������ ��ȯī��Ʈ �������� �������ٸ�
//	//if (HPRatio <= summonRatio)
//	//{
//	//	summonCount--;
//	//	return true;
//	//}
//	//
//	//return false;
//}
void RealFinalboss::SkillUse()
{
	m_pSkill->Trigger();

	if (m_pModel->IsAnimationEnd())
	{
		m_bUsingSkill = false;
		m_fSkillCoolTimeCount = 0;
		m_eBossState = BS_RUN;
		ChangeAni();
	}
}

void RealFinalboss::Skill2()
{
	vector<MonsterParent*> tt;


	if (m_pModel->IsAnimationPercent(0.3))
	{

		m_pSkill->Trigger();
	}

	if (m_pModel->IsAnimationPercent(0.6))
	{
		SkillPrepare2();
		m_pSkill->Trigger();
	}

	if (m_pModel->IsAnimationPercent(0.8))
	{
		SkillPrepare2();
		m_pSkill->Trigger();
	}

	if (m_pModel->IsAnimationEnd())
	{
		m_bSkill2Use = false;

		m_fSkillCoolTimeCount2 = 0;
		m_eBossState = BS_RUN;
		ChangeAni();
	}
}

//void RealFinalboss::Casting()
//{
//	if (m_pModel->IsAnimationEnd())
//		//if (m_pModel->IsAnimationPercent(0.5f))
//	{
//		m_eBossState = BS_SKILL1;
//		ChangeAni();
//		//	m_pModel->SetAnimationPosition(0.5f);
//	}
//}