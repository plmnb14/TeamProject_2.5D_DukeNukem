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
#include "SoundMgr.h"
#include "Grenade.h"
#include "HittedCircle.h"
#include "Effect_BloodSplit.h"


CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pKeyMgr(ENGINE::GetKeyMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr),
	m_pTransform(nullptr), m_pCollider(nullptr), m_pGroundChekCollider(nullptr), m_pColliderLedge(nullptr),
	m_pRigid(nullptr), m_fSlideUp(0),
	m_pSubject(ENGINE::GetCameraSubject()), m_pPlayerSubject(ENGINE::GetPlayerSubject()),
	m_eWeaponState(ENGINE::WEAPON_TAG::MELLE), m_fZoomAccel(0),
	m_pObserver(nullptr), m_bZoom(false), m_fMaxZoom(0), m_fMinZoom(0), m_bSpecial(0),
	m_bCanAttack(true), m_vLedgeVec({ 0,0,0 }), m_bIsLedge(0), m_bCanLedge(0), m_fLength_Y(0),
	m_eActState(W_DRAW), m_vLedgeUpVec({ 0,0,0 }), m_fHorizontal(0), m_iJumpCount(0), m_bGrenade(0),
	m_iGrenadeCount(0), m_iMaxGrenadeCount(0), m_iWaypoint_Index(0),
	m_fWalkSoundDelay(0.f), m_iWalkSoundIndex(0), m_bPlaySlideSound(false), m_pMeleeCollider(nullptr),
	m_bOnNextStage(false), m_DeathTimer(0)
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
	{
		m_DeathTimer += m_pTimeMgr->GetDelta();

		if (m_DeathTimer >= 3.f)
		{
			m_bOnNextStage = true;
		}

		return NO_EVENT;
	}
	
	if (m_pMeleeCollider->Get_IsCollision())
	{
		//D3DXVECTOR3 tmpDir = m_pTransform->GetDir();
		//D3DXVECTOR3 tmpRight = {};
		//
		//D3DXVec3Cross(&tmpRight, &tmpDir, &D3DXVECTOR3{ 0,1,0 });
		//
		//D3DXVECTOR3 tmpPos_Melee = { m_pTransform->GetPos().x + tmpRight.x * 2
		//	, m_pTransform->GetPos().y + 1,
		//	m_pTransform->GetPos().z + tmpRight.z * 2};

		int iSound = rand() % 6;

		CSoundMgr::GetInstance()->SetVolume(CSoundMgr::WEAPON_AFTER, 0.5f);
		CSoundMgr::GetInstance()->StopSound(CSoundMgr::WEAPON_AFTER);
		switch (iSound)
		{
		case 0:
			CSoundMgr::GetInstance()->MyPlaySound(L"Fist_Hit_01.mp3", CSoundMgr::WEAPON_AFTER);
			break;
		case 1:
			CSoundMgr::GetInstance()->MyPlaySound(L"Fist_Hit_02.mp3", CSoundMgr::WEAPON_AFTER);
			break;
		case 2:
			CSoundMgr::GetInstance()->MyPlaySound(L"Fist_Hit_03.mp3", CSoundMgr::WEAPON_AFTER);
			break;
		case 3:
			CSoundMgr::GetInstance()->MyPlaySound(L"Fist_Hit_04.mp3", CSoundMgr::WEAPON_AFTER);
			break;
		case 4:
			CSoundMgr::GetInstance()->MyPlaySound(L"Fist_Hit_05.mp3", CSoundMgr::WEAPON_AFTER);
			break;
		case 5:
			CSoundMgr::GetInstance()->MyPlaySound(L"Fist_Hit_06.mp3", CSoundMgr::WEAPON_AFTER);
			break;

			m_pMeleeCollider->Set_IsCollision(false);
		}


		D3DXVECTOR3 tmpDir = m_pTransform->GetDir();
		D3DXVECTOR3 tmpPos = { m_pTransform->GetPos().x + tmpDir.x * 3 , m_pTransform->GetPos().y + tmpDir.y * 3 + 1, m_pTransform->GetPos().z + tmpDir.z * 3 };

		CGameObject* pInstance = CEffect_BloodSplit::Create(m_pGraphicDev, tmpPos);
		m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pInstance);

		m_pMeleeCollider->Set_IsCollision(false);
	}

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();
	KeyInput();

	Check_Slide();
	Check_Run();
	Check_Physic();
	Check_Ledge();
	Check_Grenade();
	Check_Dead();
	Check_Hitted();
	
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

	D3DXVECTOR3 tmpDir = m_pTransform->GetDir();
	D3DXVECTOR3 tmpPos = { m_pTransform->GetPos().x + tmpDir.x , m_pTransform->GetPos().y + 1 , m_pTransform->GetPos().z + tmpDir.z };

	m_pColliderLedge->LateUpdate(tmpPos);

	//D3DXVECTOR3 tmpRight = {};
	//D3DXVec3Cross(&tmpRight, &tmpDir, &D3DXVECTOR3{ 0,1,0 });
	//
	//D3DXVECTOR3 tmpPos_Melee = { m_pTransform->GetPos().x + (m_pCollider->Get_Radius().x + tmpRight.x) 
	//							, m_pTransform->GetPos().y,
	//							m_pTransform->GetPos().z + (m_pCollider->Get_Radius().z * tmpRight.z) };
	//
	m_pMeleeCollider->LateUpdate(tmpPos);
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
	m_pTransform->SetPos(D3DXVECTOR3(0.f, 8.f, 2.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 2.f, 1.f));
	
	
	// 물리적 콜라이더
	m_pCollider->Set_Radius({ 0.9f , 2.5f, 0.9f });			// 각 축에 해당하는 반지름을 설정
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
	
	
	D3DXVECTOR3 tmpDir = m_pTransform->GetDir();
	D3DXVECTOR3 tmpRight = {};

	D3DXVec3Cross(&tmpRight, &tmpDir, &D3DXVECTOR3{0,1,0});

	D3DXVECTOR3 tmpPos = { m_pTransform->GetPos().x + (m_pCollider->Get_Radius().x + tmpRight.x) , m_pTransform->GetPos().y + 0.5f, m_pTransform->GetPos().z + (m_pCollider->Get_Radius().z * tmpRight.z) };

	// 렛지 콜라이더
	m_pColliderLedge->Set_Radius({ 0.3f , 2.0f, 0.3f });	
	m_pColliderLedge->Set_Dynamic(true);					
	m_pColliderLedge->Set_Trigger(true);					
	m_pColliderLedge->Set_CenterPos(tmpPos);
	m_pColliderLedge->Set_UnderPos();						
	m_pColliderLedge->SetUp_Box();				

	D3DXVECTOR3 tmpPos_Melee = { m_pTransform->GetPos().x + (m_pCollider->Get_Radius().x + tmpRight.x) , m_pTransform->GetPos().y, m_pTransform->GetPos().z + (m_pCollider->Get_Radius().z * tmpRight.z) };

	// 밀리 콜라이더
	m_pMeleeCollider->Set_Radius({ 3.5f , 3.f, 3.5f });
	m_pMeleeCollider->Set_Dynamic(true);
	m_pMeleeCollider->Set_Trigger(true);
	m_pMeleeCollider->Set_CenterPos(tmpPos_Melee);
	m_pMeleeCollider->Set_UnderPos();
	m_pMeleeCollider->SetUp_Box();
	m_pMeleeCollider->Set_Enabled(false);


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
	m_pCondition->Set_MoveSpeed(20.f);
	m_pCondition->Set_MoveAccel(1.f);
	m_pCondition->Set_Damage(15.f);
	
	
	m_fZoomAccel	= 0.f;
	m_fZoomSpeed	= 5;
	m_fMaxZoom		= 70;
	m_fMinZoom		= 20;
	m_iGrenadeCount = 99;
	m_iMaxGrenadeCount = 99;

	m_pWInfo.wWeaponDamage = 15;

	m_tCondition.fHp = m_pCondition->Get_Hp();
	m_tCondition.fArmor = m_pCondition->Get_Armor();
	m_pPlayerSubject->AddData(ENGINE::CPlayerSubject::PLAYER_INFO, &(m_tCondition));
	m_pWInfo.eWeaponTag = ENGINE::WEAPON_TAG::MELLE;
	m_pWInfo.wCurBullet = 0;
	m_pPlayerSubject->AddData(ENGINE::CPlayerSubject::WEAPON_INFO, &m_pWInfo);

	ENGINE::W_INFO* pTag = new ENGINE::W_INFO;

	memcpy(pTag, &m_pWInfo, sizeof(ENGINE::W_INFO));

	m_mWeaponInfo.insert(make_pair(ENGINE::MELLE, pTag));
	m_mWeaponInfo[m_pWInfo.eWeaponTag];

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

	for_each(m_mWeaponInfo.begin(), m_mWeaponInfo.end(),
		[](auto& MyPair)
	{
		Safe_Delete(MyPair.second);
	});

	m_mWeaponInfo.clear();
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
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_DYNAMIC, L"Buffer_Player");
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

	// Ledge Collider
	pComponent = ENGINE::CCollider::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Ledge_Collider", pComponent });

	m_pColliderLedge = dynamic_cast<ENGINE::CCollider*>(pComponent);
	NULL_CHECK_RETURN(m_pColliderLedge, E_FAIL);

	// Melee Collider
	pComponent = ENGINE::CCollider::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Melee_Collider", pComponent });

	m_pMeleeCollider = dynamic_cast<ENGINE::CCollider*>(pComponent);
	NULL_CHECK_RETURN(m_pMeleeCollider, E_FAIL);


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
	if (m_bIsLedge)
		return;

	float fMoveSpeed = m_pCondition->Get_MoveSpeed() * m_pCondition->Get_MoveAccel() * m_pCondition->Get_MoveAccel() * m_pTimeMgr->GetDelta();
	float fAngleSpeed = 90.f * m_pTimeMgr->GetDelta();

	ShootType();


	if (m_pKeyMgr->KeyDown(ENGINE::KEY_G))
	{
		if (m_iGrenadeCount <= 0)
			return;

		if (m_eActState != W_GRENADE)
		{
			CSoundMgr::GetInstance()->SetVolume(CSoundMgr::WEAPON_AFTER, 0.5f);
			CSoundMgr::GetInstance()->StopSound(CSoundMgr::WEAPON_AFTER);
			CSoundMgr::GetInstance()->MyPlaySound(L"Grenade_Throw.wav", CSoundMgr::WEAPON_AFTER);

			Grenade();
			m_eActState = W_GRENADE;
			m_bGrenade = true;
		}
	}

	if (m_pKeyMgr->KeyDown(ENGINE::KEY_LSHIFT))
	{
		m_pCondition->Set_Run(true);
		m_pCondition->Set_MoveSpeed(28.f);
	}

	else if (m_pKeyMgr->KeyUp(ENGINE::KEY_LSHIFT))
	{
		m_pCondition->Set_Run(false);
	
		m_pCondition->Set_MoveSpeed(20.f);
		D3DXVECTOR3 vTemp = { 0 , 0 , 0 };
		dynamic_cast<CCamera*>(m_pCamera)->Set_CamShakePos(vTemp);
	}

	if (m_pKeyMgr->KeyDown(ENGINE::KEY_LCTRL))
	{
		if (m_pCondition->Get_Run() == false || m_pCondition->Get_Slide())
			return;

		if (!m_bPlaySlideSound)
		{
			CSoundMgr::GetInstance()->SetVolume(CSoundMgr::PLAYER, 0.5f);
			CSoundMgr::GetInstance()->StopSound(CSoundMgr::PLAYER);
			CSoundMgr::GetInstance()->MyPlaySound(L"Player_Slide.mp3", CSoundMgr::PLAYER);

			m_bPlaySlideSound = true;
		}

		//dynamic_cast<CCamera*>(m_pCamera)->Set_CamYPos(-2);

		m_pCondition->Set_MoveAccel(2.5f);
		m_pCondition->Set_Slide(true);

		m_pCondition->Set_MoveSpeed(25.f);
		D3DXVECTOR3 vTemp = { 0 , 0 , 0 };
		dynamic_cast<CCamera*>(m_pCamera)->Set_CamShakePos(vTemp);

	}

	// 재장전
	if (m_pKeyMgr->KeyDown(ENGINE::KEY_R))
	{
		if (m_eWeaponState == ENGINE::SHOTGUN)
		{
			m_eActState = W_RELOAD;
			return;
		}

		if (m_eWeaponState == ENGINE::MELLE)
			return;

		if (m_pWInfo.wCurBullet <= 0)
			return;

		if (m_pWInfo.wMagazineBullet == m_pWInfo.wMagazineSize)
			return;

		if(m_eActState == W_IDLE || m_eActState == W_ZOOMOUT)
			Reload();
	}


	// 이동관련
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

	if (m_pKeyMgr->KeyPressing(ENGINE::KEY_W) || m_pKeyMgr->KeyPressing(ENGINE::KEY_A)||
		m_pKeyMgr->KeyPressing(ENGINE::KEY_S) || (m_pKeyMgr->KeyPressing(ENGINE::KEY_D)))
	{
		Check_WalkSound();
	}

	if (m_pKeyMgr->KeyPressing(ENGINE::KEY_LEFT))
	{
		m_pTransform->MoveAngle(ENGINE::ANGLE_Y, -fAngleSpeed);
	}

	if (m_pKeyMgr->KeyPressing(ENGINE::KEY_RIGHT))
	{
		m_pTransform->MoveAngle(ENGINE::ANGLE_Y, fAngleSpeed);
	}

	// 점프
	if (m_pKeyMgr->KeyDown(ENGINE::KEY_SPACE))
	{
		if (!m_bCanLedge && !m_bIsLedge)
		{
			if (m_iJumpCount >= 2)
				return;

			++m_iJumpCount;

			m_pRigid->Set_Accel({ 1, 1.5f, 1 });
			m_pRigid->Set_IsJump(true);
			m_pRigid->Set_IsGround(false);

			int a = rand() % 4;

			CSoundMgr::GetInstance()->SetVolume(CSoundMgr::PLAYER, 1.5f);
			CSoundMgr::GetInstance()->StopSound(CSoundMgr::PLAYER);

			switch (a)
			{
			case 0:
			{
				CSoundMgr::GetInstance()->MyPlaySound(L"Jump_01.mp3", CSoundMgr::PLAYER);
				break;
			}
			case 1:
			{
				CSoundMgr::GetInstance()->MyPlaySound(L"Jump_02.mp3", CSoundMgr::PLAYER);
				break;
			}
			case 2:
			{
				CSoundMgr::GetInstance()->MyPlaySound(L"Jump_03.mp3", CSoundMgr::PLAYER);
				break;
			}

			case 3:
			{
				CSoundMgr::GetInstance()->MyPlaySound(L"Jump_04.mp3", CSoundMgr::PLAYER);
				break;
			}
			}
		}

		if (m_bCanLedge && !m_bIsLedge)
		{
			m_eActState = W_LEDGE;

			int a = rand() % 2;

			CSoundMgr::GetInstance()->SetVolume(CSoundMgr::PLAYER, 1.5f);
			CSoundMgr::GetInstance()->StopSound(CSoundMgr::PLAYER);

			switch (a)
			{
			case 0:
			{
				CSoundMgr::GetInstance()->MyPlaySound(L"Ledge_01.mp3", CSoundMgr::PLAYER);
				break;
			}
			case 1:
			{
				CSoundMgr::GetInstance()->MyPlaySound(L"Ledge_02.mp3", CSoundMgr::PLAYER);
				break;
			}
			}
		}
	}

	if (m_bGrenade)
		return;

	Check_Weapon();
}

