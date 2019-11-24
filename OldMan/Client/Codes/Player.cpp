#include "stdafx.h"
#include "Player.h"
#include "Camera.h"
#include "Trasform.h"
#include "Camera_Component.h"
#include "Bullet.h"
#include "Collider.h"
#include "CameraObserver.h"
#include "RigidBody.h"
#include "Condition.h"


CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pKeyMgr(ENGINE::GetKeyMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr),
	m_pTransform(nullptr), m_pCollider(nullptr), m_pGroundChekCollider(nullptr),
	m_pRigid(nullptr), m_fSlideUp(0),
	m_pSubject(ENGINE::GetCameraSubject()), m_pPlayerSubject(ENGINE::GetPlayerSubject()),
	m_eWeaponState(ENGINE::WEAPON_TAG::MELLE), m_fZoomAccel(0),
	m_pObserver(nullptr) , m_bZoom(false), m_fMaxZoom(0) , m_fMinZoom(0),
	m_eActState(W_DRAW)
{	
	ZeroMemory(&m_pWInfo, sizeof(ENGINE::W_INFO));
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

	Check_Slide();
	Check_Run();
	Check_Physic();
	
	UpdateObserverData();

	return NO_EVENT;
}

void CPlayer::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();

	ShootDelay();
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
	
	// Ʈ������ ����
	m_pTransform->SetPos(D3DXVECTOR3(0.f, 2.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 2.f, 1.f));
	
	
	// ������ �ݶ��̴�
	m_pCollider->Set_Radius({ 1.0f , 2.0f, 1.0f });			// �� �࿡ �ش��ϴ� �������� ����
	m_pCollider->Set_Dynamic(true);							// ����, ���� Collider ����
	m_pCollider->Set_Trigger(false);						// Ʈ���� ����
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
	m_pCondition->Set_RangeAttack(true);
	m_pCondition->Set_SpecialAttack(true);
	m_pCondition->Set_Slide(false);
	m_pCondition->Set_Run(false);
	m_pCondition->Set_MoveSpeed(10.f);
	m_pCondition->Set_MoveAccel(1.f);
	
	
	m_fZoomAccel	= 0.f;
	m_fZoomSpeed	= 5;
	m_fMaxZoom		= 70;
	m_fMinZoom		= 20;
	
	m_pPlayerSubject->AddData(ENGINE::CPlayerSubject::PLAYER_INFO, &m_pCondition);
	m_pWInfo.eWeaponTag = ENGINE::WEAPON_TAG::MELLE;
	m_pWInfo.wCurBullet = 0;
	m_pPlayerSubject->AddData(ENGINE::CPlayerSubject::WEAPON_INFO, &m_pWInfo);

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

	// Condition
	pComponent = ENGINE::CCondition::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Condition", pComponent });

	m_pCondition = dynamic_cast<ENGINE::CCondition*>(pComponent);
	NULL_CHECK_RETURN(m_pCondition, E_FAIL);

	return S_OK;
}

