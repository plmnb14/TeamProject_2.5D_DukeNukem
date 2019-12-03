#include "stdafx.h"
#include "Boss_CyberDemon.h"
#include "Camera.h"
#include "Trasform.h"
#include "Camera_Component.h"
#include "Bullet.h"
#include "FlameBullet.h"
#include "Rock.h"
#include "Collider.h"
#include "CameraObserver.h"
#include "RigidBody.h"
#include "Condition.h"
#include "SoundMgr.h"
#include "Billborad.h"
#include "Animator.h"
#include "Player.h"
#include "Effect_RocketSmoke.h"
#include "Effect_Explosion_Rocket.h"


CBoss_CyberDemon::CBoss_CyberDemon(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr),
	m_pTransform(nullptr), m_pCollider(nullptr), m_pGroundChekCollider(nullptr),
	m_pRigid(nullptr), m_pSubject(ENGINE::GetCameraSubject()), m_pPlayerSubject(ENGINE::GetPlayerSubject()),
	m_pObserver(nullptr), m_pCondition(nullptr), m_pBillboard(nullptr), m_pAnimator(nullptr),
	m_eState(CYBER_WALK), m_fLifeTime(0), m_fFowardDealy(0), m_bIsFindPlayer(false),
	m_bIsCharging(false), m_vDashDir(0, 0, 0), m_bIsJump(false)
{
}

CBoss_CyberDemon::~CBoss_CyberDemon()
{
	Release();
}

int CBoss_CyberDemon::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();

	State();
	Monster_Foward();

	return NO_EVENT;
}

void CBoss_CyberDemon::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();

	D3DXMatrixIdentity(&m_matView);

	D3DXMATRIX Localmatrix, Cameramatrix;													  //  로컬, 카메라 행렬 
	D3DXVECTOR3 vSize;																		  // 대상의 사이즈 
	Localmatrix = m_pTransform->GetWorldMatrix();

	if (m_pObserver != nullptr)
		Cameramatrix = m_pObserver->GetViewMatrix();

	vSize = m_pTransform->GetSize();


	m_pBillboard->Billborad_Yagnle(Localmatrix, Cameramatrix, vSize);                          // 빌보드 설정
	m_matView = m_pBillboard->GetWorldMatrix_Billborad();

	m_pCollider->LateUpdate(m_pTransform->GetPos());

	m_pGroundChekCollider->LateUpdate({ m_pTransform->GetPos().x ,
		m_pTransform->GetPos().y - m_pCollider->Get_Radius().y,
		m_pTransform->GetPos().z });

	Check_Physic();
}

void CBoss_CyberDemon::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matView);
	m_pTexture->Render(m_pAnimator->RenderSet(m_pTimeMgr->GetDelta()));
	m_pBuffer->Render();
}

HRESULT CBoss_CyberDemon::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	// 트랜스폼 세팅
	m_pTransform->SetPos(D3DXVECTOR3(40.f, 10.f, 10.f));
	m_pTransform->SetSize(D3DXVECTOR3(14.f, 14.f, 14.f));


	// 물리적 콜라이더
	m_pCollider->Set_Radius({ 5.f , 14.f, 5.f });			// 각 축에 해당하는 반지름을 설정
	m_pCollider->Set_Dynamic(true);							// 동적, 정적 Collider 유무
	m_pCollider->Set_Trigger(false);						// 트리거 유무
	m_pCollider->Set_CenterPos(m_pTransform->GetPos());		// Collider 의 정중앙좌표
	m_pCollider->Set_UnderPos();							// Collider 의 하단중앙 좌표
	m_pCollider->SetUp_Box();								// 설정된 것들을 Collider 에 반영합니다.
	m_pCollider->Set_MaxY(-m_pCollider->Get_Radius().y - 1);
	m_pCollider->Set_Type(ENGINE::COLLISION_AABB);


	// 트리거 콜라이더
	m_pGroundChekCollider->Set_Radius({ 0.3f , 0.3f, 0.3f });
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
	m_pRigid->Set_fPower(10.f);								// 점프 파워

	m_pRigid->Set_Speed({ 10.f , 10.f , 10.f });				// 각 축에 해당하는 속도
	m_pRigid->Set_Accel({ 1.f, 0.f, 0.f });					// 각 축에 해당하는 Accel 값
	m_pRigid->Set_MaxAccel({ 2.f , 4.f , 2.f });			// 각 축에 해당하는 MaxAccel 값


															// 컨디션
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

