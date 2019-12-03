#include "stdafx.h"
#include "WeaponIcon.h"
#include "Trasform.h"
#include "CameraObserver.h"
#include "PlayerObserver.h"

CWeaponIcon::CWeaponIcon(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev), m_eWeaponType(ENGINE::WEAPON_TAG::RIFLE)
{
}

CWeaponIcon::~CWeaponIcon()
{
	Release();
}

int CWeaponIcon::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();
	CheckWeapon();

	return NO_EVENT;
}

void CWeaponIcon::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
}

void CWeaponIcon::Render()
{
	CUI::Render();
}

HRESULT CWeaponIcon::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));

	m_fSizeX = 50.f;
	m_fSizeY = 50.f;

	m_eWeaponType_Old = ENGINE::WEAPON_TAG::RIFLE;

	return S_OK;
}

HRESULT CWeaponIcon::LateInit()
{
	CUI::LateInit();

	return S_OK;
}

void CWeaponIcon::Release()
{
}

void CWeaponIcon::CheckWeapon()
{
	// player에서 weapon 받아오기
	m_eWeaponType = m_pPlayerObserver->GetWeaponInfo().eWeaponTag;

	if (m_eWeaponType_Old == m_eWeaponType)
		return;

	m_eWeaponType_Old = m_eWeaponType;
	m_bVisible = true;
	switch (m_eWeaponType)
	{
	case ENGINE::WEAPON_TAG::MELLE:
		ChangeTex(L"weaponIcon_fist.png");
		break;
	case ENGINE::WEAPON_TAG::REVOLVER:
		ChangeTex(L"weaponIcon_revolver.png");
		break;
	case ENGINE::WEAPON_TAG::RIFLE:
		ChangeTex(L"weaponIcon_smg.png");
		break;
	case ENGINE::WEAPON_TAG::SHOTGUN:
		ChangeTex(L"weaponIcon_shotgun.png");
		break;
	case ENGINE::WEAPON_TAG::SMG:
		ChangeTex(L"weaponIcon_smg.png");
		break;
	case ENGINE::WEAPON_TAG::LUNCHER:
		ChangeTex(L"weaponIcon_launcher.png");
		break;
	default:
		break;
	}
}

CWeaponIcon* CWeaponIcon::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CWeaponIcon* pInstance = new CWeaponIcon(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}