void CPlayer::Check_Weapon()
{
	if (m_pKeyMgr->KeyDown(ENGINE::KEY_1))
	{
		auto iter_find = m_mWeaponInfo.find(ENGINE::WEAPON_TAG::MELLE);

		if (m_mWeaponInfo.end() == iter_find)
			return;

		else
		{
			if (m_eWeaponState == iter_find->second->eWeaponTag)
			{
				cout << "Fist already selected" << endl;

				return;
			}

			else
			{
				if (m_eWeaponState != ENGINE::NO_WEAPON)
				{
					auto iter_find_old = m_mWeaponInfo.find(m_pWInfo.eWeaponTag);

					if (m_mWeaponInfo.end() != iter_find_old)
					{
						iter_find_old->second->wCurBullet = m_pWInfo.wCurBullet;
						iter_find_old->second->wMagazineBullet = m_pWInfo.wMagazineBullet;
					}
				}

				m_eActState = W_DRAW;

				cout << "Select Fist" << endl;


				m_eWeaponState = iter_find->second->eWeaponTag;
				memcpy(&m_pWInfo, iter_find->second, sizeof(ENGINE::W_INFO));
				m_bCanAttack = true;
			}
		}
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
				cout << "Revolver already selected" << endl;

				return;
			}

			else
			{
				if (m_eWeaponState != ENGINE::NO_WEAPON)
				{
					auto iter_find_old = m_mWeaponInfo.find(m_pWInfo.eWeaponTag);

					if (m_mWeaponInfo.end() != iter_find_old)
					{
						iter_find_old->second->wCurBullet = m_pWInfo.wCurBullet;
						iter_find_old->second->wMagazineBullet = m_pWInfo.wMagazineBullet;
					}
				}

				m_eActState = W_DRAW;

				cout << "Select Revolver" << endl;


				m_eWeaponState = iter_find->second->eWeaponTag;
				memcpy(&m_pWInfo, iter_find->second, sizeof(ENGINE::W_INFO));
				m_bCanAttack = true;
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
				cout << "SMG already selected" << endl;

				return;
			}

			else
			{
				if (m_eWeaponState != ENGINE::NO_WEAPON)
				{
					auto iter_find_old = m_mWeaponInfo.find(m_pWInfo.eWeaponTag);

					if (m_mWeaponInfo.end() != iter_find_old)
					{
						iter_find_old->second->wCurBullet = m_pWInfo.wCurBullet;
						iter_find_old->second->wMagazineBullet = m_pWInfo.wMagazineBullet;
					}
				}

				m_eActState = W_DRAW;

				cout << "Select SMG" << endl;

				m_eWeaponState = iter_find->second->eWeaponTag;
				memcpy(&m_pWInfo, iter_find->second, sizeof(ENGINE::W_INFO));
				m_bCanAttack = true;
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
				cout << "Pump_Shotgun already selected" << endl;

				return;
			}

			else
			{
				if (m_eWeaponState != ENGINE::NO_WEAPON)
				{
					auto iter_find_old = m_mWeaponInfo.find(m_pWInfo.eWeaponTag);

					if (m_mWeaponInfo.end() != iter_find_old)
					{
						iter_find_old->second->wCurBullet = m_pWInfo.wCurBullet;
						iter_find_old->second->wMagazineBullet = m_pWInfo.wMagazineBullet;
					}
				}

				cout << "Select Pump_Shotgun" << endl;

				m_eActState = W_DRAW;
				m_eWeaponState = iter_find->second->eWeaponTag;
				memcpy(&m_pWInfo, iter_find->second, sizeof(ENGINE::W_INFO));
				m_bCanAttack = true;
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
				cout << "Rocket_Luncher already selected" << endl;

				return;
			}

			else
			{
				if (m_eWeaponState != ENGINE::NO_WEAPON)
				{
					auto iter_find_old = m_mWeaponInfo.find(m_pWInfo.eWeaponTag);

					if (m_mWeaponInfo.end() != iter_find_old)
					{
						iter_find_old->second->wCurBullet = m_pWInfo.wCurBullet;
						iter_find_old->second->wMagazineBullet = m_pWInfo.wMagazineBullet;
					}
				}

				cout << "Select Rocket_Luncher" << endl;

				m_eActState = W_DRAW;
				m_eWeaponState = iter_find->second->eWeaponTag;
				memcpy(&m_pWInfo, iter_find->second, sizeof(ENGINE::W_INFO));
				m_bCanAttack = true;
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
			m_iJumpCount = 0;
			return;
		}

		D3DXVECTOR3 JumpLength = { 0, -m_pRigid->Set_Fall(m_pTransform->GetPos(), m_pTimeMgr->GetDelta()),0 };
		m_pTransform->Move_AdvancedPos_Vec3(JumpLength);
	}
}

