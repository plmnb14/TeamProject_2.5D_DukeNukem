#include "stdafx.h"
#include "Terrain.h"
#include "Trasform.h"

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr),
	m_eTerrainType(ENGINE::TERRAIN_END)
{
}

CTerrain::~CTerrain()
{
	Release();
}

int CTerrain::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::Update();

	KeyInput();

	return NO_EVENT;
}

void CTerrain::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
}

void CTerrain::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &(m_pTransform->GetWorldMatrix()));
	m_pTexture->Render(0);
	m_pBuffer->Render();
}

HRESULT CTerrain::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));

	return S_OK;
}

void CTerrain::Release()
{
}

HRESULT CTerrain::AddComponent()
{
	return S_OK;
}

void CTerrain::KeyInput()
{
	/*float fMoveSpeed = 5.f * m_pTimeMgr->GetDelta();
	float fAngleSpeed = 90.f * m_pTimeMgr->GetDelta();

	if (GetAsyncKeyState(VK_UP) & 0x8000)
		m_pTransform->MovePos(fMoveSpeed);
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		m_pTransform->MovePos(-fMoveSpeed);
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		m_pTransform->MoveAngle(ENGINE::ANGLE_Y, -fAngleSpeed);
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		m_pTransform->MoveAngle(ENGINE::ANGLE_Y, fAngleSpeed);*/
}

CTerrain* CTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CTerrain* pInstance = new CTerrain(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}
