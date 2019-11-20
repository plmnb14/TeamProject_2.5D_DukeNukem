#include "stdafx.h"
#include "Aim.h"
#include "Trasform.h"
#include "CameraObserver.h"
#include "PlayerObserver.h"

CAim::CAim(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev), m_eWeaponType(ENGINE::WEAPON_TAG::RIFLE)
{
}

CAim::~CAim()
{
	Release();
}

int CAim::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();

	CheckWeaponAim();
	CheckMoveRifleAim();
	CheckMoveShotgunAim();

	for (auto& iter : m_RifleAimArr)
		iter->Update();

	for (auto& iter : m_ShotgunAimArr)
		iter->Update();

	return NO_EVENT;
}

void CAim::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
}

void CAim::Render()
{
	switch (m_eWeaponType)
	{
	case ENGINE::WEAPON_TAG::MELLE:
		CUI::Render();
		break;
	case ENGINE::WEAPON_TAG::REVOLVER:
		CUI::Render();
		break;
	case ENGINE::WEAPON_TAG::RIFLE:
	case ENGINE::WEAPON_TAG::SMG:
	{
		for (auto& iter : m_RifleAimArr)
			iter->Render();

		break;
	}
	case ENGINE::WEAPON_TAG::SHOTGUN:
	{
		for (auto& iter : m_ShotgunAimArr)
			iter->Render();

		break;
	}
	case ENGINE::WEAPON_TAG::LUNCHER:
		CUI::Render();
		break;
	}

}

HRESULT CAim::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));

	m_fSizeX = 50.f;
	m_fSizeY = 50.f;

	m_fMaxMoveAim = 5.f;
	m_fMoveAim = 0.f;

	m_eWeaponType_Old = ENGINE::WEAPON_TAG::RIFLE;

	InitRifleAim();
	InitShotgunAim();

	return S_OK;
}

HRESULT CAim::LateInit()
{
	CUI::LateInit();

	return S_OK;
}

void CAim::Release()
{
	CUI::Release();

	for (auto& iter : m_RifleAimArr)
		ENGINE::Safe_Delete(iter);

	for (auto& iter : m_ShotgunAimArr)
		ENGINE::Safe_Delete(iter);
}

void CAim::CheckWeaponAim()
{
	// player에서 weapon 받아오기
	m_eWeaponType = m_pPlayerObserver->GetWeaponInfo().eWeaponTag;

	if (m_eWeaponType_Old == m_eWeaponType)
		return;

	m_eWeaponType_Old = m_eWeaponType;
	switch (m_eWeaponType)
	{
	case ENGINE::WEAPON_TAG::MELLE:
		ChangeTex(L"Aim_dot.png");
		break;
	case ENGINE::WEAPON_TAG::REVOLVER:
		ChangeTex(L"Aim_3.png");
		break;
	case ENGINE::WEAPON_TAG::RIFLE:
		break;
	case ENGINE::WEAPON_TAG::SHOTGUN:
		break;
	case ENGINE::WEAPON_TAG::SMG:
		break;
	case ENGINE::WEAPON_TAG::LUNCHER:
		ChangeTex(L"LuncherAim.png");
		break;
	default:
		break;
	}
}

void CAim::CheckMoveRifleAim()
{
	if (m_eWeaponType != ENGINE::WEAPON_TAG::RIFLE
		|| m_eWeaponType != ENGINE::WEAPON_TAG::SMG)
		return;

	// 임시
	float fMoveAmount = 20.f * m_pTimeMgr->GetDelta();
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		if (m_fMaxMoveAim > m_fMoveAim)
		{
			m_fMoveAim += fMoveAmount;

			m_RifleAimArr[AIM_LEFT]		->SetPos(D3DXVECTOR3(m_RifleAimArr[AIM_LEFT]->GetPos().x - fMoveAmount, 0.f, 0.f));
			m_RifleAimArr[AIM_RIGHT]	->SetPos(D3DXVECTOR3(m_RifleAimArr[AIM_RIGHT]->GetPos().x + fMoveAmount, 0.f, 0.f));
			m_RifleAimArr[AIM_UP]		->SetPos(D3DXVECTOR3(0.f, m_RifleAimArr[AIM_UP]->GetPos().y - fMoveAmount, 0.f));
			m_RifleAimArr[AIM_DOWM]		->SetPos(D3DXVECTOR3(0.f, m_RifleAimArr[AIM_DOWM]->GetPos().y + fMoveAmount, 0.f));
		}
	}
	else
	{
		if (m_fMoveAim > 0.f)
		{
			m_fMoveAim -= fMoveAmount;
			m_RifleAimArr[AIM_LEFT]->SetPos(D3DXVECTOR3(m_RifleAimArr[AIM_LEFT]->GetPos().x + fMoveAmount, 0.f, 0.f));
			m_RifleAimArr[AIM_RIGHT]->SetPos(D3DXVECTOR3(m_RifleAimArr[AIM_RIGHT]->GetPos().x - fMoveAmount, 0.f, 0.f));
			m_RifleAimArr[AIM_UP]->SetPos(D3DXVECTOR3(0.f, m_RifleAimArr[AIM_UP]->GetPos().y + fMoveAmount, 0.f));
			m_RifleAimArr[AIM_DOWM]->SetPos(D3DXVECTOR3(0.f, m_RifleAimArr[AIM_DOWM]->GetPos().y - fMoveAmount, 0.f));
		}
	}
}

