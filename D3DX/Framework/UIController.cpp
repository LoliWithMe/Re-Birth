#include "../stdafx.h"
#include "UIController.h"



void UIObject::FindAllChild(vector<UIObject*>& vecChild)
{
	vecChild.push_back(this);

	for (auto child : m_vecChild)
		child->FindAllChild(vecChild);
}

UIObject::UIObject()
	: m_pParent(NULL)
	, m_pFunction(NULL)
	, m_vPosition(D3DXVECTOR3(0, 0, 0))
	, m_vScale(D3DXVECTOR3(1, 1, 1))
	, m_eAnchor(UIAC_LT)
	, m_vColor(255, 255, 255)
{
	for (int i = 0; i < UITEX_END; i++)
		m_pTex[i] = NULL;
}


UIObject::~UIObject()
{
}

void UIObject::Update()
{
	float width = m_ptTexWH[UITEX_NORMAL].x * m_vScale.x;
	float height = m_ptTexWH[UITEX_NORMAL].y * m_vScale.y;

	D3DXMatrixIdentity(&m_matWorld);
	m_matWorld._11 = m_vScale.x;
	m_matWorld._22 = m_vScale.y;
	m_matWorld._33 = m_vScale.z;
	m_matWorld._41 = m_vPosition.x;
	m_matWorld._42 = m_vPosition.y;
	m_matWorld._43 = m_vPosition.z;

	if (m_eAnchor == UIAC_RT)
		m_matWorld._41 -= width;
	if (m_eAnchor == UIAC_LB)
		m_matWorld._42 -= height;
	if (m_eAnchor == UIAC_RB)
	{
		m_matWorld._41 -= width;
		m_matWorld._42 -= height;
	}
	if (m_eAnchor == UIAC_C)
	{
		m_matWorld._41 -= width / 2;
		m_matWorld._42 -= height / 2;
	}

	if (m_pParent)
	{
		m_matWorld._41 += m_pParent->m_matWorld._41;
		m_matWorld._42 += m_pParent->m_matWorld._42;
		m_matWorld._43 += m_pParent->m_matWorld._43;
	}

	if (m_pFunction)
	{
		RECT rc;
		rc.left = m_matWorld._41;
		rc.top = m_matWorld._42;
		rc.right = rc.left + width;
		rc.bottom = rc.top + height;

		if (PtInRect(&rc, MOUSE_POS))
		{
			m_pFunction->OnOver(this);
			if (!m_isOver)
			{
				m_isOver = true;
				m_pFunction->OnEntrance(this);
			}
			if (INPUT->KeyUp(VK_LBUTTON))
				m_pFunction->OnClick(this);
		}
		else if (m_isOver)
		{
			m_isOver = false;
			m_pFunction->OnExit(this);
		}
	}

	if (!m_vecChild.empty())
		for (auto child : m_vecChild)
			child->Update();
}

void UIObject::Render()
{
	RECT rc;
	rc.left = m_matWorld._41;
	rc.top = m_matWorld._42;

	SPRITE_BEGIN;

	bool draw = false;
	if (m_pTex[UITEX_DOWN] && INPUT->KeyPress(VK_LBUTTON))
	{
		rc.right = rc.left + m_ptTexWH[UITEX_DOWN].x;
		rc.bottom = rc.top + m_ptTexWH[UITEX_DOWN].y;
		if (PtInRect(&rc, MOUSE_POS))
		{
			SPRITE->SetTransform(&m_matWorld);
			SPRITE->Draw(m_pTex[UITEX_DOWN], NULL, NULL, NULL, D3DCOLOR_ARGB(m_nAlpha, (int)m_vColor.x, (int)m_vColor.y, (int)m_vColor.z));
			draw = true;
		}
	}

	if (m_pTex[UITEX_OVER] && !draw)
	{
		rc.right = rc.left + m_ptTexWH[UITEX_OVER].x;
		rc.bottom = rc.top + m_ptTexWH[UITEX_OVER].y;
		if (PtInRect(&rc, MOUSE_POS))
		{
			SPRITE->SetTransform(&m_matWorld);
			SPRITE->Draw(m_pTex[UITEX_OVER], NULL, NULL, NULL, D3DCOLOR_ARGB(m_nAlpha, (int)m_vColor.x, (int)m_vColor.y, (int)m_vColor.z));
			draw = true;
		}
	}

	if (m_pTex[UITEX_NORMAL] && !draw)
	{
		rc.right = rc.left + m_ptTexWH[UITEX_NORMAL].x;
		rc.bottom = rc.top + m_ptTexWH[UITEX_NORMAL].y;

		SPRITE->SetTransform(&m_matWorld);
		SPRITE->Draw(m_pTex[UITEX_NORMAL], NULL, NULL, NULL, D3DCOLOR_ARGB(m_nAlpha, (int)m_vColor.x, (int)m_vColor.y, (int)m_vColor.z));
	}

	SPRITE_END;

	if (!m_vecChild.empty())
		for (auto child : m_vecChild)
			child->Render();
}

void UIObject::AddChild(UIObject * pChild)
{
	m_vecChild.push_back(pChild);
	pChild->m_pParent = this;
}

void UIObject::SetTexture(LPDIRECT3DTEXTURE9 normal, LPDIRECT3DTEXTURE9 over, LPDIRECT3DTEXTURE9 down)
{
	m_pTex[UITEX_NORMAL] = normal;
	m_pTex[UITEX_OVER] = over;
	m_pTex[UITEX_DOWN] = down;

	m_nAlpha = 255;

	D3DSURFACE_DESC desc;
	normal->GetLevelDesc(0, &desc);
	m_ptTexWH[UITEX_NORMAL] = MakePoint(desc.Width, desc.Height);
	if (over)
	{
		over->GetLevelDesc(0, &desc);
		m_ptTexWH[UITEX_OVER] = MakePoint(desc.Width, desc.Height);
	}
	if (down)
	{
		down->GetLevelDesc(0, &desc);
		m_ptTexWH[UITEX_DOWN] = MakePoint(desc.Width, desc.Height);
	}
}

void UIObject::SetFunction(IUIFunction * function)
{
	m_pFunction = function;
}

UIObject * UIObject::Find(string name)
{
	vector<UIObject*> vecChild;
	FindAllChild(vecChild);

	for (auto child : vecChild)
		if (child->m_sName.compare(name) == 0)
			return child;

	return NULL;
}

void UIObject::Release()
{
	for (auto child : m_vecChild)
		child->Release();

	delete this;
}