HRESULT CBoss_CyberDemon::LateInit()
{
	m_pObserver = CCameraObserver::Create();
	NULL_CHECK_RETURN(m_pObserver, E_FAIL);

	m_pSubject->Subscribe(m_pObserver);

	m_pTarget = m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player();

	return S_OK;
}

void CBoss_CyberDemon::Release()
{
	m_pSubject->UnSubscribe(m_pObserver);
	ENGINE::Safe_Delete(m_pObserver);
}

HRESULT CBoss_CyberDemon::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	// Texture
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, L"Overload_WalkFront");
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

	m_pGroundChekCollider = static_cast<ENGINE::CCollider*>(pComponent);
	NULL_CHECK_RETURN(m_pGroundChekCollider, E_FAIL);


	// Rigid
	pComponent = ENGINE::CRigidBody::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"RigidBody", pComponent });

	m_pRigid = static_cast<ENGINE::CRigidBody*>(pComponent);
	NULL_CHECK_RETURN(m_pRigid, E_FAIL);

	// Condition
	pComponent = ENGINE::CCondition::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Condition", pComponent });

	m_pCondition = static_cast<ENGINE::CCondition*>(pComponent);
	NULL_CHECK_RETURN(m_pCondition, E_FAIL);

	// Billboard
	pComponent = ENGINE::CBillborad::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Billboard", pComponent });

	m_pBillboard = static_cast<ENGINE::CBillborad*>(pComponent);
	NULL_CHECK_RETURN(m_pBillboard, E_FAIL);

	// Animator
	pComponent = ENGINE::CAnimator::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Animator", pComponent });

	m_pAnimator = static_cast<ENGINE::CAnimator*>(pComponent);
	NULL_CHECK_RETURN(m_pAnimator, E_FAIL);

	return S_OK;
}

void CBoss_CyberDemon::Check_Physic()
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

void CBoss_CyberDemon::State()
{
	if (D3DXVec3Length(&(m_vPlayer_Pos - m_pTransform->GetPos())) < 35.f)
	{
		m_bIsFindPlayer = true;
		cout << "Find Target!!!!!!!" << endl;
	}

	if (m_fPatternChangeDelay > 7.f)
	{
		m_fPatternChangeDelay = 0.f;
		m_eState = (CBoss_CyberDemon::CYBERDEMON_STATE)(m_eState + 1);
		if (m_eState == CYBER_DEAD)
			m_eState = CYBER_WALK;
	}
	else
		m_fPatternChangeDelay += m_pTimeMgr->GetDelta();

	if (!m_bIsFindPlayer)
		m_eState = CYBER_WALK;
	//else
	//	//test
	//	m_eState = CYBER_JUMP;

	switch (m_eState)
	{
	case CBoss_CyberDemon::CYBER_WALK:
	{
		Walk();
		break;
	}
	case CBoss_CyberDemon::CYBER_DASH:
	{
		Dash();
		break;
	}
	case CBoss_CyberDemon::CYBER_SLASH:
	{
		Slash();
		break;
	}
	case CBoss_CyberDemon::CYBER_FIRE:
	{
		Fire();
		break;
	}
	case CBoss_CyberDemon::CYBER_JUMP:
	{
		Jump();
		break;
	}
	case CBoss_CyberDemon::CYBER_DEAD:
	{
		ChangeTex(L"Cyber_Dead");
		break;
	}
	default:
		break;
	}

}