void CPlayer::KeyInput()
{
	float fMoveSpeed = m_pCondition->Get_MoveSpeed() * m_pCondition->Get_MoveAccel() * m_pCondition->Get_MoveAccel() * m_pTimeMgr->GetDelta();
	float fAngleSpeed = 90.f * m_pTimeMgr->GetDelta();

	ShootType();

	if (m_pKeyMgr->KeyDown(ENGINE::KEY_LSHIFT))
	{
		m_pCondition->Set_Run(true);
		m_pCondition->Set_MoveSpeed(16.f);
	}

	else if (m_pKeyMgr->KeyUp(ENGINE::KEY_LSHIFT))
	{
		m_pCondition->Set_Run(false);
	
		m_pCondition->Set_MoveSpeed(10.f);
		D3DXVECTOR3 vTemp = { 0 , 0 , 0 };
		dynamic_cast<CCamera*>(m_pCamera)->Set_CamShakePos(vTemp);
	}

	if (m_pKeyMgr->KeyDown(ENGINE::KEY_LCTRL))
	{
		if (m_pCondition->Get_Run() == false || m_pCondition->Get_Slide())
			return;

		//dynamic_cast<CCamera*>(m_pCamera)->Set_CamYPos(-2);

		m_pCondition->Set_MoveAccel(2.5f);
		m_pCondition->Set_Slide(true);

		m_pCondition->Set_MoveSpeed(10.f);
		D3DXVECTOR3 vTemp = { 0 , 0 , 0 };
		dynamic_cast<CCamera*>(m_pCamera)->Set_CamShakePos(vTemp);

	}

	// ������
	if (m_pKeyMgr->KeyDown(ENGINE::KEY_R))
	{
		if (m_eWeaponState == ENGINE::MELLE)
			return;

		if (m_pWInfo.wCurBullet <= 0)
			return;

		if (m_pWInfo.wMagazineBullet == m_pWInfo.wMagazineSize)
			return;

		if(m_eActState == W_NONE || m_eActState == W_ZOOMOUT)
			Reload();
	}


	// �̵�����
	if (m_pKeyMgr->KeyPressing(ENGINE::KEY_W))
	{
		m_pTransform->MovePos(fMoveSpeed);
	}

	if (m_pKeyMgr->KeyPressing(ENGINE::KEY_S))
	{
		m_pTransform->MovePos(-fMoveSpeed);
	}

	if (m_pKeyMgr->KeyPressing(ENGINE::KEY_A))
	{
		D3DXVECTOR3 vDir, vWorldUp;
		vWorldUp = { 0.f , 1.f , 0.f };
		
		D3DXVec3Cross(&vDir, &m_pTransform->GetDir(), &vWorldUp);
		m_pTransform->Move_AdvancedPos(D3DXVECTOR3(vDir.x, 0.f, vDir.z) , fMoveSpeed);
	}

	if (m_pKeyMgr->KeyPressing(ENGINE::KEY_D))
	{
		D3DXVECTOR3 vDir, vWorldUp;
		vWorldUp = { 0.f , 1.f , 0.f };
		
		D3DXVec3Cross(&vDir, &m_pTransform->GetDir(), &vWorldUp);
		m_pTransform->Move_AdvancedPos(D3DXVECTOR3(vDir.x, 0.f, vDir.z), -fMoveSpeed);
	}

	if (m_pKeyMgr->KeyPressing(ENGINE::KEY_LEFT))
	{
		m_pTransform->MoveAngle(ENGINE::ANGLE_Y, -fAngleSpeed);
	}

	if (m_pKeyMgr->KeyPressing(ENGINE::KEY_RIGHT))
	{
		m_pTransform->MoveAngle(ENGINE::ANGLE_Y, fAngleSpeed);
	}

	// ����
	if (m_pKeyMgr->KeyDown(ENGINE::KEY_SPACE))
	{
		m_pRigid->Set_Accel({ 1, 1.5f, 1 });
		m_pRigid->Set_IsJump(true);
		m_pRigid->Set_IsGround(false);
	}

	if (m_pKeyMgr->KeyDown(ENGINE::KEY_1))
	{
		// ���� ����
		//m_eWeaponState = m_mWeaponInfo[]
	}

	if (m_pKeyMgr->KeyDown(ENGINE::KEY_2))
	{
		auto iter_find = m_mWeaponInfo.find(ENGINE::WEAPON_TAG::REVOLVER);

		if (m_mWeaponInfo.end() == iter_find)
			return;

		else
		{
			if (m_eWeaponState == iter_find->second->eWeaponTag)
			{

				return;
			}

			else
			{

				m_eWeaponState = iter_find->second->eWeaponTag;
				memcpy(&m_pWInfo, iter_find->second, sizeof(ENGINE::W_INFO));
			}
		}
	}

	if (m_pKeyMgr->KeyDown(ENGINE::KEY_3))
	{
		auto iter_find = m_mWeaponInfo.find(ENGINE::WEAPON_TAG::SMG);

		if (m_mWeaponInfo.end() == iter_find)
			return;

		else
		{
			if (m_eWeaponState == iter_find->second->eWeaponTag)
			{

				return;
			}

			else
			{

				m_eWeaponState = iter_find->second->eWeaponTag;
				memcpy(&m_pWInfo, iter_find->second, sizeof(ENGINE::W_INFO));
			}
		}
	}

	if (m_pKeyMgr->KeyDown(ENGINE::KEY_4))
	{
		auto iter_find = m_mWeaponInfo.find(ENGINE::WEAPON_TAG::SHOTGUN);

		if (m_mWeaponInfo.end() == iter_find)
			return;

		else
		{
			if (m_eWeaponState == iter_find->second->eWeaponTag)
			{

				return;
			}

			else
			{

				m_eWeaponState = iter_find->second->eWeaponTag;
				memcpy(&m_pWInfo, iter_find->second, sizeof(ENGINE::W_INFO));
			}
		}
	}

	if (m_pKeyMgr->KeyDown(ENGINE::KEY_5))
	{
		auto iter_find = m_mWeaponInfo.find(ENGINE::WEAPON_TAG::LUNCHER);

		if (m_mWeaponInfo.end() == iter_find)
			return;

		else
		{
			if (m_eWeaponState == iter_find->second->eWeaponTag)
			{

				return;
			}

			else
			{

				m_eWeaponState = iter_find->second->eWeaponTag;
				memcpy(&m_pWInfo, iter_find->second, sizeof(ENGINE::W_INFO));
			}
		}
	}
}

