#include "stdafx.h"
#include "Boss_Overload.h"
#include "Camera.h"
#include "Trasform.h"
#include "Camera_Component.h"
#include "Bullet.h"
#include "Collider.h"
#include "CameraObserver.h"
#include "RigidBody.h"
#include "Condition.h"
#include "SoundMgr.h"
#include "Billborad.h"
#include "Animator.h"
#include "Player.h"


CBoss_Overload::CBoss_Overload(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr),
	m_pTransform(nullptr), m_pCollider(nullptr), m_pGroundChekCollider(nullptr),
	m_pRigid(nullptr), m_pSubject(ENGINE::GetCameraSubject()), m_pPlayerSubject(ENGINE::GetPlayerSubject()),
	m_pObserver(nullptr), m_pCondition(nullptr), m_pBillboard(nullptr), m_pAnimator(nullptr),
	m_eOverState(OVER_IDLE), m_fLifeTime(0)
{
}

CBoss_Overload::~CBoss_Overload()
{
	Release();
}

int CBoss_Overload::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();

	State();

	return NO_EVENT;
}

void CBoss_Overload::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();

	D3DXMatrixIdentity(&m_matView);

	D3DXMATRIX Localmatrix, Cameramatrix;													  //  ����, ī�޶� ��� 
	D3DXVECTOR3 vSize;																		  // ����� ������ 
	Localmatrix = m_pTransform->GetWorldMatrix();
	Cameramatrix = m_pObserver->GetViewMatrix();

	vSize = m_pTransform->GetSize();


	m_pBillboard->Billborad_Yagnle(Localmatrix, Cameramatrix, vSize);                          // ������ ����
	m_matView = m_pBillboard->GetWorldMatrix_Billborad();

	m_pCollider->LateUpdate(m_pTransform->GetPos());

	m_pGroundChekCollider->LateUpdate({ m_pTransform->GetPos().x ,
		m_pTransform->GetPos().y - m_pCollider->Get_Radius().y,
		m_pTransform->GetPos().z });

	Check_Physic();
}

void CBoss_Overload::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matView);
	m_pTexture->Render(m_pAnimator->RenderSet(m_pTimeMgr->GetDelta()));
	m_pBuffer->Render();
}

HRESULT CBoss_Overload::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	// Ʈ������ ����
	m_pTransform->SetPos(D3DXVECTOR3(40.f, 10.f, 10.f));
	m_pTransform->SetSize(D3DXVECTOR3(14.f, 14.f, 14.f));


	// ������ �ݶ��̴�
	m_pCollider->Set_Radius({ 5.f , 14.f, 5.f });			// �� �࿡ �ش��ϴ� �������� ����
	m_pCollider->Set_Dynamic(true);							// ����, ���� Collider ����
	m_pCollider->Set_Trigger(false);						// Ʈ���� ����
	m_pCollider->Set_CenterPos(m_pTransform->GetPos());		// Collider �� ���߾���ǥ
	m_pCollider->Set_UnderPos();							// Collider �� �ϴ��߾� ��ǥ
	m_pCollider->SetUp_Box();								// ������ �͵��� Collider �� �ݿ��մϴ�.
	m_pCollider->Set_MaxY(-m_pCollider->Get_Radius().y - 1);
	m_pCollider->Set_Type(ENGINE::COLLISION_AABB);


															// Ʈ���� �ݶ��̴�
	m_pGroundChekCollider->Set_Radius({ 0.3f , 0.3f, 0.3f });
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
	m_pRigid->Set_fPower(10.f);								// ���� �Ŀ�

	m_pRigid->Set_Speed({ 10.f , 10.f , 10.f });				// �� �࿡ �ش��ϴ� �ӵ�
	m_pRigid->Set_Accel({ 1.f, 0.f, 0.f });					// �� �࿡ �ش��ϴ� Accel ��
	m_pRigid->Set_MaxAccel({ 2.f , 4.f , 2.f });			// �� �࿡ �ش��ϴ� MaxAccel ��


															// �����
	m_pCondition->Set_Hp(100.f);
	m_pCondition->Set_Armor(0.f);
	m_pCondition->Set_Shield(0.f);
	m_pCondition->Set_Fuel(0.f);
	m_pCondition->Set_Stamina(100.f);

	m_pCondition->Set_Dodge(false);
	m_pCondition->Set_Cinematic(false);
	m_pCondition->Set_Hit(false);
	m_pCondition->Set_Attack(false);
	m_pCondition->Set_Invincible(false);
	m_pCondition->Set_JetPack(false);
	m_pCondition->Set_MeleeAttack(true);
	m_pCondition->Set_RangeAttack(false);
	m_pCondition->Set_SpecialAttack(true);
	m_pCondition->Set_Slide(false);
	m_pCondition->Set_Run(false);
	m_pCondition->Set_MoveSpeed(10.f);
	m_pCondition->Set_MoveAccel(1.f);

	m_pAnimator->Set_FrameAmp(8.f);
	m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_ZERO);
	m_pAnimator->Set_Reverse(false);
	m_pAnimator->Set_MaxFrame(0);
	m_pAnimator->Stop_Animation(false);

	return S_OK;
}

