#include "stdafx.h"
#include "Weapon_Revolver.h"
#include "Trasform.h"
#include "Collider.h"
#include "RigidBody.h"
#include "CameraObserver.h"
#include "CameraSubject.h"
#include "Billborad.h"

CWeapon_Revolver::CWeapon_Revolver(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr),
	m_pTransform(nullptr), m_pCollider(nullptr), m_pGroundChekCollider(nullptr),
	m_pRigid(nullptr), m_pSubject(ENGINE::GetCameraSubject()), m_pBillborad(nullptr), m_pObserver(nullptr)
{
	//ZeroMemory(m_pWInfo, sizeof(ENGINE::W_INFO));
}

CWeapon_Revolver::~CWeapon_Revolver()
{
}

int CWeapon_Revolver::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();

	return NO_EVENT;
}

void CWeapon_Revolver::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();

	//Physic();

	D3DXMATRIX Localmatrix, Cameramatrix;													  //  ����, ī�޶� ��� 
	D3DXVECTOR3 vSize;																		  // ����� ������ 
	Localmatrix = m_pTransform->GetWorldMatrix();
	Cameramatrix = m_pObserver->GetViewMatrix();
	vSize = m_pTransform->GetSize();

	m_pBillborad->Billborad_Front(Localmatrix, Cameramatrix, vSize);                          // ������ ����
	m_matView = m_pBillborad->GetWorldMatrix_Billborad();                                    // �����忡�� ������ ����� �޾ƿ´�. 



	m_pCollider->LateUpdate(m_pTransform->GetPos());

	m_pGroundChekCollider->LateUpdate({ m_pTransform->GetPos().x ,
		m_pTransform->GetPos().y - m_pCollider->Get_Radius().y,
		m_pTransform->GetPos().z });

	m_pCollider->Set_IsCollision(false);
}

void CWeapon_Revolver::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matView);
	//m_pTexture->Render(0);
	m_pBuffer->Render();
}

HRESULT CWeapon_Revolver::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);


	m_pWInfo.eBulletType = ENGINE::HITSCAN;	// ��Ʈ��ĵ �������, ����ü �������
	m_pWInfo.fInterval = 0.2f;				// �߻� ����
	m_pWInfo.fKnockBack_Value = 0.1f;		// ������

	m_pWInfo.wMaxBullet = 128;				// �ִ� źȯ ��
	m_pWInfo.wUseBullet = 1;				// �ѹ� �߻� �� �Ҹ� źȯ ��
	m_pWInfo.wCurBullet = 0;				// ���� źâ ��, ���� ȹ�� �� źâ ��

	m_pWInfo.wWeaponDamage = 3;				// ���� ������
	m_pWInfo.eWeaponTag = ENGINE::REVOLVER;


	// Ʈ������ ����
	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));


	// ������ �ݶ��̴�
	m_pCollider->Set_Radius({ 1.0f , 1.0f, 1.0f });			// �� �࿡ �ش��ϴ� �������� ����
	m_pCollider->Set_Dynamic(true);					// ����, ���� Collider ����
	m_pCollider->Set_Trigger(true);						// Ʈ���� ����
	m_pCollider->Set_CenterPos(m_pTransform->GetPos());		// Collider �� ���߾���ǥ
	m_pCollider->Set_UnderPos();							// Collider �� �ϴ��߾� ��ǥ
	m_pCollider->SetUp_Box();								// ������ �͵��� Collider �� �ݿ��մϴ�.

															// Ʈ���� �ݶ��̴�
	m_pGroundChekCollider->Set_Radius({ 0.3f , 0.2f, 0.3f });
	m_pGroundChekCollider->Set_Dynamic(true);
	m_pGroundChekCollider->Set_Trigger(true);
	m_pGroundChekCollider->Set_CenterPos({ m_pTransform->GetPos().x ,
		m_pTransform->GetPos().y - m_pCollider->Get_Radius().y,
		m_pTransform->GetPos().z });
	m_pGroundChekCollider->Set_UnderPos();
	m_pGroundChekCollider->SetUp_Box();


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

HRESULT CWeapon_Revolver::LateInit()
{
	m_pObserver = CCameraObserver::Create();
	NULL_CHECK_RETURN(m_pObserver, E_FAIL);

	m_pSubject->Subscribe(m_pObserver);

	return S_OK;
}

void CWeapon_Revolver::Release()
{
}

HRESULT CWeapon_Revolver::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	// Texture
	//pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, L"Texture_Player");
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent.insert({ L"Texture", pComponent });
	//
	//m_pTexture = dynamic_cast<ENGINE::CTexture*>(pComponent);
	//NULL_CHECK_RETURN(m_pTexture, E_FAIL);

	// Buffer
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, L"Buffer_Player");
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Buffer", pComponent });

	m_pBuffer = dynamic_cast<ENGINE::CVIBuffer*>(pComponent);
	NULL_CHECK_RETURN(m_pBuffer, E_FAIL);

	// Transform
	pComponent = ENGINE::CTransform::Create(D3DXVECTOR3(0.f, 0.f, 1.f));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Transform", pComponent });

	m_pTransform = dynamic_cast<ENGINE::CTransform*>(pComponent);
	NULL_CHECK_RETURN(m_pTransform, E_FAIL);


	// Collider
	pComponent = ENGINE::CCollider::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Collider", pComponent });

	m_pCollider = dynamic_cast<ENGINE::CCollider*>(pComponent);
	NULL_CHECK_RETURN(m_pCollider, E_FAIL);


	// GroundCheck_Collider
	pComponent = ENGINE::CCollider::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"GCheck_Collider", pComponent });

	m_pGroundChekCollider = dynamic_cast<ENGINE::CCollider*>(pComponent);
	NULL_CHECK_RETURN(m_pGroundChekCollider, E_FAIL);


	// Rigid
	pComponent = ENGINE::CRigidBody::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"RigidBody", pComponent });

	m_pRigid = dynamic_cast<ENGINE::CRigidBody*>(pComponent);
	NULL_CHECK_RETURN(m_pRigid, E_FAIL);

	pComponent = ENGINE::CBillborad::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_pBillborad = dynamic_cast<ENGINE::CBillborad*>(pComponent);
	NULL_CHECK_RETURN(m_pBillborad, E_FAIL);
	m_mapComponent.insert({ L"BillBoard", pComponent });

	return S_OK;
}

void CWeapon_Revolver::Set_Pos(D3DXVECTOR3 _Pos)
{
	m_pTransform->SetPos(_Pos);
}

void CWeapon_Revolver::Physic()
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

CWeapon_Revolver * CWeapon_Revolver::Create(LPDIRECT3DDEVICE9 pGraphicDev ,D3DXVECTOR3 _Pos)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);
	
	CWeapon_Revolver* pInstance = new CWeapon_Revolver(pGraphicDev);
	
	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}
	
	pInstance->Set_Pos(_Pos);
	
	return pInstance;
}