void CPlayer::UpdateObserverData()
{
	// Update Observer 

	m_tCondition.fHp = m_pCondition->Get_Hp();
	m_tCondition.fArmor = m_pCondition->Get_Armor();
	m_pPlayerSubject->AddData(ENGINE::CPlayerSubject::PLAYER_INFO, &(m_tCondition));
	m_pPlayerSubject->AddData(ENGINE::CPlayerSubject::WEAPON_INFO, &m_pWInfo);
	m_pPlayerSubject->AddData(ENGINE::CPlayerSubject::GRENADE_COUNT, &m_iGrenadeCount);
}

void CPlayer::Shoot()
{
	if (m_pWInfo.fDelayTimer != 0)
		return;

	if (m_mWeaponInfo.empty())
		return;

	if (m_pWInfo.wMagazineBullet > 0)
	{
		if (m_bSpecial == false)
		{
			if (m_bZoom == false)
				m_eActState = W_FIRE;

			if (m_bZoom == true)
				m_eActState = W_ZOOMFIRE;
		}

		switch (m_pWInfo.eWeaponTag)
		{
		case ENGINE::WEAPON_TAG::MELLE:
		{
			break;
		}
		case ENGINE::WEAPON_TAG::LUNCHER:
		{
			int iSound = rand() % 3;

			CSoundMgr::GetInstance()->SetVolume(CSoundMgr::BULLET_SHOOT, 1.0f);
			CSoundMgr::GetInstance()->StopSound(CSoundMgr::BULLET_SHOOT);
			switch (iSound)
			{
			case 0:
				CSoundMgr::GetInstance()->MyPlaySound(L"RocketLuncher_Shoot_1.mp3", CSoundMgr::BULLET_SHOOT);
				break;
			case 1:
				CSoundMgr::GetInstance()->MyPlaySound(L"RocketLuncher_Shoot_2.mp3", CSoundMgr::BULLET_SHOOT);
				break;
			case 2:
				CSoundMgr::GetInstance()->MyPlaySound(L"RocketLuncher_Shoot_3.mp3", CSoundMgr::BULLET_SHOOT);
				break;
			}
			break;
		}
		case ENGINE::WEAPON_TAG::REVOLVER:
		case ENGINE::WEAPON_TAG::RIFLE:
		{
			CSoundMgr::GetInstance()->SetVolume(CSoundMgr::BULLET_SHOOT, 0.3f);

			int iSound = rand() % 4;

			switch (iSound)
			{
				CSoundMgr::GetInstance()->StopSound(CSoundMgr::BULLET_SHOOT);

			case 0:
				CSoundMgr::GetInstance()->MyPlaySound(L"Revolver_Shoot_1.mp3", CSoundMgr::BULLET_SHOOT);
				break;
			case 1:
				CSoundMgr::GetInstance()->MyPlaySound(L"Revolver_Shoot_2.mp3", CSoundMgr::BULLET_SHOOT);
				break;
			case 2:
				CSoundMgr::GetInstance()->MyPlaySound(L"Revolver_Shoot_3.mp3", CSoundMgr::BULLET_SHOOT);
				break;
			case 3:
				CSoundMgr::GetInstance()->MyPlaySound(L"Revolver_Shoot_4.mp3", CSoundMgr::BULLET_SHOOT);
				break;
			}
			break;
		}
		case ENGINE::WEAPON_TAG::SMG:
		{

			int iSound = rand() % 2;

			CSoundMgr::GetInstance()->SetVolume(CSoundMgr::BULLET_SHOOT, 1.0f);
			//CSoundMgr::GetInstance()->StopSound(CSoundMgr::BULLET_SHOOT);
			switch (iSound)
			{
			case 0:
				CSoundMgr::GetInstance()->MyPlaySound(L"SMG_Shoot_1.mp3", CSoundMgr::BULLET_SHOOT);
				break;
			case 1:
				CSoundMgr::GetInstance()->MyPlaySound(L"SMG_Shoot_2.mp3", CSoundMgr::BULLET_SHOOT);
				break;
			}
			break;
		}
		default:
			break;
		}


		m_pCondition->Set_RangeAttack(true);

		int xSpread = m_pWInfo.fSpread_X - (m_pWInfo.fSpread_X * 2);
		int ySpread = m_pWInfo.fSpread_Y - (m_pWInfo.fSpread_Y * 2);

		float xRand = rand() % xSpread * 0.01f;
		float yRand = rand() % ySpread * 0.01f;

		m_pWInfo.wMagazineBullet -= m_pWInfo.wUseBullet;
		m_pWInfo.fDelayTimer = m_pWInfo.fInterval;

		cout << "Remain bullet : " << m_pWInfo.wMagazineBullet << endl;

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

			// 탄 퍼짐 관련
			fAngle[0] = D3DXToDegree(acosf(tmpLook.y)) - 93 + xRand;
			fAngle[1] = m_pTransform->GetAngle(ENGINE::ANGLE_Y) + yRand;
			fAngle[2] = 0;

			CGameObject* pInstance = CBullet::Create(m_pGraphicDev, tmpPos, tmpLook, fAngle , m_pWInfo.fBullet_Speed , m_pWInfo.eWeaponTag, m_pWInfo.wWeaponDamage);
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

			CGameObject* pInstance = CBullet::Create(m_pGraphicDev, tmpPos, tmpDir, fAngle , m_pWInfo.fBullet_Speed, m_pWInfo.eWeaponTag, m_pWInfo.wWeaponDamage);
			m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::BULLET_PLAYER, pInstance);
		}
	}

	else if (m_pWInfo.wMagazineBullet <= 0)
	{
		CSoundMgr::GetInstance()->SetVolume(CSoundMgr::PLAYER, 0.5f);
		CSoundMgr::GetInstance()->StopSound(CSoundMgr::PLAYER);
		CSoundMgr::GetInstance()->MyPlaySound(L"Bullet_AmountZero.mp3", CSoundMgr::PLAYER);

		m_pCondition->Set_RangeAttack(false);

		if (m_bSpecial == false)
		{
			//if (m_bZoom == false)
			//	m_eActState = W_IDLE;

			if (m_bZoom == true)
				m_eActState = W_ZOOMOUT;
		}
	}
}

