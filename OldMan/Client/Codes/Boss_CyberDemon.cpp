#include "stdafx.h"
#include "Boss_CyberDemon.h"
#include "Camera.h"
#include "Trasform.h"
#include "Camera_Component.h"
#include "Bullet.h"
#include "FlameBullet.h"
#include "MonsterBullet.h"
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
#include "GaugeBar.h"


CBoss_CyberDemon::CBoss_CyberDemon(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr),
	m_pTransform(nullptr), m_pCollider(nullptr), m_pGroundChekCollider(nullptr),
	m_pRigid(nullptr), m_pSubject(ENGINE::GetCameraSubject()), m_pPlayerSubject(ENGINE::GetPlayerSubject()),
	m_pBossSubject(ENGINE::GetBossSubject()),
	m_pObserver(nullptr), m_pCondition(nullptr), m_pBillboard(nullptr), m_pAnimator(nullptr),
	m_eState(CYBER_WALK), m_fLifeTime(0), m_fFowardDealy(0), m_bIsFindPlayer(false),
	m_bIsCharging(false), m_vDashDir(0, 0, 0), m_bIsJump(false), m_fAccel(1), m_pHpBar(nullptr),
	m_bIsChargeHitted(false), m_fOldHp(0.f), m_fHittedSoundDelay(0.f), m_bIsPlayedDeadSound(false)
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
	HittedSound();

	m_tCondition.fHp = m_pCondition->Get_Hp();
	m_pBossSubject->AddData(ENGINE::CBossSubject::BOSS_INFO, &(m_tCondition));

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
	m_pMelleCollider->LateUpdate(m_pTransform->GetPos());

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

	// 트리거 콜라이더     인식범위랑 비슷하게 필요하다 
	m_pMelleCollider->Set_Radius(m_pCollider->Get_Radius() * 1.2);
	m_pMelleCollider->Set_Dynamic(true);
	m_pMelleCollider->Set_Trigger(true);
	m_pMelleCollider->Set_CenterPos({ m_pTransform->GetPos().x ,
		m_pTransform->GetPos().y - m_pCollider->Get_Radius().y,
		m_pTransform->GetPos().z });
	m_pMelleCollider->Set_UnderPos();
	m_pMelleCollider->SetUp_Box();
	m_pMelleCollider->Set_Enabled(false);
	m_pMelleCollider->Set_Type(ENGINE::COLLISION_AABB);


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
	m_pCondition->Set_Hp(5000.f);
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

	m_fOldHp = 5000.f;

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

	// MEELE
	pComponent = ENGINE::CCollider::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Monster_Mell", pComponent });

	m_pMelleCollider = static_cast<ENGINE::CCollider*>(pComponent);
	NULL_CHECK_RETURN(m_pCollider, E_FAIL);

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
	if (m_eState == CYBER_DEAD)
	{
		static_cast<CGaugeBar*>(m_pHpBar)->SetVisible(false);

		if (!m_bIsPlayedDeadSound)
		{
			CSoundMgr::GetInstance()->SetVolume(CSoundMgr::MONSTER, 1.0f);
			CSoundMgr::GetInstance()->StopSound(CSoundMgr::MONSTER);
			CSoundMgr::GetInstance()->MyPlaySound(L"CyberDemon_Dead.mp3", CSoundMgr::MONSTER);

			m_bIsPlayedDeadSound = true;
		}
		
		return;
	}

	if (!m_bIsFindPlayer && D3DXVec3Length(&(m_vPlayer_Pos - m_pTransform->GetPos())) < 35.f)
	{
		m_bIsFindPlayer = true;

		m_pHpBar = CGaugeBar::Create(m_pGraphicDev, CGaugeBar::BAR_BOSSHP);
		m_mapLayer[ENGINE::CLayer::UI]->AddObject(ENGINE::OBJECT_TYPE::UI, m_pHpBar);
		m_pHpBar->Set_MapLayer(m_mapLayer);
		static_cast<CUI*>(m_pHpBar)->SetSize(30.f, 30.f);
		static_cast<CUI*>(m_pHpBar)->SetPos(D3DXVECTOR3(0.f, 280.f, 0.f));
	}

	if (m_fPatternChangeDelay > 3.f)
	{
		m_fPatternChangeDelay = 0.f;
		m_eState = (CBoss_CyberDemon::CYBERDEMON_STATE)(m_eState + 1);
		if (m_pCondition->Get_Hp() > 0 && m_eState == CYBER_DEAD)
			m_eState = CYBER_WALK;
	}
	else
		m_fPatternChangeDelay += m_pTimeMgr->GetDelta();

	if (!m_bIsFindPlayer)
		m_eState = CYBER_WALK;

	if (m_pCondition->Get_Hp() <= 0)
		m_eState = CYBER_DEAD;

	if (m_eState == CYBER_DEAD)
	{
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_STOP);
	}

	switch (m_eState)
	{
	case CBoss_CyberDemon::CYBER_WALK:
	{
		Walk();
		break;
	}
	case CBoss_CyberDemon::CYBER_DASH:
	{
		m_pMelleCollider->Set_Enabled(true);
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

void CBoss_CyberDemon::HittedSound()
{
	if (m_fHittedSoundDelay > 1.f && (m_fOldHp > m_pCondition->Get_Hp()))
	{
		m_fHittedSoundDelay = 0.f;

		CSoundMgr::GetInstance()->SetVolume(CSoundMgr::MONSTER_VOICE, 1.0f);
		CSoundMgr::GetInstance()->StopSound(CSoundMgr::MONSTER_VOICE);

		int iRand = rand() % 4;
		switch (iRand)
		{
		case 0:
			CSoundMgr::GetInstance()->MyPlaySound(L"CyberDemon_Hitted_1.mp3", CSoundMgr::MONSTER_VOICE);
			break;
		case 1:
			CSoundMgr::GetInstance()->MyPlaySound(L"CyberDemon_Hitted_2.mp3", CSoundMgr::MONSTER_VOICE);
			break;
		case 2:
			CSoundMgr::GetInstance()->MyPlaySound(L"CyberDemon_Hitted_3.mp3", CSoundMgr::MONSTER_VOICE);
			break;
		case 3:
			CSoundMgr::GetInstance()->MyPlaySound(L"CyberDemon_Hitted_4.mp3", CSoundMgr::MONSTER_VOICE);
			break;
		}

		m_fOldHp = m_pCondition->Get_Hp();
	}
	else
		m_fHittedSoundDelay += m_pTimeMgr->GetDelta();
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


	if (m_fWalkSoundDelay > 1.f)
	{
		m_fWalkSoundDelay = 0.f;

		CSoundMgr::GetInstance()->SetVolume(CSoundMgr::MONSTER_EFF, 1.0f);
		CSoundMgr::GetInstance()->StopSound(CSoundMgr::MONSTER_EFF);

		int iRand = rand() % 5;
		switch (iRand)
		{
		case 0:
			CSoundMgr::GetInstance()->MyPlaySound(L"CyberDemon_Walk_1.mp3", CSoundMgr::MONSTER_EFF);
			break;
		case 1:
			CSoundMgr::GetInstance()->MyPlaySound(L"CyberDemon_Walk_2.mp3", CSoundMgr::MONSTER_EFF);
			break;
		case 2:
			CSoundMgr::GetInstance()->MyPlaySound(L"CyberDemon_Walk_3.mp3", CSoundMgr::MONSTER_EFF);
			break;
		case 3:
			CSoundMgr::GetInstance()->MyPlaySound(L"CyberDemon_Walk_4.mp3", CSoundMgr::MONSTER_EFF);
			break;
		case 4:
			CSoundMgr::GetInstance()->MyPlaySound(L"CyberDemon_Walk_4.mp3", CSoundMgr::MONSTER_EFF);
			break;
		}
	}
	else
		m_fWalkSoundDelay += m_pTimeMgr->GetDelta();
}

void CBoss_CyberDemon::Dash()
{
	float fSpeed = m_pCondition->Get_MoveSpeed() * m_pTimeMgr->GetDelta() * m_fAccel * m_fAccel * 5.f;

	if (!m_bIsCharging && m_fChargeDelay > 0.5f)
	{
		m_fAccel = 3.f;
		m_bIsCharging = true;

		CSoundMgr::GetInstance()->SetVolume(CSoundMgr::MONSTER_EFF, 1.0f);
		CSoundMgr::GetInstance()->StopSound(CSoundMgr::MONSTER_EFF);

		int iRand = rand() % 3;
		switch (iRand)
		{
		case 0:
			CSoundMgr::GetInstance()->MyPlaySound(L"CyberDemon_Dash_1.mp3", CSoundMgr::MONSTER_EFF);
			break;
		case 1:
			CSoundMgr::GetInstance()->MyPlaySound(L"CyberDemon_Dash_2.mp3", CSoundMgr::MONSTER_EFF);
			break;
		case 2:
			CSoundMgr::GetInstance()->MyPlaySound(L"CyberDemon_Dash_3.mp3", CSoundMgr::MONSTER_EFF);
			break;
		}

		CSoundMgr::GetInstance()->SetVolume(CSoundMgr::MONSTER, 1.0f);
		CSoundMgr::GetInstance()->StopSound(CSoundMgr::MONSTER);
		CSoundMgr::GetInstance()->MyPlaySound(L"CyberDemon_DashEff.mp3", CSoundMgr::MONSTER);

	}
	else if (m_bIsCharging && m_fChargeDelay > 2.5f)
	{
		m_bIsCharging = false;
		m_fChargeDelay = 0.f;
		m_pMelleCollider->Set_Enabled(false);
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
		if (m_fAccel >= 1)
			m_fAccel -= 0.1f;

		else if (m_fAccel < 1)
			m_fAccel = 1;

		// 플레이어 피격
		float fDashDmg = 30.f;
		if (!m_bIsChargeHitted && m_pMelleCollider->Get_IsCollision())
		{
			static_cast<ENGINE::CCondition*>(m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player()->Get_Component(L"Condition"))->Add_Hp(-fDashDmg);
			m_bIsChargeHitted = false;

			CSoundMgr::GetInstance()->SetVolume(CSoundMgr::PLAYER_VOICE, 1.0f);
			CSoundMgr::GetInstance()->StopSound(CSoundMgr::PLAYER_VOICE);
			int iSound = rand() % 4;

			switch (iSound)
			{
			case 0:
				CSoundMgr::GetInstance()->MyPlaySound(L"Player_Hitted_1.mp3", CSoundMgr::PLAYER_VOICE);
				break;
			case 1:
				CSoundMgr::GetInstance()->MyPlaySound(L"Player_Hitted_2.mp3", CSoundMgr::PLAYER_VOICE);
				break;
			case 2:
				CSoundMgr::GetInstance()->MyPlaySound(L"Player_Hitted_3.mp3", CSoundMgr::PLAYER_VOICE);
				break;
			case 3:
				CSoundMgr::GetInstance()->MyPlaySound(L"Player_Hitted_4.mp3", CSoundMgr::PLAYER_VOICE);
				break;
			}
		}

		m_pTransform->Move_AdvancedPos(m_vDashDir, fSpeed);
		m_pTransform->SetDir(m_vDashDir);

		CGameObject* pInstance = CEffect_RocketSmoke::Create(m_pGraphicDev, m_pCollider->Get_CenterPos(), D3DXVECTOR3(3, 3, 3));
		m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pInstance);
	}

}

void CBoss_CyberDemon::Slash()
{
	float fAngle[3] = { 0.f };
	float fBulletSpeed = 50.f;
	int	  iBulletCreatePosX[7] = { -4.5f, -3.f, -1.5f, 0.f, 1.5f, 3.f, 4.5f };
	int	  iBulletCreatePosZ[7] = { -4.5f, -3.f, -1.5f, 0.f, -1.5f, -3.f, -4.5f };
	int	  iBulletRandomY[3] = { -11.f, -6.f, -3.f };
	int	  iBulletRandomX[3] = { -4.f, -1.f, 2.f };

	D3DXVECTOR3 tmpPos_0 = static_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();
	tmpPos_0.y = m_pTransform->GetPos().y;
	D3DXVECTOR3 tmpDir = tmpPos_0 - m_pTransform->GetPos();
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
		int iMinus = 1;

		for (int i = 0; i < 14; i++)
		{
			float fDirAngle = D3DXVec3Dot(&tmpDir, &m_pTransform->GetPos());
			D3DXMATRIX matRot;
			D3DXVECTOR3 vSize = {0,0,0};
			D3DXMatrixRotationY(&matRot, fDirAngle);

			// 가로
			if (bIsHorizon)
			{
				if (i < 8)
				{
					tmpPos = { m_pTransform->GetPos().x + tmpRight.x * (i * 2 - 7 * 2) + tmpDir.x * (i) * 0.2f,
					m_pTransform->GetPos().y + iBulletRandomY[iRandY],
					m_pTransform->GetPos().z + tmpRight.z * (i * 2 - 7 * 2) + tmpDir.z * (i) * 0.2f };

					vSize = { 2 + i * 0.5f , 2 + i * 0.5f, 2 + i * 0.5f };
				}

				else
				{
					tmpPos = { m_pTransform->GetPos().x + tmpRight.x * (i * 2 - 7 * 2) + tmpDir.x * (i - 2 * iMinus) * 0.2f,
						m_pTransform->GetPos().y + iBulletRandomY[iRandY],
						m_pTransform->GetPos().z + tmpRight.z * (i * 2 - 7 * 2) + tmpDir.z * (i - 2 * iMinus) * 0.2f };

					vSize = { 2 + i * 0.5f - (i - 7) , 2 + i * 0.5f - (i - 7), 2 + i * 0.5f - (i - 7) };

					++iMinus;
				}
			}

			// 세로
			else
			{
				if (i < 8)
				{
					tmpPos = { m_pTransform->GetPos().x + tmpDir.x * (i) * 0.3f,
						m_pTransform->GetPos().y + 10 - i * 2,
						m_pTransform->GetPos().z  + tmpDir.z * (i) * 0.3f };

					vSize = { 2 + i * 0.5f , 2 + i * 0.5f, 2 + i * 0.5f };
				}

				else
				{
					tmpPos = { m_pTransform->GetPos().x  + tmpDir.x * (i - 2 * iMinus) * 0.3f,
						m_pTransform->GetPos().y + 10 - i * 2,
						m_pTransform->GetPos().z + tmpDir.z * (i - 2 * iMinus) * 0.3f };

					vSize = { 2 + i * 0.5f - (i - 7) , 2 + i * 0.5f - (i - 7), 2 + i * 0.5f - (i - 7) };

					++iMinus;
				}
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
			pBulletTr->SetSize(vSize);
		}

		CSoundMgr::GetInstance()->SetVolume(CSoundMgr::MONSTER, 1.0f);
		CSoundMgr::GetInstance()->StopSound(CSoundMgr::MONSTER);

		int iRand = rand() % 2;
		switch (iRand)
		{
		case 0:
			CSoundMgr::GetInstance()->MyPlaySound(L"CyberDemon_Slash_1.mp3", CSoundMgr::MONSTER);
			break;
		case 1:
			CSoundMgr::GetInstance()->MyPlaySound(L"CyberDemon_Slash_2.mp3", CSoundMgr::MONSTER);
			break;
		}

	}
	else
		m_fSlashDelay += m_pTimeMgr->GetDelta() * 3;
	
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

		CGameObject* pInstance = CMonsterBullet::Create(
			m_pGraphicDev,
			m_pTransform->GetPos(),
			m_vPlayer_Pos - m_pTransform->GetPos(),
			fAngle,
			fBulletSpeed,
			ENGINE::MONSTER_REVOLVER);
		pInstance->Set_MapLayer(m_mapLayer);
		m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::BULLET_MONSTER, pInstance);
		static_cast<ENGINE::CTransform*>(pInstance->Get_Component(L"Transform"))->SetSize(D3DXVECTOR3(3, 3, 3));

		CSoundMgr::GetInstance()->SetVolume(CSoundMgr::MONSTER, 1.0f);
		CSoundMgr::GetInstance()->StopSound(CSoundMgr::MONSTER);
		CSoundMgr::GetInstance()->MyPlaySound(L"CyberDemon_Fire_1.mp3", CSoundMgr::MONSTER);
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
		for (int i = 0; i < 40; i++)
		{
			float x = rand() % 20 * 0.05f;
			float z = rand() % 20 * 0.05f;

			D3DXVECTOR3 tmpPos_0 = static_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();
			D3DXVECTOR3 vTmpDir = tmpPos_0 - m_pTransform->GetPos();

			D3DXVec3Normalize(&vTmpDir, &vTmpDir);

			D3DXVECTOR3 vTmpRight = { 1,0,0 };

			D3DXVec3Cross(&vTmpRight, &vTmpDir, &D3DXVECTOR3{ 0,1,0 });

			D3DXVECTOR3 vTmpPos_Plus_1 = { tmpPos_0.x + vTmpRight.x * 17 + x - vTmpDir.x * (i * 4), tmpPos_0.y , tmpPos_0.z + vTmpRight.z * 17 + x - vTmpDir.z * (i * 4) };
			D3DXVECTOR3 vTmpPos_Plus_2 = { tmpPos_0.x + vTmpRight.x * 17 + x + vTmpDir.x * (i * 4), tmpPos_0.y , tmpPos_0.z + vTmpRight.z * 17 + x + vTmpDir.z * (i * 4) };
			D3DXVECTOR3 vTmpPos_Minus_1 = { tmpPos_0.x - vTmpRight.x * 17 + z - vTmpDir.x * (i * 4), tmpPos_0.y , tmpPos_0.z - vTmpRight.z * 17 + z - vTmpDir.z * (i * 4) };
			D3DXVECTOR3 vTmpPos_Minus_2 = { tmpPos_0.x - vTmpRight.x * 17 + z + vTmpDir.x * (i * 4), tmpPos_0.y , tmpPos_0.z - vTmpRight.z * 17 + z + vTmpDir.z * (i * 4) };

			CGameObject* pInstance = CRock::Create(m_pGraphicDev , i * 0.01f);
			pInstance->Set_MapLayer(m_mapLayer);
			ENGINE::CTransform* pTransform = static_cast<ENGINE::CTransform*>(pInstance->Get_Component(L"Transform"));
			pTransform->SetPos(D3DXVECTOR3(vTmpPos_Plus_1.x, m_pTransform->GetPos().y - 24.f, vTmpPos_Plus_1.z));
			m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::TERRAIN, pInstance);

			pInstance = CRock::Create(m_pGraphicDev , i * 0.01f);
			pInstance->Set_MapLayer(m_mapLayer);
			pTransform = static_cast<ENGINE::CTransform*>(pInstance->Get_Component(L"Transform"));
			pTransform->SetPos(D3DXVECTOR3(vTmpPos_Minus_1.x, m_pTransform->GetPos().y - 24.f, vTmpPos_Minus_1.z));
			m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::TERRAIN, pInstance);


			pInstance = CRock::Create(m_pGraphicDev, i * 0.01f);
			pInstance->Set_MapLayer(m_mapLayer);
			pTransform = static_cast<ENGINE::CTransform*>(pInstance->Get_Component(L"Transform"));
			pTransform->SetPos(D3DXVECTOR3(vTmpPos_Plus_2.x, m_pTransform->GetPos().y - 24.f, vTmpPos_Plus_2.z));
			m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::TERRAIN, pInstance);

			pInstance = CRock::Create(m_pGraphicDev, i * 0.01f);
			pInstance->Set_MapLayer(m_mapLayer);
			pTransform = static_cast<ENGINE::CTransform*>(pInstance->Get_Component(L"Transform"));
			pTransform->SetPos(D3DXVECTOR3(vTmpPos_Minus_2.x, m_pTransform->GetPos().y - 24.f, vTmpPos_Minus_2.z));
			m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::TERRAIN, pInstance);
		}
		m_bIsJump = false;
		m_fPatternChangeDelay = 5.f;

		CSoundMgr::GetInstance()->SetVolume(CSoundMgr::MONSTER_EFF, 1.0f);
		CSoundMgr::GetInstance()->StopSound(CSoundMgr::MONSTER_EFF);
		CSoundMgr::GetInstance()->MyPlaySound(L"CyberDemon_Jump.mp3", CSoundMgr::MONSTER_EFF);

		CSoundMgr::GetInstance()->SetVolume(CSoundMgr::MONSTER, 1.0f);
		CSoundMgr::GetInstance()->StopSound(CSoundMgr::MONSTER);
		CSoundMgr::GetInstance()->MyPlaySound(L"CyberDemon_RockCreate.ogg", CSoundMgr::MONSTER);
	}

	D3DXVECTOR3 vTempDir = m_vRandomPos - m_pTransform->GetPos();
	D3DXVec3Normalize(&vTempDir, &vTempDir);
	vTempDir.y = 0;

	m_pTransform->SetDir(vTempDir);
	
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