void CBoss_CyberDemon::Walk()
{
	if (m_bIsFindPlayer)
	{
		float fSpeed = m_pCondition->Get_MoveSpeed() * m_pTimeMgr->GetDelta();

		D3DXVECTOR3 vTempDir = static_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos() - m_pTransform->GetPos();
		D3DXVec3Normalize(&vTempDir, &vTempDir);
		vTempDir.y = 0;

		m_pTransform->Move_AdvancedPos(vTempDir, fSpeed);
		m_pTransform->SetDir(vTempDir);
	}
	else
	{
		if (m_fRandomWalkDelay > 4.f)
		{
			m_fRandomWalkDelay = 0.f;
			bool bIsMinus = (rand() % 2) ? true : false;
			
			if(bIsMinus) m_vRandomPos = m_pTransform->GetPos() + D3DXVECTOR3{(float)(rand() % 30), (float)(rand() % 30) , (float)(rand() % 30)};
			else		  m_vRandomPos = m_pTransform->GetPos() - D3DXVECTOR3{ (float)(rand() % 30), (float)(rand() % 30) , (float)(rand() % 30) };
		}
		else
			m_fRandomWalkDelay += m_pTimeMgr->GetDelta();

		float fSpeed = m_pCondition->Get_MoveSpeed() * m_pTimeMgr->GetDelta();

		D3DXVECTOR3 vTempDir = m_vRandomPos - m_pTransform->GetPos();
		D3DXVec3Normalize(&vTempDir, &vTempDir);
		vTempDir.y = 0;

		m_pTransform->Move_AdvancedPos(vTempDir, fSpeed);
		m_pTransform->SetDir(vTempDir);
	}
}

void CBoss_CyberDemon::Dash()
{
	float fSpeed = m_pCondition->Get_MoveSpeed() * m_pTimeMgr->GetDelta() * 5.f;

	if (!m_bIsCharging && m_fChargeDelay > 3.f)
	{
		m_bIsCharging = true;
	}
	else if (m_bIsCharging && m_fChargeDelay > 6.7f)
	{
		m_bIsCharging = false;
		m_fChargeDelay = 0.f;
	}
	else
	{
		m_fChargeDelay += m_pTimeMgr->GetDelta();
	}

	if (!m_bIsCharging)
	{
		m_vDashDir = static_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos() - m_pTransform->GetPos();
		D3DXVec3Normalize(&m_vDashDir, &m_vDashDir);
		m_vDashDir.y = 0;
		m_pTransform->SetDir(m_vDashDir);
	}
	else
	{
		m_pTransform->Move_AdvancedPos(m_vDashDir, fSpeed);
		m_pTransform->SetDir(m_vDashDir);

		CGameObject* pInstance = CEffect_RocketSmoke::Create(m_pGraphicDev, m_pCollider->Get_CenterPos(), D3DXVECTOR3(2, 2, 2));
		m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pInstance);
	}

}

void CBoss_CyberDemon::Slash()
{
	float fAngle[3] = { 0.f };
	float fBulletSpeed = 50.f;
	int	  iBulletCreatePosX[7] = { -4.5f, -3.f, -1.5f, 0.f, 1.5f, 3.f, 4.5f };
	int	  iBulletCreatePosZ[7] = { -4.5f, -3.f, -1.5f, 0.f, -1.5f, -3.f, -4.5f };
	int	  iBulletRandomY[3] = { -5.f, 0.f, 5.f };
	int	  iBulletRandomX[3] = { -3.f, 0.f, 3.f };

	D3DXVECTOR3 tmpDir = m_vPlayer_Pos - m_pTransform->GetPos();
	D3DXVECTOR3 tmpRight = {1,0,0};

	D3DXVec3Normalize(&tmpDir, &tmpDir);
	D3DXVec3Cross(&tmpRight, &tmpDir, &D3DXVECTOR3{ 0, 1, 0 });

	D3DXVECTOR3 tmpPos;
	D3DXVECTOR3 vPlayerTempPos;

	if (m_fSlashDelay > 1.f)
	{
		m_fSlashDelay = 0.f;
		int iRandX = rand() % 3;
		int iRandY = rand() % 3;
		bool bIsHorizon = (rand() % 2) ? true : false;

		for (int i = 0; i < 7; i++)
		{
			float fDirAngle = D3DXVec3Dot(&tmpDir, &m_pTransform->GetPos());
			D3DXMATRIX matRot;
			D3DXMatrixRotationY(&matRot, fDirAngle);

			if (bIsHorizon)
			{
				if (i <= 3)
				{
					tmpPos = { m_pTransform->GetPos().x + ((iBulletCreatePosX[i]) * tmpRight.x) - tmpDir.x * (0.5f * i),
						m_pTransform->GetPos().y + iBulletRandomY[iRandY],
						m_pTransform->GetPos().z + (iBulletCreatePosZ[i] * tmpRight.z) - tmpDir.z * (0.5f * i) };
				}
				else
				{
					tmpPos = { m_pTransform->GetPos().x - ((iBulletCreatePosX[i]) * tmpRight.x) - tmpDir.x * (0.5f * i),
						m_pTransform->GetPos().y + iBulletRandomY[iRandY],
						m_pTransform->GetPos().z - (iBulletCreatePosZ[i] * tmpRight.z) - tmpDir.z * (0.5f * i) };
				}
				

				vPlayerTempPos = { m_vPlayer_Pos.x + ((iBulletCreatePosX[i]) * tmpRight.x) - tmpDir.x * (0.5f * i),
					m_vPlayer_Pos.y + iBulletRandomY[iRandY],
					m_vPlayer_Pos.z + (iBulletCreatePosZ[i] * tmpRight.z) - tmpDir.z * (0.5f * i) };
			}
			else
			{
				tmpPos = { m_pTransform->GetPos().x - tmpDir.x * iBulletRandomX[iRandX],
					m_pTransform->GetPos().y + (i * 2) + iBulletRandomY[iRandY],
					m_pTransform->GetPos().z - tmpDir.z * iBulletCreatePosZ[i] };

				//vPlayerTempPos = { m_vPlayer_Pos.x - tmpDir.x * iBulletRandomX[iRandX],
				//	m_vPlayer_Pos.y + (i * 2) + iBulletRandomY[iRandY],
				//	m_vPlayer_Pos.z - tmpDir.z * iBulletCreatePosZ[i] };
			}
			
			CGameObject* pInstance = CFlameBullet::Create(
				m_pGraphicDev,
				tmpPos,
				tmpDir,
				fAngle,
				fBulletSpeed,
				ENGINE::MONSTER_REVOLVER);
			pInstance->Set_MapLayer(m_mapLayer);
			m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::BULLET_MONSTER, pInstance);

			ENGINE::CTransform* pBulletTr = static_cast<ENGINE::CTransform*>(pInstance->Get_Component(L"Transform"));
			pBulletTr->SetSize(D3DXVECTOR3(3, 3, 3));
		}
	}
	else
		m_fSlashDelay += m_pTimeMgr->GetDelta();
	
}

