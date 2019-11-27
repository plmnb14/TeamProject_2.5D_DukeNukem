#include "stdafx.h"
#include "Effect_Fireball.h"
#include "Trasform.h"
#include "Animator.h"
#include "CameraObserver.h"
#include "Billborad.h"
#include "RigidBody.h"
#include "Effect_RocketSmoke.h"
#include "Effect_Firewall.h"
#include "Collider.h"

CEffect_Fireball::CEffect_Fireball(LPDIRECT3DDEVICE9 pGraphicDev)
	:CVfx(pGraphicDev), m_wFrame(0), m_yAngle(0), m_fMove(0), m_fPower(0) ,m_fMaxAccel(0), m_bCreate(0), m_iCount(0), m_fLifetime(0)
{
}

CEffect_Fireball::~CEffect_Fireball()
{
	Release();
}

int CEffect_Fireball::Update()
{
	++m_iCount;

	if (m_bIsDead)
	{
		if (m_pCollider->Get_IsCollision())
		{
			D3DXVECTOR3 vTempSize = { m_pTransform->GetSize().x *2, m_pTransform->GetSize().y *2, m_pTransform->GetSize().z *2};
			//D3DXVECTOR3 vTempDir = -m_pTransform->GetDir();
			//D3DXVECTOR3 vTempPos = { m_pCollider->Get_CenterPos().x + vTempDir.x * 3 ,
			//	m_pCollider->Get_CenterPos().y + vTempDir.y * 3 ,
			//	m_pCollider->Get_CenterPos().z + vTempDir.z * 3 };

			//CGameObject* pInstance = CEffect_Firewall::Create(m_pGraphicDev, vTempPos);
			CGameObject* pInstance = CEffect_Firewall::Create(m_pGraphicDev, m_pTransform->GetPos() , vTempSize);
			m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pInstance);
		}
		return DEAD_OBJ;
	}

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();

	m_pTransform->MovePos(m_fMove * m_pTimeMgr->GetDelta());

	Check_Physic();

	if (m_iCount > 5)
	{
		D3DXVECTOR3 m_vSize = m_pTransform->GetSize();

		CGameObject* pInstance = CEffect_RocketSmoke::Create(m_pGraphicDev, m_pTransform->GetPos(), m_vSize);
		m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pInstance);
	}

	m_fLifetime -= m_pTimeMgr->GetDelta();

	return NO_EVENT;
}

void CEffect_Fireball::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();

	m_pCollider->LateUpdate(m_pTransform->GetPos());

	D3DXMatrixIdentity(&m_matView);

	D3DXMATRIX Localmatrix, Cameramatrix;													  //  로컬, 카메라 행렬 
	D3DXVECTOR3 vSize;																		  // 대상의 사이즈 
	Localmatrix = m_pTransform->GetWorldMatrix();
	Cameramatrix = m_pObserver->GetViewMatrix();

	vSize = m_pTransform->GetSize();

	m_pBillborad->Billborad_Front(Localmatrix, Cameramatrix, vSize);                          // 빌보드 설정
	m_matView = m_pBillborad->GetWorldMatrix_Billborad();

	m_bCreate = true;

	if (m_fLifetime < 0.1f)
		m_bIsDead = true;
}

void CEffect_Fireball::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matView);
	m_pTexture->Render(0);
	m_pBuffer->Render();
}

