#pragma once
#include "MagicMonster.h"

class NifilHeim :
	public MagicMonster
{

private:
	virtual void SetupStat() override;
	virtual void SetupSkill() override;

	virtual void SkillUse() override;

	virtual void DropItemSetup() override;

public:
	NifilHeim();
	virtual ~NifilHeim();

	virtual void Setup(Map* map, D3DXVECTOR3 spawnPos, bool isSummon = false) override;
	
};