void CBoss_CyberDemon::Fire()
{
	float fAngle[3] = { 0.f };
	float fBulletSpeed = 120.f * m_pTimeMgr->GetDelta();

	// Set Dir
	D3DXVECTOR3 vTempDir = static_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos() - m_pTransform->GetPos();
	D3DXVec3Normalize(&vTempDir, &vTempDir);
	vTempDir.y = 0;

	m_pTransform->SetDir(vTempDir);

	// Fire
	if (m_fFireDelay > 0.5f)
	{
		m_fFireDelay = 0.f;

		CGameObject* pInstance = CBullet::Create(
			m_pGraphicDev,
			m_pTransform->GetPos(),
			m_vPlayer_Pos - m_pTransform->GetPos(),
			fAngle,
			fBulletSpeed,
			ENGINE::MONSTER_REVOLVER);
		pInstance->Set_MapLayer(m_mapLayer);
		m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::BULLET_MONSTER, pInstance);
	}
	else
		m_fFireDelay += m_pTimeMgr->GetDelta();
	
}

void CBoss_CyberDemon::Jump()
{
	if (!m_bIsJump)
	{
		m_pRigid->Set_Accel({ 1, 1.2f, 1 });
		m_pRigid->Set_IsJump(true);
		m_pRigid->Set_IsGround(false);

		m_bIsJump = true;
	}

	if (m_bIsJump && m_pRigid->Get_IsGround())
	{
		for (int i = 0; i < 5; i++)
		{
			D3DXVECTOR3 vTmpPos = m_vPlayer_Pos;
			D3DXVECTOR3 vTmpDir = m_vPlayer_Pos - m_pTransform->GetPos();

			D3DXVec3Normalize(&vTmpDir, &vTmpDir);

			D3DXVECTOR3 vTmpRight = { 1,0,0 };

			D3DXVec3Cross(&vTmpRight, &vTmpDir, &D3DXVECTOR3{ 0,1,0 });

			D3DXVECTOR3 vTmpPos_Plus = { vTmpPos.x + vTmpRight.x * 15, vTmpPos.y - 0.2f , vTmpPos.z + vTmpRight.z * (20 * i) };
			D3DXVECTOR3 vTmpPos_Minus{ vTmpPos.x - vTmpRight.x * 15, vTmpPos.y - 0.2f , vTmpPos.z - vTmpRight.z * (20 * i) };

			CGameObject* pInstance = CRock::Create(m_pGraphicDev);
			pInstance->Set_MapLayer(m_mapLayer);
			ENGINE::CTransform* pTransform = static_cast<ENGINE::CTransform*>(pInstance->Get_Component(L"Transform"));
			pTransform->SetPos(D3DXVECTOR3(vTmpPos_Plus.x, m_pTransform->GetPos().y - 15.f, vTmpPos_Plus.z));
			m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::TERRAIN, pInstance);

			pInstance = CRock::Create(m_pGraphicDev);
			pInstance->Set_MapLayer(m_mapLayer);
			pTransform = static_cast<ENGINE::CTransform*>(pInstance->Get_Component(L"Transform"));
			pTransform->SetPos(D3DXVECTOR3(vTmpPos_Minus.x, m_pTransform->GetPos().y - 15.f, vTmpPos_Minus.z));
			m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::TERRAIN, pInstance);
		}
		m_bIsJump = false;
		m_fPatternChangeDelay = 7.f;
	}

	
}