void CPlayer::Check_Physic()
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

void CPlayer::UpdateObserverData()
{
	// Update Observer Data
	m_pPlayerSubject->AddData(ENGINE::CPlayerSubject::WEAPON_INFO, &m_pWInfo);
}

void CPlayer::Shoot()
{
	if (m_pWInfo.fDelayTimer != 0)
		return;

	if (m_mWeaponInfo.empty())
		return;

	if (m_pWInfo.wMagazineBullet > 0)
	{
		if(m_bZoom == false)
			m_eActState = W_FIRE;

		if (m_bZoom == true)
			m_eActState = W_ZOOMFIRE;

		m_pCondition->Set_RangeAttack(true);

		int xSpread = m_pWInfo.fSpread_X - (m_pWInfo.fSpread_X * 2);
		int ySpread = m_pWInfo.fSpread_Y - (m_pWInfo.fSpread_Y * 2);

		float xRand = rand() % xSpread * 0.01f;
		float yRand = rand() % ySpread * 0.01f;

		m_pWInfo.wMagazineBullet -= m_pWInfo.wUseBullet;
		m_pWInfo.fDelayTimer = m_pWInfo.fInterval;


		D3DXVECTOR3 tmpDir = m_pTransform->GetDir();
		D3DXVECTOR3 tmpLook = dynamic_cast<CCamera*>(m_pCamera)->Get_Look();
		D3DXVECTOR3 tmpUp = dynamic_cast<CCamera*>(m_pCamera)->Get_Up();
		D3DXVECTOR3 tmpRight = dynamic_cast<CCamera*>(m_pCamera)->Get_Right();

		if (dynamic_cast<CCamera*>(m_pCamera)->Get_ViewPoint() == dynamic_cast<CCamera*>(m_pCamera)->FIRST_PERSON)
		{
			//D3DXVECTOR3 tmpPos = { dynamic_cast<CCamera*>(m_pCamera)->Get_Pos().x + tmpLook.x * 1 - 1 * tmpUp.x + tmpRight.x * 2,
			//	dynamic_cast<CCamera*>(m_pCamera)->Get_Pos().y + tmpLook.y * 1 - 1 * tmpUp.y + tmpRight.y * 2,
			//	dynamic_cast<CCamera*>(m_pCamera)->Get_Pos().z + tmpLook.z * 1 - 1 * tmpUp.z + tmpRight.z * 2 };

			D3DXVECTOR3 tmpPos = { dynamic_cast<CCamera*>(m_pCamera)->Get_Pos().x - 0.5f * tmpUp.x,
				dynamic_cast<CCamera*>(m_pCamera)->Get_Pos().y - 0.5f * tmpUp.y,
				dynamic_cast<CCamera*>(m_pCamera)->Get_Pos().z - 0.5f * tmpUp.z };

			float fAngle[3];

			// ź ���� ����
			fAngle[0] = D3DXToDegree(acosf(tmpLook.y)) - 93 + xRand;
			fAngle[1] = m_pTransform->GetAngle(ENGINE::ANGLE_Y) + yRand;
			fAngle[2] = 0;

			CGameObject* pInstance = CBullet::Create(m_pGraphicDev, tmpPos, tmpLook, fAngle , m_pWInfo.fBullet_Speed , m_pWInfo.eWeaponTag);
			m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::BULLET_PLAYER, pInstance);
			pInstance->Set_MapLayer(m_mapLayer);

			dynamic_cast<CCamera*>(m_pCamera)->Set_Hotizontal(m_pWInfo.fHorizontal_Rebound);
			dynamic_cast<CCamera*>(m_pCamera)->Set_Vertical(m_pWInfo.fVertical_Rebound);
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

			CGameObject* pInstance = CBullet::Create(m_pGraphicDev, tmpPos, tmpDir, fAngle , m_pWInfo.fBullet_Speed, m_pWInfo.eWeaponTag);
			m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::BULLET_PLAYER, pInstance);
		}
	}

	if (m_pWInfo.wMagazineBullet <= 0)
	{
		m_pCondition->Set_RangeAttack(false);

		if (m_bZoom == false)
			m_eActState = W_NONE;

		if (m_bZoom == true)
			m_eActState = W_ZOOMOUT;
	}
}

