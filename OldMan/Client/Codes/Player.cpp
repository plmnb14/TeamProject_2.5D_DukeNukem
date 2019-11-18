#include "stdafx.h"
#include "Player.h"
#include "Camera.h"
#include "Trasform.h"
#include "Camera_Component.h"
#include "Bullet.h"
#include "Collider.h"
#include "CameraObserver.h"
#include "RigidBody.h"


CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),	
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pKeyMgr(ENGINE::GetKeyMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), 
	m_pTransform(nullptr), m_pCollider(nullptr), m_pGroundChekCollider(nullptr),
	m_pRigid(nullptr),
	m_pSubject(ENGINE::GetCameraSubject()),
	m_eWeaponState(),
	m_pObserver(nullptr)
{	
}

CPlayer::~CPlayer()
{
	Release();
}

int CPlayer::Update() 
{
	if (m_bIsDead)
		return DEAD_OBJ;
	
	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();
	KeyInput();
	//Physic();

	return NO_EVENT;
}

void CPlayer::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();

	Physic();
	m_pCollider->LateUpdate(m_pTransform->GetPos());
	m_pGroundChekCollider->LateUpdate({ m_pTransform->GetPos().x ,
										m_pTransform->GetPos().y - m_pCollider->Get_Radius().y,
										m_pTransform->GetPos().z });
	m_pCollider->Set_IsCollision(false);
}

void CPlayer::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &(m_pTransform->GetWorldMatrix()));
	//m_pTexture->Render(0);
	m_pBuffer->Render();
}

HRESULT CPlayer::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	// 트랜스폼 세팅
	m_pTransform->SetPos(D3DXVECTOR3(0.f, 2.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));


	// 물리적 콜라이더
	m_pCollider->Set_Radius({ 1.0f , 1.0f, 1.0f });			// 각 축에 해당하는 반지름을 설정
	m_pCollider->Set_Dynamic(true);							// 동적, 정적 Collider 유무
	m_pCollider->Set_Trigger(false);						// 트리거 유무
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

HRESULT CPlayer::LateInit()
{
	m_pObserver = CCameraObserver::Create();
	NULL_CHECK_RETURN(m_pObserver, E_FAIL);

	m_pSubject->Subscribe(m_pObserver);

	return S_OK;
}

void CPlayer::Release()
{
	m_pSubject->UnSubscribe(m_pObserver);
	ENGINE::Safe_Delete(m_pObserver);
}

HRESULT CPlayer::AddComponent()
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

	return S_OK;
}

void CPlayer::KeyInput()
{
	float fMoveSpeed = 5.f * m_pTimeMgr->GetDelta();
	float fAngleSpeed = 90.f * m_pTimeMgr->GetDelta();

	srand(unsigned(time(NULL)));

	float xRand = rand() % (100 - 50) * 0.01f;
	float yRand = rand() % (100 - 50) * 0.01f;

	if (m_pKeyMgr->KeyPressing(ENGINE::KEY_LBUTTON))
	{
		Shoot();
	}


	if (GetAsyncKeyState('W'))
	{
		m_pTransform->MovePos(fMoveSpeed);
	}

	if (GetAsyncKeyState('S'))
	{
		m_pTransform->MovePos(-fMoveSpeed);
	}

	if (GetAsyncKeyState('A'))
	{
		D3DXVECTOR3 vDir, vWorldUp;
		vWorldUp = { 0.f , 1.f , 0.f };
		
		D3DXVec3Cross(&vDir, &m_pTransform->GetDir(), &vWorldUp);
		m_pTransform->Move_AdvancedPos(D3DXVECTOR3(vDir.x, 0.f, vDir.z) , fMoveSpeed);
	}

	if (GetAsyncKeyState('D'))
	{
		D3DXVECTOR3 vDir, vWorldUp;
		vWorldUp = { 0.f , 1.f , 0.f };
		
		D3DXVec3Cross(&vDir, &m_pTransform->GetDir(), &vWorldUp);
		m_pTransform->Move_AdvancedPos(D3DXVECTOR3(vDir.x, 0.f, vDir.z), -fMoveSpeed);
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		m_pTransform->MoveAngle(ENGINE::ANGLE_Y, -fAngleSpeed);
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		m_pTransform->MoveAngle(ENGINE::ANGLE_Y, fAngleSpeed);
	}

	if (m_pKeyMgr->KeyDown(ENGINE::KEY_SPACE))
	{
		m_pRigid->Set_Accel({ 0, 1.5f, 0 });
		m_pRigid->Set_IsJump(true);
		m_pRigid->Set_IsGround(false);
	}
}

void CPlayer::Physic()
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

void CPlayer::Shoot()
{
	D3DXVECTOR3 tmpDir = m_pTransform->GetDir();
	D3DXVECTOR3 tmpLook = dynamic_cast<CCamera*>(m_pCamera)->Get_Look();
	D3DXVECTOR3 tmpUp = dynamic_cast<CCamera*>(m_pCamera)->Get_Up();
	D3DXVECTOR3 tmpRight = dynamic_cast<CCamera*>(m_pCamera)->Get_Right();

	if (dynamic_cast<CCamera*>(m_pCamera)->Get_ViewPoint() == dynamic_cast<CCamera*>(m_pCamera)->FIRST_PERSON)
	{
		D3DXVECTOR3 tmpPos = { dynamic_cast<CCamera*>(m_pCamera)->Get_Pos().x + tmpLook.x * 1 - 2 * tmpUp.x + tmpRight.x * 2,
			dynamic_cast<CCamera*>(m_pCamera)->Get_Pos().y + tmpLook.y * 1 - 2 * tmpUp.y + tmpRight.y * 2,
			dynamic_cast<CCamera*>(m_pCamera)->Get_Pos().z + tmpLook.z * 1 - 2 * tmpUp.z + tmpRight.z * 2 };

		float fAngle[3];

		fAngle[0] = D3DXToDegree(acosf(tmpLook.y)) - 96;
		fAngle[1] = m_pTransform->GetAngle(ENGINE::ANGLE_Y) - 6;
		fAngle[2] = 0;

		CGameObject* pInstance = CBullet::Create(m_pGraphicDev, tmpPos, tmpLook, fAngle);
		m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::BULLET, pInstance);
	}

	else if (dynamic_cast<CCamera*>(m_pCamera)->Get_ViewPoint() == dynamic_cast<CCamera*>(m_pCamera)->THIRD_PERSON)
	{
		D3DXVECTOR3 tmpPos = { m_pTransform->GetPos().x + tmpDir.x * 3,
			m_pTransform->GetPos().y + 1.5f,
			m_pTransform->GetPos().z + tmpDir.z * 3 };

		float fAngle[3];

		fAngle[0] = m_pTransform->GetAngle(ENGINE::ANGLE_X);
		fAngle[1] = m_pTransform->GetAngle(ENGINE::ANGLE_Y);
		fAngle[2] = m_pTransform->GetAngle(ENGINE::ANGLE_Z);

		CGameObject* pInstance = CBullet::Create(m_pGraphicDev, tmpPos, tmpDir, fAngle);
		m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::BULLET, pInstance);
	}
}

void CPlayer::Swap_Weapon()
{
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CPlayer* pInstance = new CPlayer(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}
