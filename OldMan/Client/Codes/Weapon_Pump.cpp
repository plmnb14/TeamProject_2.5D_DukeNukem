#include "stdafx.h"
#include "Weapon_Pump.h"
#include "Trasform.h"
#include "Collider.h"
#include "RigidBody.h"
#include "CameraObserver.h"
#include "CameraSubject.h"
#include "Billborad.h"
#include "Player.h"
#include "SoundMgr.h"

CWeapon_Pump::CWeapon_Pump(LPDIRECT3DDEVICE9 pGraphicDev)
	: CWeapon(pGraphicDev)
{
	//ZeroMemory(m_pWInfo, sizeof(ENGINE::W_INFO));
}

CWeapon_Pump::~CWeapon_Pump()
{
}

int CWeapon_Pump::Update()
{
	if (m_bIsDead)
	{
		CSoundMgr::GetInstance()->SetVolume(CSoundMgr::UI, 0.5f);
		CSoundMgr::GetInstance()->StopSound(CSoundMgr::UI);
		CSoundMgr::GetInstance()->MyPlaySound(L"Gun_PickUp.ogg", CSoundMgr::UI);

		dynamic_cast<CPlayer*>(m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player())->Set_WeaponInfo(&m_pWInfo);
		return DEAD_OBJ;
	}

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();

	return NO_EVENT;
}

void CWeapon_Pump::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();

	//Physic();

	D3DXMATRIX Localmatrix, Cameramatrix;													  //  로컬, 카메라 행렬 
	D3DXVECTOR3 vSize;																		  // 대상의 사이즈 
	Localmatrix = m_pTransform->GetWorldMatrix();

	if (m_pObserver != nullptr)
		Cameramatrix = m_pObserver->GetViewMatrix();

	vSize = m_pTransform->GetSize();

	m_pBillborad->Billborad_Front(Localmatrix, Cameramatrix, vSize);                          // 빌보드 설정
	m_matView = m_pBillborad->GetWorldMatrix_Billborad();                                    // 빌보드에서 설정된 행렬을 받아온다. 



	m_pCollider->LateUpdate(m_pTransform->GetPos());

	m_pGroundChekCollider->LateUpdate({ m_pTransform->GetPos().x ,
		m_pTransform->GetPos().y - m_pCollider->Get_Radius().y,
		m_pTransform->GetPos().z });

	m_pCollider->Set_IsCollision(false);
}

void CWeapon_Pump::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matView);
	m_pTexture->Render(2);
	m_pBuffer->Render();
}

HRESULT CWeapon_Pump::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);


	m_pWInfo.eBulletType = ENGINE::HITSCAN;	// 히트스캔 방식인지, 투사체 방식인지
	m_pWInfo.fInterval = 1.00f;				// 발사 간격
	m_pWInfo.fKnockBack_Value = 5.0f;		// 저지력

	m_pWInfo.wMaxBullet = 128;				// 최대 탄환 수
	m_pWInfo.wUseBullet = 1;				// 한번 발사 당 소모 탄환 수
	m_pWInfo.wCurBullet = 256;				// 현재  수, 최초 획득 시 탄창 수
	m_pWInfo.wMagazineSize = 12;			// 한 탄창 최대 보관 수
	m_pWInfo.wMagazineBullet = 12;			// 현재 탄창의 총알 개수

	m_pWInfo.fVertical_Rebound = 0.5f;		// 수평 반동
	m_pWInfo.fHorizontal_Rebound = 8.0f;		// 수직 반동

	m_pWInfo.fSpread_X = 1000;
	m_pWInfo.fSpread_Y = 1000;

	m_pWInfo.fBullet_Speed = 100.f;

	m_pWInfo.wWeaponDamage = 3.f;				// 무기 데미지
	m_pWInfo.eWeaponTag = ENGINE::SHOTGUN;


	// 트랜스폼 세팅
	m_pTransform->SetPos(D3DXVECTOR3(0.f, 1.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(2.f, 2.f, 2.f));


	// 물리적 콜라이더
	m_pCollider->Set_Radius({ 1.0f , 2.0f, 1.0f });			// 각 축에 해당하는 반지름을 설정
	m_pCollider->Set_Dynamic(true);					// 동적, 정적 Collider 유무
	m_pCollider->Set_Trigger(true);						// 트리거 유무
	m_pCollider->Set_CenterPos(m_pTransform->GetPos());		// Collider 의 정중앙좌표
	m_pCollider->Set_UnderPos();							// Collider 의 하단중앙 좌표
	m_pCollider->SetUp_Box();								// 설정된 것들을 Collider 에 반영합니다.

															// 트리거 콜라이더
	m_pGroundChekCollider->Set_Radius({ 0.3f , 0.2f, 0.3f });
	m_pGroundChekCollider->Set_Dynamic(true);
	m_pGroundChekCollider->Set_Trigger(true);
	m_pGroundChekCollider->Set_CenterPos({ m_pTransform->GetPos().x ,
		m_pTransform->GetPos().y - m_pCollider->Get_Radius().y,
		m_pTransform->GetPos().z });
	m_pGroundChekCollider->Set_UnderPos();
	m_pGroundChekCollider->SetUp_Box();


	// 리지드 바디 세팅
	m_pRigid->Set_UseGravity(true);							// 중력의 영향 유무

	m_pRigid->Set_IsGround(false);							// 지상인지 체크
	m_pRigid->Set_IsAir(true);								// 공중인지 체크
	m_pRigid->Set_IsFall(true);								// 낙하중인지 체크
	m_pRigid->Set_IsJump(false);

	m_pRigid->Set_fMass(1.f);								// 물체의 무게
	m_pRigid->Set_fPower(5.f);								// 점프 파워

	m_pRigid->Set_Speed({ 1.f , 1.f , 1.f });				// 각 축에 해당하는 속도
	m_pRigid->Set_Accel({ 0.f, 0.f, 0.f });					// 각 축에 해당하는 Accel 값
	m_pRigid->Set_MaxAccel({ 2.f , 6.f , 2.f });			// 각 축에 해당하는 MaxAccel 값

	return S_OK;
}

HRESULT CWeapon_Pump::LateInit()
{
	m_pObserver = CCameraObserver::Create();
	NULL_CHECK_RETURN(m_pObserver, E_FAIL);

	m_pSubject->Subscribe(m_pObserver);

	return S_OK;
}

void CWeapon_Pump::Release()
{
}

HRESULT CWeapon_Pump::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	// Texture
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, L"Equip");
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Texture", pComponent });

	m_pTexture = dynamic_cast<ENGINE::CTexture*>(pComponent);
	NULL_CHECK_RETURN(m_pTexture, E_FAIL);

	// Buffer
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_DYNAMIC, L"Buffer_RcTex");
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

void CWeapon_Pump::Set_Pos(D3DXVECTOR3 _Pos)
{
	m_pTransform->SetPos(_Pos);
}

void CWeapon_Pump::Physic()
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

CWeapon_Pump * CWeapon_Pump::Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CWeapon_Pump* pInstance = new CWeapon_Pump(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	pInstance->Set_Pos(_Pos);

	return pInstance;
}