HRESULT CBoss_Overload::LateInit()
{
	m_pObserver = CCameraObserver::Create();
	NULL_CHECK_RETURN(m_pObserver, E_FAIL);

	m_pSubject->Subscribe(m_pObserver);

	m_pTarget = m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player();

	return S_OK;
}

void CBoss_Overload::Release()
{
	m_pSubject->UnSubscribe(m_pObserver);
	ENGINE::Safe_Delete(m_pObserver);
}

HRESULT CBoss_Overload::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	// Texture
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_DYNAMIC, L"Overload_WalkFront");
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

	// Condition
	pComponent = ENGINE::CCondition::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Condition", pComponent });

	m_pCondition = dynamic_cast<ENGINE::CCondition*>(pComponent);
	NULL_CHECK_RETURN(m_pCondition, E_FAIL);

	// Billboard
	pComponent = ENGINE::CBillborad::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Billboard", pComponent });

	m_pBillboard = dynamic_cast<ENGINE::CBillborad*>(pComponent);
	NULL_CHECK_RETURN(m_pBillboard, E_FAIL);

	// Animator
	pComponent = ENGINE::CAnimator::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Animator", pComponent });

	m_pAnimator = dynamic_cast<ENGINE::CAnimator*>(pComponent);
	NULL_CHECK_RETURN(m_pAnimator, E_FAIL);

	return S_OK;
}

void CBoss_Overload::Check_Physic()
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

void CBoss_Overload::State()
{
	switch (m_eOverState)
	{
	case OVER_MET:
	{
		ChangeTex(L"Overload_Met");
		break;
	}
	case OVER_IDLE:
	{
		Direct_Missile();
		ChangeTex(L"Overload_WalkFront");
		break;
	}
	case OVER_WALK:
	{
		ChangeTex(L"Overload_WalkFront");
		break;
	}
	case OVER_RUSH:
	{
		ChangeTex(L"Overload_WalkFront");
		break;
	}
	case OVER_MISSILE_DIRECT:
	{
		ChangeTex(L"Overload_Fire");
		break;
	}
	case OVER_MISSILE_HOMMING:
	{
		ChangeTex(L"Overload_Fire");
		break;
	}
	case OVER_MISSILE_RAIN:
	{
		ChangeTex(L"Overload_Fire");
		break;
	}
	}
}

void CBoss_Overload::Walk()
{
	float fSpeed = m_pCondition->Get_MoveSpeed() * m_pTimeMgr->GetDelta();

	D3DXVECTOR3 vTempDir = static_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos() - m_pTransform->GetPos();
	D3DXVec3Normalize(&vTempDir, &vTempDir);
	vTempDir.y = 0;

	m_pTransform->Move_AdvancedPos(vTempDir , fSpeed);
}

void CBoss_Overload::Direct_Missile()
{
	m_fLifeTime += m_pTimeMgr->GetDelta();

	D3DXVECTOR3 vTmpPos = m_pTransform->GetPos();
	D3DXVECTOR3 vTmpDir = static_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos() - m_pTransform->GetPos();

	if (m_fLifeTime >= 1.f)
	{
		float a[3] = {0,0,0};

		CGameObject* pInstance = CBullet::Create(m_pGraphicDev, vTmpPos, vTmpDir, a, 0.5f, ENGINE::MONSTER_LUNCHER);
		m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::BULLET_MONSTER, pInstance);
		pInstance->Set_MapLayer(m_mapLayer);

		m_fLifeTime = 0.f;
	}
}

void CBoss_Overload::Shot_Direct()
{
}

void CBoss_Overload::Shot_Homing()
{
}

void CBoss_Overload::Shot_Rain()
{
}

void CBoss_Overload::ChangeTex(wstring _wstrTex)
{
	if (m_wstrTex.compare(_wstrTex) == 0)
		return;

	m_wstrTex = _wstrTex;

	m_mapComponent.erase(L"Texture");

	ENGINE::CComponent* pComponent = nullptr;
	pComponent = ENGINE::GetResourceMgr()->CloneResource(ENGINE::RESOURCE_DYNAMIC, _wstrTex);
	NULL_CHECK(pComponent);
	m_mapComponent.insert({ L"Texture", pComponent });

	m_pAnimator->Set_MaxFrame(dynamic_cast<ENGINE::CResources*>(pComponent)->Get_MaxFrame());

	m_pTexture = dynamic_cast<ENGINE::CTexture*>(pComponent);
	NULL_CHECK(m_pTexture);
}

CBoss_Overload* CBoss_Overload::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CBoss_Overload* pInstance = new CBoss_Overload(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}