void CPlayer::Shoot_Shotgun()
{
	if (m_pWInfo.wMagazineBullet > 0)
	{
		if (m_bCanAttack == false)
			return;

		m_bCanAttack = false;

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

				// 탄 퍼짐 관련
				fAngle[0] = D3DXToDegree(acosf(tmpLook.y)) - 96 + xRand;
				fAngle[1] = m_pTransform->GetAngle(ENGINE::ANGLE_Y) - 6 + yRand;
				fAngle[2] = 0;

				CGameObject* pInstance = CBullet::Create(m_pGraphicDev, tmpPos, tmpLook, fAngle, m_pWInfo.fBullet_Speed , m_pWInfo.eWeaponTag , m_pWInfo.wWeaponDamage);
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

				CGameObject* pInstance = CBullet::Create(m_pGraphicDev, tmpPos, tmpDir, fAngle, m_pWInfo.fBullet_Speed , m_pWInfo.eWeaponTag, m_pWInfo.wWeaponDamage);
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::BULLET_PLAYER, pInstance);
				pInstance->Set_MapLayer(m_mapLayer);
			}
		}

		m_pWInfo.wMagazineBullet -= m_pWInfo.wUseBullet;
		m_pWInfo.fDelayTimer = m_pWInfo.fInterval;

		cout << "Remain bullet : " << m_pWInfo.wMagazineBullet << endl;

		CSoundMgr::GetInstance()->SetVolume(CSoundMgr::BULLET_SHOOT, 0.5f);

		int iSound = rand() % 5;

		switch (iSound)
		{
			CSoundMgr::GetInstance()->StopSound(CSoundMgr::BULLET_SHOOT);

		case 0:
			CSoundMgr::GetInstance()->MyPlaySound(L"Shotgun_Shoot_1.mp3", CSoundMgr::BULLET_SHOOT);
			break;
		case 1:
			CSoundMgr::GetInstance()->MyPlaySound(L"Shotgun_Shoot_2.mp3", CSoundMgr::BULLET_SHOOT);
			break;
		case 2:
			CSoundMgr::GetInstance()->MyPlaySound(L"Shotgun_Shoot_3.mp3", CSoundMgr::BULLET_SHOOT);
			break;
		case 3:
			CSoundMgr::GetInstance()->MyPlaySound(L"Shotgun_Shoot_4.mp3", CSoundMgr::BULLET_SHOOT);
			break;
		case 4:
			CSoundMgr::GetInstance()->MyPlaySound(L"Shotgun_Shoot_5.mp3", CSoundMgr::BULLET_SHOOT);
			break;
		}
	}

	if (m_pWInfo.wMagazineBullet <= 0)
	{
		m_pCondition->Set_RangeAttack(false);

		//if (m_bZoom == false)
		//	m_eActState = W_IDLE;

		if (m_bZoom == true)
			m_eActState = W_ZOOMOUT;

		CSoundMgr::GetInstance()->SetVolume(CSoundMgr::PLAYER, 0.5f);
		CSoundMgr::GetInstance()->StopSound(CSoundMgr::PLAYER);
		CSoundMgr::GetInstance()->MyPlaySound(L"Bullet_AmountZero.mp3", CSoundMgr::PLAYER);
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
	// 재장전
	cout << "Reloading" << endl;

	m_eActState = W_RELOAD;

	if (m_eWeaponState != ENGINE::SHOTGUN)
	{
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

	else if(m_eWeaponState == ENGINE::SHOTGUN)
	{
		if (m_pWInfo.wCurBullet > 0 && m_pWInfo.wMagazineBullet < m_pWInfo.wMagazineSize)
		{
			m_pWInfo.wCurBullet -= 1;
			m_pWInfo.wMagazineBullet += 1;
		}
	}

	//cout << "Remain Maxbullet : " << m_pWInfo.wCurBullet << endl;
}

void CPlayer::SpecialShot()
{
	if (m_pWInfo.wMagazineBullet > 0)
	{
		if (m_eActState == W_SPECIAL_READY)
			return;

		if (m_bSpecial)
		{
			m_eActState = W_SPECIAL_SHOT;
			Shoot();

			m_bSpecial = false;
			m_pWInfo.fDelayTimer *= 0.5f;
		}
	}

	else if (m_pWInfo.wMagazineBullet <= 0)
	{
		m_eActState = W_SPECIAL_END;
		m_bSpecial = false;
	}
}

void CPlayer::Grenade()
{
	if (m_iGrenadeCount <= 0)
		return;

	--m_iGrenadeCount;

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

		// 탄 퍼짐 관련
		fAngle[0] = D3DXToDegree(acosf(tmpLook.y)) - 90;
		fAngle[1] = m_pTransform->GetAngle(ENGINE::ANGLE_Y);
		fAngle[2] = 0;

		CGameObject* pInstance = CGrenade::Create(m_pGraphicDev, tmpPos, tmpLook, fAngle, 60, m_pWInfo.eWeaponTag, 10);
		m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::GRENADE, pInstance);
		pInstance->Set_MapLayer(m_mapLayer);
	}
}