void CPlayer::Shoot_Shotgun()
{
	if (m_pWInfo.wMagazineBullet > 0)
	{
		if (m_bZoom == false)
			m_eActState = W_FIRE;

		if (m_bZoom == true)
			m_eActState = W_ZOOMFIRE;

		for (int i = 0; i < 30; ++i)
		{
			float a = 0.f;
			float c = 0.f;
			float b = (float)(rand() % 2);

			if (b == 1)
			{
				a = 1;
				c = -1;
			}
			else if (b == 0)
			{
				a = -1;
				c = 1;
			}


			int xSpread = (int)(m_pWInfo.fSpread_X - (m_pWInfo.fSpread_X * 2) + (i * a * 2));
			int ySpread = (int)(m_pWInfo.fSpread_Y - (m_pWInfo.fSpread_Y * 2) + (i * c * 2));

			float xRand = rand() % xSpread * 0.01f;
			float yRand = rand() % ySpread * 0.01f;

			D3DXVECTOR3 tmpDir = m_pTransform->GetDir();
			D3DXVECTOR3 tmpLook = dynamic_cast<CCamera*>(m_pCamera)->Get_Look();
			D3DXVECTOR3 tmpUp = dynamic_cast<CCamera*>(m_pCamera)->Get_Up();
			D3DXVECTOR3 tmpRight = dynamic_cast<CCamera*>(m_pCamera)->Get_Right();

			if (dynamic_cast<CCamera*>(m_pCamera)->Get_ViewPoint() == dynamic_cast<CCamera*>(m_pCamera)->FIRST_PERSON)
			{
				D3DXVECTOR3 tmpPos = { dynamic_cast<CCamera*>(m_pCamera)->Get_Pos().x  - 1 * tmpUp.x,
					dynamic_cast<CCamera*>(m_pCamera)->Get_Pos().y - 1 * tmpUp.y,
					dynamic_cast<CCamera*>(m_pCamera)->Get_Pos().z - 1 * tmpUp.z };

				float fAngle[3];

				// ź ���� ����
				fAngle[0] = D3DXToDegree(acosf(tmpLook.y)) - 96 + xRand;
				fAngle[1] = m_pTransform->GetAngle(ENGINE::ANGLE_Y) - 6 + yRand;
				fAngle[2] = 0;

				CGameObject* pInstance = CBullet::Create(m_pGraphicDev, tmpPos, tmpLook, fAngle, m_pWInfo.fBullet_Speed , m_pWInfo.eWeaponTag);
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::BULLET_PLAYER, pInstance);

				dynamic_cast<CCamera*>(m_pCamera)->Set_Hotizontal(m_pWInfo.fHorizontal_Rebound);
				dynamic_cast<CCamera*>(m_pCamera)->Set_Vertical(m_pWInfo.fVertical_Rebound);
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

				CGameObject* pInstance = CBullet::Create(m_pGraphicDev, tmpPos, tmpDir, fAngle, m_pWInfo.fBullet_Speed , m_pWInfo.eWeaponTag);
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::BULLET_PLAYER, pInstance);
			}
		}

		m_pWInfo.wMagazineBullet -= m_pWInfo.wUseBullet;
		m_pWInfo.fDelayTimer = m_pWInfo.fInterval;

	}

	if (m_pWInfo.wMagazineBullet <= 0)
	{
		m_pCondition->Set_RangeAttack(false);

		if (m_bZoom == false)
			m_eActState = W_NONE;

		if (m_bZoom == true)
			m_eActState = W_ZOOMOUT;
	}
}

