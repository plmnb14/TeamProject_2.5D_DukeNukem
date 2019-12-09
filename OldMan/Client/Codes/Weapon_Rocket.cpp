#include "stdafx.h"
#include "Weapon_Rocket.h"
#include "Trasform.h"
#include "Collider.h"
#include "RigidBody.h"
#include "CameraObserver.h"
#include "CameraSubject.h"
#include "Billborad.h"
#include "Player.h"
#include "SoundMgr.h"

CWeapon_Rocket::CWeapon_Rocket(LPDIRECT3DDEVICE9 pGraphicDev)
	: CWeapon(pGraphicDev)
{
	//ZeroMemory(m_tWInfo, sizeof(ENGINE::W_INFO));
}

CWeapon_Rocket::~CWeapon_Rocket()
{
}

int CWeapon_Rocket::Update()
{
	if (m_bIsDead)
	{
		CSoundMgr::GetInstance()->SetVolume(CSoundMgr::UI, 0.5f);
		CSoundMgr::GetInstance()->StopSound(CSoundMgr::UI);
		CSoundMgr::GetInstance()->MyPlaySound(L"Gun_PickUp.mp3", CSoundMgr::UI);

		static_cast<CPlayer*>(m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player())->Set_WeaponInfo(&m_tWInfo);
		return DEAD_OBJ;
	}

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();

	return NO_EVENT;
}

void CWeapon_Rocket::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();

	//Physic();

	D3DXMATRIX Localmatrix, Cameramatrix;													  //  ����, ī�޶� ��� 
	D3DXVECTOR3 vSize;																		  // ����� ������ 
	Localmatrix = m_pTransform->GetWorldMatrix();

	if (m_pObserver != nullptr)
		Cameramatrix = m_pObserver->GetViewMatrix();
	vSize = m_pTransform->GetSize();

	m_pBillborad->Billborad_Front(Localmatrix, Cameramatrix, vSize);                          // ������ ����
	m_matView = m_pBillborad->GetWorldMatrix_Billborad();                                    // �����忡�� ������ ����� �޾ƿ´�. 



	m_pCollider->LateUpdate(m_pTransform->GetPos());

	m_pGChecker->LateUpdate({ m_pTransform->GetPos().x ,
		m_pTransform->GetPos().y - m_pCollider->Get_Radius().y,
		m_pTransform->GetPos().z });

	m_pCollider->Set_IsCollision(false);
}

void CWeapon_Rocket::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matView);
	m_pTexture->Render(5);
	m_pBuffer->Render();
}

HRESULT CWeapon_Rocket::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);


	m_tWInfo.eBulletType = ENGINE::HITSCAN;	// ��Ʈ��ĵ �������, ����ü �������
	m_tWInfo.fInterval = 1.00f;				// �߻� ����
	m_tWInfo.fKnockBack_Value = 5.0f;		// ������

	m_tWInfo.wMaxBullet = 128;				// �ִ� źȯ ��
	m_tWInfo.wUseBullet = 1;				// �ѹ� �߻� �� �Ҹ� źȯ ��
	m_tWInfo.wCurBullet = 256;				// ����  ��, ���� ȹ�� �� źâ ��
	m_tWInfo.wMagazineSize = 6;			// �� źâ �ִ� ���� ��
	m_tWInfo.wMagazineBullet = 6;			// ���� źâ�� �Ѿ� ����

	m_tWInfo.fVertical_Rebound = 0.5f;		// ���� �ݵ�
	m_tWInfo.fHorizontal_Rebound = 18.0f;		// ���� �ݵ�

	m_tWInfo.fSpread_X = 1;
	m_tWInfo.fSpread_Y = 1;

	m_tWInfo.fBullet_Speed = 10.f;

	m_tWInfo.wWeaponDamage = 100.f;				// ���� ������
	m_tWInfo.eWeaponTag = ENGINE::LUNCHER;

	// Ʈ������ ����
	m_pTransform->SetPos(D3DXVECTOR3(0.f, 1.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(2.f, 2.f, 2.f));


	// ������ �ݶ��̴�
	m_pCollider->Set_Radius({ 1.0f , 2.0f, 1.0f });			// �� �࿡ �ش��ϴ� �������� ����
	m_pCollider->Set_Dynamic(true);					// ����, ���� Collider ����
	m_pCollider->Set_Trigger(true);						// Ʈ���� ����
	m_pCollider->Set_CenterPos(m_pTransform->GetPos());		// Collider �� ���߾���ǥ
	m_pCollider->Set_UnderPos();							// Collider �� �ϴ��߾� ��ǥ
	m_pCollider->SetUp_Box();								// ������ �͵��� Collider �� �ݿ��մϴ�.


	// ������ �ٵ� ����
	m_pRigid->Set_UseGravity(true);							// �߷��� ���� ����

	m_pRigid->Set_IsGround(false);							// �������� üũ
	m_pRigid->Set_IsAir(true);								// �������� üũ
	m_pRigid->Set_IsFall(true);								// ���������� üũ
	m_pRigid->Set_IsJump(false);

	m_pRigid->Set_fMass(1.f);								// ��ü�� ����
	m_pRigid->Set_fPower(5.f);								// ���� �Ŀ�

	m_pRigid->Set_Speed({ 1.f , 1.f , 1.f });				// �� �࿡ �ش��ϴ� �ӵ�
	m_pRigid->Set_Accel({ 0.f, 0.f, 0.f });					// �� �࿡ �ش��ϴ� Accel ��
	m_pRigid->Set_MaxAccel({ 2.f , 6.f , 2.f });			// �� �࿡ �ش��ϴ� MaxAccel ��

	return S_OK;
}