void CPlayer::WeaponActState()
{
	switch (m_eActState)
	{
	case W_IDLE:
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
			m_pCollider->Set_MaxY(-2);

			float tmpX = m_pCondition->Get_MoveAccel() - m_pTimeMgr->GetDelta() * 3;
			m_pCondition->Set_MoveAccel(tmpX);

			if (tmpX * tmpX - 6 > -2.f)
				dynamic_cast<CCamera*>(m_pCamera)->Set_CamYPos(tmpX * tmpX - 6);

			else
				dynamic_cast<CCamera*>(m_pCamera)->Set_CamYPos(-3);
		}

		else if (m_pCondition->Get_MoveAccel() <= 0)
		{
			//m_pCondition->Set_MoveAccel(0.f);
			m_pCondition->Set_MoveSpeed(20.f);

			m_fSlideUp += m_pTimeMgr->GetDelta() * m_pTimeMgr->GetDelta() * 400 + 0.1f;

			if (m_fSlideUp < 2.f)
				dynamic_cast<CCamera*>(m_pCamera)->Set_CamYPos(-3 + m_fSlideUp);

			else if (m_fSlideUp >= 2.f)
			{
				m_pCollider->Set_MaxY(0);

				dynamic_cast<CCamera*>(m_pCamera)->Set_CamYPos(0.f);
				m_pCondition->Set_Slide(false);
				m_pCondition->Set_MoveAccel(1.f);
				m_pCondition->Set_MoveSpeed(20.f);
				m_fSlideUp = 0.f;

				if (m_pCondition->Get_Run())
				{
					m_pCondition->Set_MoveSpeed(28.f);
				}
			}
		}
	}
	else
		m_bPlaySlideSound = false;
}