void CAim::CheckMoveShotgunAim()
{
	if (m_eWeaponType != ENGINE::WEAPON_TAG::SHOTGUN)
		return;

	// 임시
	float fMoveAmount = 20.f * m_pTimeMgr->GetDelta();
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		if (m_fMaxMoveAim > m_fMoveAim)
		{
			m_fMoveAim += fMoveAmount;

			m_ShotgunAimArr[AIM_LEFT]->SetPos(D3DXVECTOR3(m_ShotgunAimArr[AIM_LEFT]->GetPos().x - fMoveAmount * 0.5f, m_ShotgunAimArr[AIM_LEFT]->GetPos().y + fMoveAmount * 0.5f, 0.f));
			m_ShotgunAimArr[AIM_RIGHT]->SetPos(D3DXVECTOR3(m_ShotgunAimArr[AIM_RIGHT]->GetPos().x + fMoveAmount * 0.5f, m_ShotgunAimArr[AIM_RIGHT]->GetPos().y + fMoveAmount * 0.5f, 0.f));
			m_ShotgunAimArr[AIM_UP]->SetPos(D3DXVECTOR3(0.f, m_ShotgunAimArr[AIM_UP]->GetPos().y - fMoveAmount * 0.7f, 0.f));
		}
	}
	else
	{
		if (m_fMoveAim > 0.f)
		{
			m_fMoveAim -= fMoveAmount;

			m_ShotgunAimArr[AIM_LEFT]->SetPos(D3DXVECTOR3(m_ShotgunAimArr[AIM_LEFT]->GetPos().x + fMoveAmount * 0.5f, m_ShotgunAimArr[AIM_LEFT]->GetPos().y - fMoveAmount * 0.5f, 0.f));
			m_ShotgunAimArr[AIM_RIGHT]->SetPos(D3DXVECTOR3(m_ShotgunAimArr[AIM_RIGHT]->GetPos().x - fMoveAmount * 0.5f, m_ShotgunAimArr[AIM_RIGHT]->GetPos().y - fMoveAmount * 0.5f, 0.f));
			m_ShotgunAimArr[AIM_UP]->SetPos(D3DXVECTOR3(0.f, m_ShotgunAimArr[AIM_UP]->GetPos().y + fMoveAmount * 0.7f, 0.f));
		}
	}
}

void CAim::InitRifleAim()
{
	m_RifleAimArr[AIM_LEFT]		= CUI::Create(m_pGraphicDev, L"RifleAim_h.png");
	m_RifleAimArr[AIM_RIGHT]	= CUI::Create(m_pGraphicDev, L"RifleAim_h.png");
	m_RifleAimArr[AIM_UP]		= CUI::Create(m_pGraphicDev, L"RifleAim_v.png");
	m_RifleAimArr[AIM_DOWM]		= CUI::Create(m_pGraphicDev, L"RifleAim_v.png");

	float fPadding = 10.f;
	m_RifleAimArr[AIM_LEFT]	->SetPos(D3DXVECTOR3(-fPadding, 0.f, 0.f));
	m_RifleAimArr[AIM_RIGHT]->SetPos(D3DXVECTOR3(fPadding, 0.f, 0.f));
	m_RifleAimArr[AIM_UP]	->SetPos(D3DXVECTOR3(0.f, -fPadding, 0.f));
	m_RifleAimArr[AIM_DOWM]	->SetPos(D3DXVECTOR3(0.f, fPadding, 0.f));

	m_RifleAimOriPos[AIM_LEFT]	= D3DXVECTOR3(-fPadding, 0.f, 0.f);
	m_RifleAimOriPos[AIM_RIGHT] = D3DXVECTOR3(fPadding, 0.f, 0.f);
	m_RifleAimOriPos[AIM_UP]	= D3DXVECTOR3(0.f, -fPadding, 0.f);
	m_RifleAimOriPos[AIM_DOWM]	= D3DXVECTOR3(0.f, fPadding, 0.f);
}

void CAim::InitShotgunAim()
{
	m_ShotgunAimArr[AIM_LEFT]	= CUI::Create(m_pGraphicDev, L"ShotgunAim_Left.png");
	m_ShotgunAimArr[AIM_RIGHT]	= CUI::Create(m_pGraphicDev, L"ShotgunAim_Right.png");
	m_ShotgunAimArr[AIM_UP] = CUI::Create(m_pGraphicDev, L"ShotgunAim_Bottom.png");
	// Center Dot
	m_ShotgunAimArr[AIM_DOWM] = CUI::Create(m_pGraphicDev, L"Aim_dot.png");

	m_ShotgunAimArr[AIM_LEFT]	->SetSize(200.f, 200.f);
	m_ShotgunAimArr[AIM_RIGHT]	->SetSize(200.f, 200.f);
	m_ShotgunAimArr[AIM_UP]		->SetSize(200.f, 200.f);
	m_ShotgunAimArr[AIM_DOWM]	->SetSize(200.f, 200.f);

	m_ShotgunAimOriPos[AIM_LEFT]	= D3DXVECTOR3(0.f, 0.f, 0.f);
	m_ShotgunAimOriPos[AIM_RIGHT]	= D3DXVECTOR3(0.f, 0.f, 0.f);
	m_ShotgunAimOriPos[AIM_UP]		= D3DXVECTOR3(0.f, 0.f, 0.f);
	m_ShotgunAimOriPos[AIM_DOWM]	= D3DXVECTOR3(0.f, 0.f, 0.f);
}

CAim* CAim::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CAim* pInstance = new CAim(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}