void CBoss_CyberDemon::Monster_Foward()
{
	Monster_Callcul();

	D3DXVECTOR3 vMonster_Player_Dir_Free = m_vPlayer_Pos - m_vMonster_Pos;    //왜필요하더라?

	D3DXVECTOR3 vMonsterDir_Forward = { 1.f, 0.f,0.f };                // 몬스터의 룩 벡터 X축 기준 룩 벡터 
																	   //m_pTransform->SetDir(vMonster_Player_Dir);
	D3DXVec3Normalize(&m_vMonster_Player_Dir, &m_vMonster_Player_Dir);
	D3DXVec3Normalize(&vMonsterDir_Forward, &vMonsterDir_Forward);
	D3DXVec3Normalize(&m_vMonsterDir_Fowrd_Get, &m_vMonsterDir_Fowrd_Get);

	float fDot_Player_Monster_Forward, fDot_Monster_Right, fMonster_Get_Angle;
	fDot_Player_Monster_Forward = D3DXVec3Dot(&m_vMonster_Player_Dir, &m_vMonsterDir_Fowrd_Get);
	D3DXVECTOR3 vMonsterDir_Right = { m_vMonsterDir_Fowrd_Get.x, m_vMonsterDir_Fowrd_Get.y + 1.0f, m_vMonsterDir_Fowrd_Get.z };      																   //m_pTransform->SetDir(vMonster_Player_Dir);

	D3DXVECTOR3 vMonster_RIght_Dir, Mon_Left_Dir, cross;

	D3DXVec3Cross(&vMonster_RIght_Dir, &m_vMonsterDir_Fowrd_Get, &vMonsterDir_Right); // 우향 벡터를 구하기 위한 외적 ECTOR3(0.0f, -1.0f, 0.0f)); // 좌향 벡터를 구하기 위한 외적 

	fDot_Monster_Right = D3DXVec3Dot(&vMonster_RIght_Dir, &m_vMonster_Player_Dir);                 // - 일때 오른쪽 +왼쪽이다. 
	//m_fFowardDealy += m_pTimeMgr->GetDelta();

	//if (m_fFowardDealy > 0.3)
	{
		if (fDot_Player_Monster_Forward * 90 > 67.5 && fDot_Player_Monster_Forward * 90 < 90 && fDot_Monster_Right > 0)
		{

			if (0 < fDot_Monster_Right * 90 && fDot_Monster_Right * 90 < 22.5)	
			{
				m_pAnimator->Stop_Animation(false);
				switch (m_eState)
				{
				case CBoss_CyberDemon::CYBER_MET:
					break;
				case CBoss_CyberDemon::CYBER_WALK:
					ChangeTex(L"Cyber_WalkFront");
					break;
				case CBoss_CyberDemon::CYBER_DASH:
					ChangeTex(L"Cyber_Dash_Front");
					break;
				case CBoss_CyberDemon::CYBER_SLASH:
					ChangeTex(L"Cyber_Slash");
					break;
				case CBoss_CyberDemon::CYBER_FIRE:
					ChangeTex(L"Cyber_Fire_Front");
					break;
				case CBoss_CyberDemon::CYBER_JUMP:
					ChangeTex(L"Cyber_Dash_Front");
					break;
				case CBoss_CyberDemon::CYBER_DEAD:
					break;
				default:
					break;
				}

				m_pAnimator->Set_FrameAmp(5.f);
			}

		}
		else if (fDot_Player_Monster_Forward * 90 > 67.5 && fDot_Player_Monster_Forward * 90 < 90 && fDot_Monster_Right < 0)
		{
			if (-22.5 < fDot_Monster_Right * 90 && fDot_Monster_Right * 180 < 0) 
			{
				m_pAnimator->Stop_Animation(false);
				switch (m_eState)
				{
				case CBoss_CyberDemon::CYBER_MET:
					break;
				case CBoss_CyberDemon::CYBER_WALK:
					ChangeTex(L"Cyber_WalkFront");
					break;
				case CBoss_CyberDemon::CYBER_DASH:
					ChangeTex(L"Cyber_Dash_Front");
					break;
				case CBoss_CyberDemon::CYBER_SLASH:
					ChangeTex(L"Cyber_Slash");
					break;
				case CBoss_CyberDemon::CYBER_FIRE:
					ChangeTex(L"Cyber_Fire_Front");
					break;
				case CBoss_CyberDemon::CYBER_JUMP:
					ChangeTex(L"Cyber_Dash_Front");
					break;
				case CBoss_CyberDemon::CYBER_DEAD:
					break;
				default:
					break;
				}
				m_pAnimator->Set_FrameAmp(5.f);
			}

		}
		else if (fDot_Player_Monster_Forward * 90 > 22.5 && fDot_Player_Monster_Forward * 90 < 67.5&& fDot_Monster_Right > 0)
		{
			if (22.5 < fDot_Monster_Right * 90 && fDot_Monster_Right * 90 < 67.5)  
			{
				m_pAnimator->Stop_Animation(false);
				switch (m_eState)
				{
				case CBoss_CyberDemon::CYBER_MET:
					break;
				case CBoss_CyberDemon::CYBER_WALK:
					ChangeTex(L"Cyber_WalkFrontSide_L");
					break;
				case CBoss_CyberDemon::CYBER_DASH:
					ChangeTex(L"Cyber_Dash_Front");
					break;
				case CBoss_CyberDemon::CYBER_SLASH:
					ChangeTex(L"Cyber_Slash");
					break;
				case CBoss_CyberDemon::CYBER_FIRE:
					ChangeTex(L"Cyber_Fire_FrontSide_L");
					break;
				case CBoss_CyberDemon::CYBER_JUMP:
					ChangeTex(L"Cyber_Dash_Front");
					break;
				case CBoss_CyberDemon::CYBER_DEAD:
					break;
				default:
					break;
				}
				m_pAnimator->Set_FrameAmp(5.f);
			}

		}
		else if (fDot_Player_Monster_Forward * 90 > 22.5 && fDot_Player_Monster_Forward * 90 < 67.5&& fDot_Monster_Right < 0)
		{
			if (-67.5 < fDot_Monster_Right * 90 && fDot_Monster_Right * 90 < -22.5) 
			{
				m_pAnimator->Stop_Animation(false);
				switch (m_eState)
				{
				case CBoss_CyberDemon::CYBER_MET:
					break;
				case CBoss_CyberDemon::CYBER_WALK:
					ChangeTex(L"Cyber_WalkFrontSide_R");
					break;
				case CBoss_CyberDemon::CYBER_DASH:
					ChangeTex(L"Cyber_Dash_Front");
					break;
				case CBoss_CyberDemon::CYBER_SLASH:
					ChangeTex(L"Cyber_Slash");
					break;
				case CBoss_CyberDemon::CYBER_FIRE:
					ChangeTex(L"Cyber_Fire_FrontSide_R");
					break;
				case CBoss_CyberDemon::CYBER_JUMP:
					ChangeTex(L"Cyber_Dash_Front");
					break;
				case CBoss_CyberDemon::CYBER_DEAD:
					break;
				default:
					break;
				}
				m_pAnimator->Set_FrameAmp(5.f);

			}

		}
		else if (fDot_Player_Monster_Forward * 90 > -22.5 && fDot_Player_Monster_Forward * 90 < 0 && fDot_Monster_Right < 0)
		{
			if (-90 < fDot_Monster_Right * 90 && fDot_Monster_Right * 90 < -67.5) 
			{
				m_pAnimator->Stop_Animation(false);
				switch (m_eState)
				{
				case CBoss_CyberDemon::CYBER_MET:
					break;
				case CBoss_CyberDemon::CYBER_WALK:
					ChangeTex(L"Cyber_WalkSide_R");
					break;
				case CBoss_CyberDemon::CYBER_DASH:
					ChangeTex(L"Cyber_Dash_Side_R");
					break;
				case CBoss_CyberDemon::CYBER_SLASH:
					ChangeTex(L"Cyber_Slash");
					break;
				case CBoss_CyberDemon::CYBER_FIRE:
					ChangeTex(L"Cyber_Fire_Side_R");
					break;
				case CBoss_CyberDemon::CYBER_JUMP:
					ChangeTex(L"Cyber_Dash_Side_R");
					break;
				case CBoss_CyberDemon::CYBER_DEAD:
					break;
				default:
					break;
				}
				m_pAnimator->Set_FrameAmp(5.f);
			}

		}
		else if (fDot_Player_Monster_Forward * 90 > -22.5 && fDot_Player_Monster_Forward * 90 < 0 && fDot_Monster_Right > 0)
		{
			if (67.5 < fDot_Monster_Right * 90 && fDot_Monster_Right * 90 < 90)
			{
				m_pAnimator->Stop_Animation(false);
				switch (m_eState)
				{
				case CBoss_CyberDemon::CYBER_MET:
					break;
				case CBoss_CyberDemon::CYBER_WALK:
					ChangeTex(L"Cyber_WalkSide_L");
					break;
				case CBoss_CyberDemon::CYBER_DASH:
					ChangeTex(L"Cyber_Dash_Side_L");
					break;
				case CBoss_CyberDemon::CYBER_SLASH:
					ChangeTex(L"Cyber_Slash");
					break;
				case CBoss_CyberDemon::CYBER_FIRE:
					ChangeTex(L"Cyber_Fire_Side_L");
					break;
				case CBoss_CyberDemon::CYBER_JUMP:
					ChangeTex(L"Cyber_Dash_Side_L");
					break;
				case CBoss_CyberDemon::CYBER_DEAD:
					break;
				default:
					break;
				}
				m_pAnimator->Set_FrameAmp(5.f);
			}

		}
		else if (fDot_Player_Monster_Forward * 90 > -67.5 && fDot_Player_Monster_Forward * 90 < -22.5 && fDot_Monster_Right < 0)
		{
			if (-67.5 < fDot_Monster_Right * 90 && fDot_Monster_Right * 90 < -22.5) 
			{
				m_pAnimator->Stop_Animation(false);
				switch (m_eState)
				{
				case CBoss_CyberDemon::CYBER_MET:
					break;
				case CBoss_CyberDemon::CYBER_WALK:
					ChangeTex(L"Cyber_WalkBackSide_R");
					break;
				case CBoss_CyberDemon::CYBER_DASH:
					ChangeTex(L"Cyber_Dash_Back");
					break;
				case CBoss_CyberDemon::CYBER_SLASH:
					ChangeTex(L"Cyber_Slash");
					break;
				case CBoss_CyberDemon::CYBER_FIRE:
					ChangeTex(L"Cyber_Fire_BackSide_R");
					break;
				case CBoss_CyberDemon::CYBER_JUMP:
					ChangeTex(L"Cyber_Dash_Back");
					break;
				case CBoss_CyberDemon::CYBER_DEAD:
					break;
				default:
					break;
				}
				m_pAnimator->Set_FrameAmp(5.f);
			}

		}
		else if (fDot_Player_Monster_Forward * 90 > -67.5 && fDot_Player_Monster_Forward * 90 < -22.5 && fDot_Monster_Right > 0)
		{
			if (22.5 < fDot_Monster_Right * 90 && fDot_Monster_Right * 90 < 67.5) 
			{
				m_pAnimator->Stop_Animation(false);
				switch (m_eState)
				{
				case CBoss_CyberDemon::CYBER_MET:
					break;
				case CBoss_CyberDemon::CYBER_WALK:
					ChangeTex(L"Cyber_WalkBackSide_L");
					break;
				case CBoss_CyberDemon::CYBER_DASH:
					ChangeTex(L"Cyber_Dash_Back");
					break;
				case CBoss_CyberDemon::CYBER_SLASH:
					ChangeTex(L"Cyber_Slash");
					break;
				case CBoss_CyberDemon::CYBER_FIRE:
					ChangeTex(L"Cyber_Fire_BackSide_L");
					break;
				case CBoss_CyberDemon::CYBER_JUMP:
					ChangeTex(L"Cyber_Dash_Back");
					break;
				case CBoss_CyberDemon::CYBER_DEAD:
					break;
				default:
					break;
				}
				m_pAnimator->Set_FrameAmp(5.f);
			}

		}
		else if (fDot_Player_Monster_Forward * 90 > -90 && fDot_Player_Monster_Forward * 90 < -67.5 && fDot_Monster_Right > 0)
		{
			if (0 < fDot_Monster_Right * 90 && fDot_Monster_Right * 90 < 22.5) 
			{
				m_pAnimator->Stop_Animation(false);
				switch (m_eState)
				{
				case CBoss_CyberDemon::CYBER_MET:
					break;
				case CBoss_CyberDemon::CYBER_WALK:
					ChangeTex(L"Cyber_WalkBack");
					break;
				case CBoss_CyberDemon::CYBER_DASH:
					ChangeTex(L"Cyber_Dash_Back");
					break;
				case CBoss_CyberDemon::CYBER_SLASH:
					ChangeTex(L"Cyber_Slash");
					break;
				case CBoss_CyberDemon::CYBER_FIRE:
					ChangeTex(L"Cyber_Fire_Back");
					break;
				case CBoss_CyberDemon::CYBER_JUMP:
					ChangeTex(L"Cyber_Dash_Back");
					break;
				case CBoss_CyberDemon::CYBER_DEAD:
					break;
				default:
					break;
				}
				m_pAnimator->Set_FrameAmp(5.f);
			}

		}
		else if (fDot_Player_Monster_Forward * 90 > -90 && fDot_Player_Monster_Forward * 90 < -67.5 && fDot_Monster_Right < 0)
		{
			if (-22.5 < fDot_Monster_Right * 90 && fDot_Monster_Right * 90 < 0)
			{
				m_pAnimator->Stop_Animation(false);
				switch (m_eState)
				{
				case CBoss_CyberDemon::CYBER_MET:
					break;
				case CBoss_CyberDemon::CYBER_WALK:
					ChangeTex(L"Cyber_WalkBack");
					break;
				case CBoss_CyberDemon::CYBER_DASH:
					ChangeTex(L"Cyber_Dash_Back");
					break;
				case CBoss_CyberDemon::CYBER_SLASH:
					ChangeTex(L"Cyber_Slash");
					break;
				case CBoss_CyberDemon::CYBER_FIRE:
					ChangeTex(L"Cyber_Fire_Back");
					break;
				case CBoss_CyberDemon::CYBER_JUMP:
					ChangeTex(L"Cyber_Dash_Back");
					break;
				case CBoss_CyberDemon::CYBER_DEAD:
					break;
				default:
					break;
				}
				m_pAnimator->Set_FrameAmp(5.f);
			}

		}
	}
}