HRESULT CEffect_Fireball::Initialize()
{
	float size = (rand() % 12 + 3) * 0.1f;

	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(size, size, size));

	int Frame = dynamic_cast<ENGINE::CResources*>(m_pTexture)->Get_MaxFrame();
	m_pAnimator->Set_MaxFrame(Frame);
	m_pAnimator->Set_FrameAmp(40.f);
	m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_STOP);
	m_pAnimator->Stop_Animation(false);


	m_fPower = rand() % 10 + 25;
	m_yAngle = rand() % 10 + 20;
	m_fMove = rand() % 10 + 20;
	m_fMaxAccel = (rand() % 20 + 30) * 0.1f;


	// 물리적 콜라이더
	m_pCollider->Set_Radius({ 0.2f , 0.2f, 0.2f });			// 각 축에 해당하는 반지름을 설정
	m_pCollider->Set_Dynamic(true);							// 동적, 정적 Collider 유무
	m_pCollider->Set_Trigger(true);						// 트리거 유무
	m_pCollider->Set_CenterPos(m_pTransform->GetPos());		// Collider 의 정중앙좌표
	m_pCollider->Set_UnderPos();							// Collider 의 하단중앙 좌표
	m_pCollider->SetUp_Box();								// 설정된 것들을 Collider 에 반영합니다.

	// 리지드 바디 세팅
	m_pRigid->Set_UseGravity(true);							// 중력의 영향 유무

	m_pRigid->Set_IsGround(false);							// 지상인지 체크
	m_pRigid->Set_IsAir(true);								// 공중인지 체크
	m_pRigid->Set_IsFall(false);								// 낙하중인지 체크
	m_pRigid->Set_IsJump(true);


	m_pRigid->Set_fMass(1.f);								// 물체의 무게
	m_pRigid->Set_fPower(m_fPower);								// 점프 파워

	m_pRigid->Set_Speed({ 10.f , 10.f , 10.f });				// 각 축에 해당하는 속도
	m_pRigid->Set_Accel({ 1, 1.5f, 1 });					// 각 축에 해당하는 Accel 값
	m_pRigid->Set_MaxAccel({ 2.f , m_fMaxAccel , 2.f });			// 각 축에 해당하는 MaxAccel 값

	m_fLifetime = 3.f;

	//m_yAngle = 45;

	//m_pTransform->SetAngle(m_yAngle, ENGINE::ANGLE_X);

	return S_OK;
}

HRESULT CEffect_Fireball::LateInit()

{
	m_pObserver = CCameraObserver::Create();
	NULL_CHECK_RETURN(m_pObserver, E_FAIL);

	m_pSubject->Subscribe(m_pObserver);

	return S_OK;
}

void CEffect_Fireball::Release()
{
}

void CEffect_Fireball::Set_Pos(D3DXVECTOR3 _Pos)
{
	m_pTransform->SetPos(_Pos);
}

void CEffect_Fireball::Set_Angle(float _Angle)
{
	m_pTransform->SetAngle(_Angle, ENGINE::ANGLE_Y);
}

void CEffect_Fireball::Check_Physic()
{
	if (m_pRigid->Get_IsJump() == true)
	{
		D3DXVECTOR3 JumpLength = { 0, m_pRigid->Set_Jump(m_pTransform->GetPos(), m_pTimeMgr->GetDelta()) , 0 };
		m_pTransform->Move_AdvancedPos_Vec3(JumpLength);

		if (m_pRigid->Get_Accel().y <= 0.f)
		{
			m_pRigid->Set_Accel({ 1,0,1 });
			m_pRigid->Set_IsFall(true);
			m_pRigid->Set_IsJump(false);
		}
	}

	if (m_pRigid->Get_IsJump() == false)
	{
		if (m_pRigid->Get_IsGround() == true && m_pRigid->Get_IsFall() == false)
		{
			return;
		}

		D3DXVECTOR3 JumpLength = { 0, -m_pRigid->Set_Fall(m_pTransform->GetPos(), m_pTimeMgr->GetDelta()),0 };
		m_pTransform->Move_AdvancedPos_Vec3(JumpLength);
	}
}

HRESULT CEffect_Fireball::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	// Texture
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_DYNAMIC, L"Explosion_Fire");
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

	// Animator
	pComponent = ENGINE::CAnimator::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Animator", pComponent });

	m_pAnimator = dynamic_cast<ENGINE::CAnimator*>(pComponent);
	NULL_CHECK_RETURN(m_pAnimator, E_FAIL);

	//빌보드 
	pComponent = ENGINE::CBillborad::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_pBillborad = dynamic_cast<ENGINE::CBillborad*>(pComponent);
	NULL_CHECK_RETURN(m_pBillborad, E_FAIL);
	m_mapComponent.insert({ L"BillBoard", pComponent });

	// Rigid
	pComponent = ENGINE::CRigidBody::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"RigidBody", pComponent });

	m_pRigid = dynamic_cast<ENGINE::CRigidBody*>(pComponent);
	NULL_CHECK_RETURN(m_pRigid, E_FAIL);

	// Collider
	pComponent = ENGINE::CCollider::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Collider", pComponent });

	m_pCollider = dynamic_cast<ENGINE::CCollider*>(pComponent);
	NULL_CHECK_RETURN(m_pCollider, E_FAIL);

	return S_OK;
}

CEffect_Fireball* CEffect_Fireball::Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos , float _Angle)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CEffect_Fireball* pInstance = new CEffect_Fireball(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	pInstance->Set_Pos(_Pos);
	pInstance->Set_Angle(_Angle);

	return pInstance;
}
