#include "../../stdafx.h"
#include "DropManager.h"
#include "../Character/Inventory.h"
#include "../Character/CharacterParant.h"
#include "../Character/Pet.h"
#include "ItemManager.h"
#include <time.h>

DropManager::DropManager()
{

}


DropManager::~DropManager()
{
	SAFE_DELETE(m_pModel);
}

void DropManager::Init()
{
	MODELMANAGER->AddModel("DropBox", "Model/Etc/", "DropBox.obj", MODELTYPE_OBJ);
	m_pModel = MODELMANAGER->GetModel("DropBox", MODELTYPE_OBJ);
	m_pModel->SetScale(D3DXVECTOR3(0.1, 0.1, 0.1));
}

void DropManager::AddDropItem(int itemID, D3DXVECTOR3 pos, bool random)
{
	if (itemID == -1) return;

	srand(time(NULL));
	
	int tempItemID = ( rand() % 4) * 100 + itemID % 100;
	
	//아이템이 없으면 1값이 반환되어 반복실행
	//있으면 0이 반환되므로 끝남
	while (ITEMMANAGER->FindItem(tempItemID))
	{
		tempItemID = (rand() % 4) * 100 + itemID % 100;
	}
	/*itemID = 51;*/

	ST_DROPBOX box;
	if (random)
		box.itemID = tempItemID/*itemID*/;
	else
		box.itemID = itemID;
	box.pos = pos;
	box.pos.y += 1;
	box.rot.x = D3DXToRadian(rand() % 360);
	box.rot.y = D3DXToRadian(rand() % 360);
	box.rot.z = D3DXToRadian(rand() % 360);
	m_vecDrop.push_back(box);
}

bool DropManager::GetDropItem(CharacterParant * character, Pet * pet)
{
	for (int i = 0; i < m_vecDrop.size(); i++)
	{
		m_pModel->SetPosition(m_vecDrop[i].pos);
		m_pModel->SetRotation(m_vecDrop[i].rot);

		if (character->GetCharacter()->IsCollisionSphere(m_pModel))
			if (character->GetCharacter()->IsCollisionOBB(m_pModel))
			{
				ItemParent* item = ITEMMANAGER->GetItem(m_vecDrop[i].itemID);
				if (character->Getm_Inventory()->AddItem(item))
				{
					m_vecDrop.erase(m_vecDrop.begin() + i);
					return true;
				}
			}

		if (pet->GetModel())
		{
			if (pet->GetModel()->IsCollisionSphere(m_pModel))
				if (pet->GetModel()->IsCollisionOBB(m_pModel))
				{
					ItemParent* item = ITEMMANAGER->GetItem(m_vecDrop[i].itemID);
					if (character->Getm_Inventory()->AddItem(item))
					{
						m_vecDrop.erase(m_vecDrop.begin() + i);
						return true;
					}
				}
		}
	}

	return false;
}

void DropManager::Render()
{
	for (int i = 0; i < m_vecDrop.size(); i++)
	{
		m_vecDrop[i].rot += D3DXVECTOR3(1, 1, 1) * 0.01;
		m_pModel->SetRotation(m_vecDrop[i].rot);
		m_pModel->SetPosition(m_vecDrop[i].pos +
			D3DXVECTOR3(0, sin(m_vecDrop[i].rot.x), 0) * 0.5);
		m_pModel->World();
		m_pModel->Render();
	}
}