void CBoss_CyberDemon::Monster_Callcul()
{
	m_vPlayer_Pos = static_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();
	m_vMonster_Pos = m_pTransform->GetPos();
	m_vPlayer_Pos_Top = { m_vPlayer_Pos.x, m_vPlayer_Pos.y + 3 ,m_vPlayer_Pos.z };
	m_vMonster_Player_Dir = m_vPlayer_Pos_Top - m_vMonster_Pos;
	m_vMonsterDir_Fowrd_Get = m_pTransform->GetDir();
}


void CBoss_CyberDemon::ChangeTex(wstring _wstrTex)
{
	if (m_wstrTex.compare(_wstrTex) == 0)
		return;

	m_wstrTex = _wstrTex;

	m_mapComponent.erase(L"Texture");

	ENGINE::CComponent* pComponent = nullptr;
	pComponent = ENGINE::GetResourceMgr()->CloneResource(ENGINE::RESOURCE_STATIC, _wstrTex);
	NULL_CHECK(pComponent);
	m_mapComponent.insert({ L"Texture", pComponent });

	m_pAnimator->Set_MaxFrame(static_cast<ENGINE::CResources*>(pComponent)->Get_MaxFrame());

	m_pTexture = static_cast<ENGINE::CTexture*>(pComponent);
	NULL_CHECK(m_pTexture);
}

CBoss_CyberDemon* CBoss_CyberDemon::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CBoss_CyberDemon* pInstance = new CBoss_CyberDemon(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}