HRESULT CWeapon_Rocket::LateInit()
{
	m_pObserver = CCameraObserver::Create();
	NULL_CHECK_RETURN(m_pObserver, E_FAIL);

	m_pSubject->Subscribe(m_pObserver);

	return S_OK;
}

void CWeapon_Rocket::Release()
{
}

HRESULT CWeapon_Rocket::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	// Texture
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, L"Equip");
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Texture", pComponent });

	m_pTexture = static_cast<ENGINE::CTexture*>(pComponent);
	NULL_CHECK_RETURN(m_pTexture, E_FAIL);

	// Buffer
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_DYNAMIC, L"Buffer_RcTex");
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Buffer", pComponent });

	m_pBuffer = static_cast<ENGINE::CVIBuffer*>(pComponent);
	NULL_CHECK_RETURN(m_pBuffer, E_FAIL);

	// Transform
	pComponent = ENGINE::CTransform::Create(D3DXVECTOR3(0.f, 0.f, 1.f));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Transform", pComponent });

	m_pTransform = static_cast<ENGINE::CTransform*>(pComponent);
	NULL_CHECK_RETURN(m_pTransform, E_FAIL);


	// Collider
	pComponent = ENGINE::CCollider::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Collider", pComponent });

	m_pCollider = static_cast<ENGINE::CCollider*>(pComponent);
	NULL_CHECK_RETURN(m_pCollider, E_FAIL);


	// GroundCheck_Collider
	pComponent = ENGINE::CCollider::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"GCheck_Collider", pComponent });

	m_pGChecker = static_cast<ENGINE::CCollider*>(pComponent);
	NULL_CHECK_RETURN(m_pGChecker, E_FAIL);


	// Rigid
	pComponent = ENGINE::CRigidBody::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"RigidBody", pComponent });

	m_pRigid = static_cast<ENGINE::CRigidBody*>(pComponent);
	NULL_CHECK_RETURN(m_pRigid, E_FAIL);

	pComponent = ENGINE::CBillborad::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_pBillborad = static_cast<ENGINE::CBillborad*>(pComponent);
	NULL_CHECK_RETURN(m_pBillborad, E_FAIL);
	m_mapComponent.insert({ L"BillBoard", pComponent });

	return S_OK;
}

void CWeapon_Rocket::Set_Pos(D3DXVECTOR3 _Pos)
{
	m_pTransform->SetPos(_Pos);
}

void CWeapon_Rocket::Physic()
{
	if (m_pRigid->Get_IsJump() == true)
	{
		D3DXVECTOR3 JumpLength = { 0, m_pRigid->Set_Jump(m_pTransform->GetPos(), m_pTimeMgr->GetDelta()) , 0 };
		m_pTransform->Move_AdvancedPos_Vec3(JumpLength);

		if (m_pRigid->Get_Accel().y <= 0.f)
		{
			m_pRigid->Set_Accel({ 0,0,0 });
			m_pRigid->Set_IsFall(true);
			m_pRigid->Set_IsJump(false);
		}
	}

	if (m_pRigid->Get_IsJump() == false)
	{
		if (m_pRigid->Get_IsGround() == true && m_pRigid->Get_IsFall() == false)
			return;

		D3DXVECTOR3 JumpLength = { 0, -m_pRigid->Set_Fall(m_pTransform->GetPos(), m_pTimeMgr->GetDelta()),0 };
		m_pTransform->Move_AdvancedPos_Vec3(JumpLength);
	}
}

CWeapon_Rocket * CWeapon_Rocket::Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CWeapon_Rocket* pInstance = new CWeapon_Rocket(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	pInstance->Set_Pos(_Pos);

	return pInstance;
}
