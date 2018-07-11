#include "../MonsterUseHeader.h"
#include "BossParent.h"


BossParent::BossParent()
{
}


BossParent::~BossParent()
{

}



void BossParent::SetupBoss(Map* map, D3DXVECTOR3 pos)
{
	MonsterParent::SetupBoss(map, pos);


}

void BossParent::Update()
{
	if (m_pModel)
	{
		m_pModel->World();
		m_pModel->Update();
	}

	if (INPUT->KeyDown('L'))
	{
		CURRENTHP(m_uMonsterStat) -= 100;
	}

	if (m_pHPBar)
	{
		float tempF = (float)CURRENTHP(m_uMonsterStat) / MAXHP(m_uMonsterStat);


		m_pHPBar->SetScale(D3DXVECTOR3(tempF, 1, 1));

		m_pHPBar->Update();
	}


	Pattern();

}
		//1536
void BossParent::Render()
{
	if (m_pModel)
	{
		m_pModel->Render();
		
		if (m_bIsTargeting) m_pHPBar->Render();
	}

	if (m_pHPBar) m_pHPBar->Render();

}


void BossParent::SetupStat()
{

}


void BossParent::Attack()
{

}

void BossParent::Skill()
{

}

void BossParent::Move()
{

}


void BossParent::DropItemSetup()
{

}

void BossParent::ChangeAni()
{

}
void BossParent::Pattern()
{

}