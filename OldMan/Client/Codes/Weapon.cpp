#include "stdafx.h"
#include "Weapon.h"
#include "Trasform.h"
#include "Collider.h"
#include "RigidBody.h"
#include "CameraObserver.h"
#include "CameraSubject.h"
#include "Billborad.h"

CWeapon::CWeapon(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr),
	m_pTransform(nullptr), m_pCollider(nullptr), m_pGroundChekCollider(nullptr),
	m_pRigid(nullptr), m_pSubject(ENGINE::GetCameraSubject()), m_pBillborad(nullptr), m_pObserver(nullptr)
{
	//ZeroMemory(m_pWInfo, sizeof(ENGINE::W_INFO));
}

CWeapon::~CWeapon()
{
}

int CWeapon::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();

	return NO_EVENT;
}

void CWeapon::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
}

void CWeapon::Render()
{
}

HRESULT CWeapon::Initialize()
{
	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));

	return S_OK;
}

HRESULT CWeapon::LateInit()
{
	m_pObserver = CCameraObserver::Create();
	NULL_CHECK_RETURN(m_pObserver, E_FAIL);

	m_pSubject->Subscribe(m_pObserver);

	return S_OK;
}

void CWeapon::Release()
{
	m_pSubject->UnSubscribe(m_pObserver);
	ENGINE::Safe_Delete(m_pObserver);
}

CWeapon* CWeapon::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CWeapon* pInstance = new CWeapon(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}