void CPlayer::ShootDelay()
{
	if (m_pWInfo.fDelayTimer > 0)
	{
		m_pWInfo.fDelayTimer -= m_pTimeMgr->GetDelta();
	}

	if (m_pWInfo.fDelayTimer < 0)
	{
		m_pWInfo.fDelayTimer = 0.f;
	}
}

void CPlayer::Reload()
{
	// ������

	m_eActState = W_RELOAD;

	if (m_pWInfo.wCurBullet < m_pWInfo.wMagazineSize)
	{
		m_pWInfo.wMagazineBullet = m_pWInfo.wCurBullet;
		m_pWInfo.wCurBullet = 0;
	}

	else
	{
		if (m_pWInfo.wMagazineBullet != 0 && m_pWInfo.wMagazineBullet > 0)
		{
			m_pWInfo.wCurBullet += m_pWInfo.wMagazineBullet;
		}

		m_pWInfo.wMagazineBullet = m_pWInfo.wMagazineSize;
		m_pWInfo.wCurBullet -= m_pWInfo.wMagazineSize;
	}

}

void CPlayer::WeaponActState()
{
	switch (m_eActState)
	{
	case W_NONE:
	{
		break;
	}

	case W_FIRST:
	{
		break;
	}

	case W_FIRE:
	{
		break;
	}

	case W_RELOAD:
	{
		break;
	}

	case W_DRAW:
	{
		break;
	}

	case W_ZOOMIN:
	{
		break;
	}

	case W_ZOOMOUT:
	{
		break;
	}

	case W_ZOOMFIRE:
	{
		break;
	}
	}
}

void CPlayer::Check_Slide()
{
	if (m_pCondition->Get_Slide())
	{
		if (m_pCondition->Get_MoveAccel() > 0)
		{
			float tmpX = m_pCondition->Get_MoveAccel() - m_pTimeMgr->GetDelta() * 3;
			m_pCondition->Set_MoveAccel(tmpX);

			if (tmpX * tmpX - 6 > -2.f)
				dynamic_cast<CCamera*>(m_pCamera)->Set_CamYPos(tmpX * tmpX - 6);

			else
				dynamic_cast<CCamera*>(m_pCamera)->Set_CamYPos(-2);
		}

		else if (m_pCondition->Get_MoveAccel() <= 0)
		{
			//m_pCondition->Set_MoveAccel(0.f);
			m_pCondition->Set_MoveSpeed(10.f);

			m_fSlideUp += m_pTimeMgr->GetDelta() * m_pTimeMgr->GetDelta() * 400 + 0.1f;

			if (m_fSlideUp < 2.f)
				dynamic_cast<CCamera*>(m_pCamera)->Set_CamYPos(-2 + m_fSlideUp);

			else if (m_fSlideUp >= 2.f)
			{
				dynamic_cast<CCamera*>(m_pCamera)->Set_CamYPos(0.f);
				m_pCondition->Set_Slide(false);
				m_pCondition->Set_MoveAccel(1.f);
				m_pCondition->Set_MoveSpeed(10.f);
				m_fSlideUp = 0.f;

				if (m_pCondition->Get_Run())
				{
					m_pCondition->Set_MoveSpeed(16.f);
				}
			}
		}
	}
}

void CPlayer::Check_Run()
{
	if (m_pCondition->Get_Run())
	{
		if (dynamic_cast<CCamera*>(m_pCamera)->Get_CamShakePos() == D3DXVECTOR3{ 0,0,0 } &&
			!m_pRigid->Get_IsJump() && !m_pRigid->Get_IsFall())
		{
			D3DXVECTOR3 vTemp = { 0.3f , 0.2f , 0.3f };
			dynamic_cast<CCamera*>(m_pCamera)->Set_CamShakePos(vTemp);
		}

		if (m_pRigid->Get_IsJump() || m_pRigid->Get_IsFall() || m_pCondition->Get_Slide())
		{
			D3DXVECTOR3 vTemp = { 0 , 0 , 0 };
			dynamic_cast<CCamera*>(m_pCamera)->Set_CamShakePos(vTemp);
		}
	}
}