void CPlayer::Check_Run()
{
	if (m_pCondition->Get_Run())
	{
		if (m_bIsLedge)
		{
			D3DXVECTOR3 vTemp = { 0 , 0 , 0 };
			dynamic_cast<CCamera*>(m_pCamera)->Set_CamShakePos(vTemp);
		}

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

		Check_WalkSound(true);
		
	}
}

void CPlayer::Check_Ledge()
{
	if (m_eActState == W_LEDGE)
	{
		if (m_bIsLedge == false)
		{
			dynamic_cast<CCamera*>(m_pCamera)->Set_Hotizontal(-35);


		//	cout << "매달리지 아낫스" << endl;


			float fLength_x = m_pTransform->GetPos().x - m_vLedgeVec.x;
			float fLength_z = m_pTransform->GetPos().z - m_vLedgeVec.z;

			(fLength_x > fLength_z ? fLength_x = 0 : fLength_z = 0);

			D3DXVECTOR3 tmpPos1 = { m_pTransform->GetPos().x + fLength_x * 0.95f , m_vLedgeVec.y - 1.5f, m_pTransform->GetPos().z + fLength_z * 0.95f };
			D3DXVECTOR3 tmpPos2 = { m_pTransform->GetPos().x + fLength_x * 0.95f , m_vLedgeVec.y + m_pCollider->Get_Radius().y + 0.2f , m_pTransform->GetPos().z + fLength_z * 0.95f };

			m_pRigid->Set_UseGravity(false);

			m_pTransform->SetPos(tmpPos1);
			m_pCollider->LateUpdate(m_pTransform->GetPos());
			m_pGroundChekCollider->LateUpdate({ m_pTransform->GetPos().x ,
				m_pTransform->GetPos().y - m_pCollider->Get_Radius().y,
				m_pTransform->GetPos().z });

			D3DXVECTOR3 tmpDir = m_pTransform->GetDir();
			D3DXVECTOR3 tmpPos3 = { m_pTransform->GetPos().x + tmpDir.x , m_pTransform->GetPos().y + 1, m_pTransform->GetPos().z + tmpDir.z };

			m_pColliderLedge->LateUpdate(tmpPos3);

			m_vLedgeUpVec = tmpPos2;
			m_fLength_Y = m_pTransform->GetPos().y - tmpPos2.y;
			m_bIsLedge = true;
		}

		if (m_bIsLedge)
		{
			if (m_pTransform->GetPos().y < m_vLedgeUpVec.y)
			{
				float yLength = (m_vLedgeUpVec.y - m_pTransform->GetPos().y);
				D3DXVECTOR3 tmpDir = { 0,1,0 };
				//D3DXVec3Normalize(&tmpDir, &tmpDir);
				D3DXVECTOR3 tmpY = {};

				m_pTransform->Move_AdvancedPos(tmpDir, 10 * m_pTimeMgr->GetDelta());

				m_pCollider->LateUpdate(m_pTransform->GetPos());
				m_pGroundChekCollider->LateUpdate({ m_pTransform->GetPos().x ,
					m_pTransform->GetPos().y - m_pCollider->Get_Radius().y,
					m_pTransform->GetPos().z });

				D3DXVECTOR3 tmpDir2 = m_pTransform->GetDir();
				D3DXVECTOR3 tmpPos3 = { m_pTransform->GetPos().x + tmpDir2.x , m_pTransform->GetPos().y + 1, m_pTransform->GetPos().z + tmpDir2.z };

				m_pColliderLedge->LateUpdate(tmpPos3);
			}

			else if (m_pTransform->GetPos().y >= m_vLedgeUpVec.y + 1)
			{
				dynamic_cast<CCamera*>(m_pCamera)->Set_Hotizontal(0);

				D3DXVECTOR3 tmpDir = { m_pTransform->GetDir().x , 0 ,m_pTransform->GetDir().z };
				D3DXVECTOR3 tmpY = {};

				m_pTransform->Move_AdvancedPos(tmpDir, 10 * m_pTimeMgr->GetDelta());
				m_bCanLedge = false;
				m_bIsLedge = false;
				m_pRigid->Set_UseGravity(true);
			}
		}
	}

	else
	{

		m_bCanLedge = false;
		m_bIsLedge = false;
		m_pRigid->Set_UseGravity(true);
	}
}


