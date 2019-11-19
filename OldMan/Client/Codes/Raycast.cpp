#include "stdafx.h"
#include "Raycast.h"

CRayCast::CRayCast(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev)
{
}

CRayCast::~CRayCast()
{
}

int CRayCast::Update()
{
	return 0;
}

void CRayCast::LateUpdate()
{
}

void CRayCast::Render()
{
}

HRESULT CRayCast::Initialize()
{
	return E_NOTIMPL;
}

void CRayCast::Release()
{
}

HRESULT CRayCast::AddComponent()
{
	return E_NOTIMPL;
}

void CRayCast::SetUp_ViewSpace()
{
	POINT pt = Get_MousePos();
}

void CRayCast::SetUp_LocalSpace()
{
}

POINT CRayCast::Get_MousePos()
{
	POINT pt = {};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	return pt;
}

CRayCast * CRayCast::Create()
{
	return nullptr;
}