void CPlayer::ShootType()
{
	if (m_eActState == W_RELOAD)
		return;

	switch (m_eWeaponState)
	{
	case ENGINE::MELLE:
	case ENGINE::REVOLVER:
	case ENGINE::LUNCHER:
	{
		Zoom();

		if (m_pKeyMgr->KeyDown(ENGINE::KEY_LBUTTON))
		{
			Shoot();
		}

		//Zoom();

		break;
	}

	case ENGINE::RIFLE:
	case ENGINE::SMG:
	{
		Zoom();

		if (m_pKeyMgr->KeyPressing(ENGINE::KEY_LBUTTON))
		{
			Shoot();
		}

		if (m_pKeyMgr->KeyUp(ENGINE::KEY_LBUTTON))
		{
			if (m_eActState == W_ZOOMFIRE)
				m_eActState = W_ZOOMIN;

			else
				m_eActState = W_NONE;

			m_pCondition->Set_RangeAttack(false);
		}

		//Zoom();

		break;
	}
	
	case ENGINE::SHOTGUN:
	{
		if (m_pKeyMgr->KeyDown(ENGINE::KEY_LBUTTON))
		{
			Shoot_Shotgun();
		}

		Zoom();

		break;
	}
	}
}

void CPlayer::Zoom()
{
	float velocity = (m_fZoomAccel * m_fZoomAccel * 2) * m_pTimeMgr->GetDelta();

	if (m_bZoom == true)
	{
		if (m_pCondition->Get_RangeAttack())
			m_eActState = W_ZOOMFIRE;

		else if (!m_pCondition->Get_RangeAttack())
			m_eActState = W_ZOOMIN;

		if(velocity < 20)
			m_fZoomAccel -= 100.f * m_pTimeMgr->GetDelta();

		if (velocity >= 20)
			velocity = 20.f;
	}	

	if (m_bZoom == false)
	{
		if(m_eActState == W_ZOOMIN)
			m_eActState = W_ZOOMOUT;

		if (velocity > 20)
			velocity = 20.f;

		if (velocity > 1)
		{
			m_fZoomAccel += 100.f * m_pTimeMgr->GetDelta();
			dynamic_cast<CCamera*>(m_pCamera)->Set_AimZoom(70 - velocity);
		}

		if (velocity <= 1)
		{
			m_fZoomAccel = 0;
			dynamic_cast<CCamera*>(m_pCamera)->Set_AimZoom(70);
		}
	}

	//if (m_bZoom == false)
	//{
	//	if (velocity < 70)
	//		m_fZoomAccel += 15.f * m_pTimeMgr->GetDelta();
	//}

	//cout << velocity << endl;

	if (m_pKeyMgr->KeyPressing(ENGINE::KEY_RBUTTON))
	{
		dynamic_cast<CCamera*>(m_pCamera)->Set_AimZoom(70 - velocity);
		m_bZoom = true;
	}

	if (m_pKeyMgr->KeyUp(ENGINE::KEY_RBUTTON))
	{
		m_bZoom = false;
	}


}

void CPlayer::Set_WeaponInfo(ENGINE::W_INFO* _WeaponInfo)
{
	auto iter_find = m_mWeaponInfo.find(_WeaponInfo->eWeaponTag);

	if (m_mWeaponInfo.end() == iter_find)
	{
		ENGINE::W_INFO* pTag = new ENGINE::W_INFO;
		
		memcpy(pTag, _WeaponInfo, sizeof(ENGINE::W_INFO));

		m_mWeaponInfo.insert(make_pair(_WeaponInfo->eWeaponTag, pTag));
		m_mWeaponInfo[_WeaponInfo->eWeaponTag];
	}

	else
		m_mWeaponInfo[_WeaponInfo->eWeaponTag]->wCurBullet += _WeaponInfo->wCurBullet;
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