void CPlayer::Check_Grenade()
{
	if (m_iGrenadeCount > m_iMaxGrenadeCount)
	{
		m_iGrenadeCount = m_iMaxGrenadeCount;
	}
}

void CPlayer::Check_Hitted()
{
	if (m_pRigid->Get_IsPushForUI())
	{
		CHittedCircle* pCircle = CHittedCircle::Create(m_pGraphicDev, CHittedCircle::SIZE_XL);

		D3DXVECTOR3 tmpPos_0 = m_pRigid->Get_PushDirForUI();
		D3DXVECTOR3 tmpDir = tmpPos_0 - m_pTransform->GetPos();
		D3DXVECTOR3 tmpRight = { 1,0,0 };

		D3DXVec3Normalize(&tmpDir, &tmpDir);
		D3DXVec3Cross(&tmpRight, &tmpDir, &D3DXVECTOR3{ 0, 1, 0 });

		float fAngle = 0.f;
		//D3DXVECTOR3 vPlayerDir = {0, 0, 0};
		//D3DXVec3Normalize(&vPlayerDir, &m_pTransform->GetDir());
		//fAngle = D3DXVec3Dot(&m_pRigid->Get_PushDirForUI(), &D3DXVECTOR3(0, 0, -1));
		fAngle = D3DXVec3Dot(&m_pRigid->Get_PushDirForUI(), &tmpRight);

		if (m_pTransform->GetDir().x < 0)
		{
			fAngle *= -1.f;
			fAngle += D3DXToRadian(180.f);
		}

		static_cast<CHittedCircle*>(pCircle)->SetAngle(fAngle);
		m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::UI, pCircle);

		m_pRigid->Set_IsPushForUI(false);
	}
}

void CPlayer::Check_WalkSound(bool _bIsRun)
{
	if (!m_pRigid->Get_IsGround())
		return;

	float fDelay = 0.f;
	if (_bIsRun)
		fDelay = 0.45f;
	else
		fDelay = 0.5f;

	if (m_fWalkSoundDelay >= fDelay)
	{
		m_fWalkSoundDelay = 0.f;

		CSoundMgr::GetInstance()->SetVolume(CSoundMgr::PLAYER_WALK, 0.5f);
		CSoundMgr::GetInstance()->StopSound(CSoundMgr::PLAYER_WALK);

		if (m_iWalkSoundIndex == 0)
			CSoundMgr::GetInstance()->MyPlaySound(L"PlayerWalk_1.wav", CSoundMgr::PLAYER_WALK);
		else if (m_iWalkSoundIndex == 1)
			CSoundMgr::GetInstance()->MyPlaySound(L"PlayerWalk_2.wav", CSoundMgr::PLAYER_WALK);
		else if (m_iWalkSoundIndex == 2)
			CSoundMgr::GetInstance()->MyPlaySound(L"PlayerWalk_3.wav", CSoundMgr::PLAYER_WALK);
		else if (m_iWalkSoundIndex == 3)
		{
			CSoundMgr::GetInstance()->MyPlaySound(L"PlayerWalk_4.wav", CSoundMgr::PLAYER_WALK);
			m_iWalkSoundIndex = -1;
		}

		m_iWalkSoundIndex++;
	}
	else
	{
		m_fWalkSoundDelay += m_pTimeMgr->GetDelta();
	}
}

void CPlayer::Check_Dead()
{
	if (m_pTransform->GetPos().y <= -70)
	{
		m_bIsDead = true;
	}
}

void CPlayer::ShootType()
{
	if (m_eActState == W_RELOAD)
		return;

	switch (m_eWeaponState)
	{
	case ENGINE::MELLE:
	{
		if (m_eActState != W_IDLE)
			return;

			if (m_pKeyMgr->KeyDown(ENGINE::KEY_LBUTTON))
			{
				m_eActState = W_L_FIST;
				//m_bCanAttack = false;

				int iSound = rand() % 3;

				CSoundMgr::GetInstance()->SetVolume(CSoundMgr::WEAPON, 1.0f);
				CSoundMgr::GetInstance()->StopSound(CSoundMgr::WEAPON);
				switch (iSound)
				{
				case 0:
					CSoundMgr::GetInstance()->MyPlaySound(L"Fist_01.mp3", CSoundMgr::WEAPON);
					break;
				case 1:
					CSoundMgr::GetInstance()->MyPlaySound(L"Fist_02.mp3", CSoundMgr::WEAPON);
					break;
				case 2:
					CSoundMgr::GetInstance()->MyPlaySound(L"Fist_03.mp3", CSoundMgr::WEAPON);
					break;
				}
			}

			else if (m_pKeyMgr->KeyDown(ENGINE::KEY_RBUTTON))
			{
				m_eActState = W_R_FIST;
				//m_bCanAttack = false;

				int iSound = rand() % 3;

				CSoundMgr::GetInstance()->SetVolume(CSoundMgr::WEAPON, 1.0f);
				CSoundMgr::GetInstance()->StopSound(CSoundMgr::WEAPON);
				switch (iSound)
				{
				case 0:
					CSoundMgr::GetInstance()->MyPlaySound(L"Fist_01.mp3", CSoundMgr::WEAPON);
					break;
				case 1:
					CSoundMgr::GetInstance()->MyPlaySound(L"Fist_02.mp3", CSoundMgr::WEAPON);
					break;
				case 2:
					CSoundMgr::GetInstance()->MyPlaySound(L"Fist_03.mp3", CSoundMgr::WEAPON);
					break;
				}

			}
		//}
	}
	case ENGINE::REVOLVER:
	{
		if (m_pKeyMgr->KeyDown(ENGINE::KEY_LBUTTON))
		{
			Shoot();
		}

		if (m_pKeyMgr->KeyDown(ENGINE::KEY_RBUTTON))
		{
			m_bSpecial = true;
			m_eActState = W_SPECIAL_READY;
		}

		if (m_bSpecial)
		{
			SpecialShot();
		}

		break;
	}

	case ENGINE::LUNCHER:
	{
		Zoom();

		if (m_pKeyMgr->KeyDown(ENGINE::KEY_LBUTTON))
		{
			Shoot();
		}

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
				m_eActState = W_IDLE;

			m_pCondition->Set_RangeAttack(false);
		}

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
}

void CPlayer::Set_WeaponInfo(ENGINE::WEAPON_TAG _eTag, ENGINE::W_INFO * _WeaponInfo)
{
	m_mWeaponInfo[_eTag] = _WeaponInfo;
}

void CPlayer::Set_MeleeTrigger(bool _bool)
{
	m_pMeleeCollider->Set_Enabled(_bool);
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
