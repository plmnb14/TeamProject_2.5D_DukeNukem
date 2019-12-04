#include "stdafx.h"
#include "Bullet.h"
#include "Camera.h"
#include "Trasform.h"
#include "Camera_Component.h"
#include "RigidBody.h"
#include "Collider.h"
#include "Billborad.h"
#include "Condition.h"
#include "CameraObserver.h"
#include "Effect_BulletHit.h"
#include "Effect_BulletHole.h"
#include "Effect_BloodSplit.h"
#include "Effect_RocketSmoke.h"
#include "Effect_Explosion_Rocket.h"
#include "Effect_Fireball.h"
#include "SoundMgr.h"
#include "Animator.h"


CBullet::CBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pKeyMgr(ENGINE::GetKeyMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr),
	m_pCollider(nullptr), m_pRigid(nullptr),m_pBillborad(nullptr), m_pObserver(nullptr),
	m_eWeaponTag(ENGINE::WEAPON_TAG::MELLE), m_pSubject(ENGINE::GetCameraSubject()),
	m_pCondition(nullptr)

{
}

CBullet::~CBullet()
{
	Release();
}

int CBullet::Update()
{
	if (m_bIsDead)
	{
		if (m_pCollider->Get_IsCollision())
		{
			CGameObject* pInstance = nullptr;

			if (m_eTag == ENGINE::WOOD_BOX)
			{
				int iSound = rand() % 3;

				CSoundMgr::GetInstance()->SetVolume(CSoundMgr::WOOD_BOX, 5.f);
				CSoundMgr::GetInstance()->StopSound(CSoundMgr::WOOD_BOX);
				switch (iSound)
				{
				case 0:
					CSoundMgr::GetInstance()->MyPlaySound(L"WoodBox_Hit_1.ogg", CSoundMgr::WOOD_BOX);
					break;
				case 1:
					CSoundMgr::GetInstance()->MyPlaySound(L"WoodBox_Hit_2.ogg", CSoundMgr::WOOD_BOX);
					break;
				case 2:
					CSoundMgr::GetInstance()->MyPlaySound(L"WoodBox_Hit_3.ogg", CSoundMgr::WOOD_BOX);
					break;
				}
			}

			if (m_eTag == ENGINE::TERRAIN)
			{
				CSoundMgr::GetInstance()->SetVolume(CSoundMgr::BULLET_DEAD, 1.0f);

				if(m_eWeaponTag != ENGINE::MONSTER_REVOLVER)
					CSoundMgr::GetInstance()->StopSound(CSoundMgr::BULLET_DEAD);

				int iSound = rand() % 2;

				if (m_eWeaponTag == ENGINE::REVOLVER)
					iSound = rand() % 3;

				switch (iSound)
				{
				case 0:
					CSoundMgr::GetInstance()->MyPlaySound(L"Bullet_Hit_4.mp3", CSoundMgr::BULLET_DEAD);
					break;
				case 1:
					CSoundMgr::GetInstance()->MyPlaySound(L"Bullet_Hit_5.mp3", CSoundMgr::BULLET_DEAD);
					break;
				case 2:
					CSoundMgr::GetInstance()->MyPlaySound(L"Bullet_Hit_3.mp3", CSoundMgr::BULLET_DEAD);
					break;
				}

				pInstance = CEffect_BulletHole::Create(m_pGraphicDev, m_pCollider->Get_CenterPos());
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pInstance);
			}

			if (m_eTag == ENGINE::MONSTER)
			{
				pInstance = CEffect_BloodSplit::Create(m_pGraphicDev, m_pCollider->Get_CenterPos());
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pInstance);

				CSoundMgr::GetInstance()->SetVolume(CSoundMgr::MONSTER, 1.0f);
				CSoundMgr::GetInstance()->StopSound(CSoundMgr::MONSTER);
				int iSound = rand() % 3;

				switch (iSound)
				{
				case 0:
					CSoundMgr::GetInstance()->MyPlaySound(L"Monster_Hitted_1.mp3", CSoundMgr::MONSTER);
					break;
				case 1:
					CSoundMgr::GetInstance()->MyPlaySound(L"Monster_Hitted_2.mp3", CSoundMgr::MONSTER);
					break;
				case 2:
					CSoundMgr::GetInstance()->MyPlaySound(L"Monster_Hitted_3.mp3", CSoundMgr::MONSTER);
					break;
				}
			}

			if (m_eTag == ENGINE::PLAYER)
			{
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

				CSoundMgr::GetInstance()->SetVolume(CSoundMgr::PLAYER, 1.0f);
				CSoundMgr::GetInstance()->StopSound(CSoundMgr::PLAYER);
				CSoundMgr::GetInstance()->MyPlaySound(L"Player_Hitted_BloodSound_1.mp3", CSoundMgr::PLAYER);
								
			}

			if (m_eWeaponTag == ENGINE::LUNCHER)
			{
				D3DXVECTOR3 vTempDir = -m_pTransform->GetDir();
				D3DXVECTOR3 vTempPos = { m_pCollider->Get_CenterPos().x + vTempDir.x * 10 ,
										m_pCollider->Get_CenterPos().y + vTempDir.y * 10 ,
										m_pCollider->Get_CenterPos().z + vTempDir.z * 10 };

				pInstance = CEffect_Explosion_Rocket::Create(m_pGraphicDev, vTempPos, m_pCondition->Get_Damage() * 0.8f );
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::EXPLOSION, pInstance);

				for (int i = 0; i < 12; ++i)
				{
					int Angle = rand() % 30;

					pInstance = CEffect_Fireball::Create(m_pGraphicDev, vTempPos , Angle * ( i + 1 ) );
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX_HIT, pInstance);
					pInstance->Set_MapLayer(m_mapLayer);
				}
				CSoundMgr::GetInstance()->SetVolume(CSoundMgr::BULLET_DEAD, 1.0f);
				CSoundMgr::GetInstance()->StopSound(CSoundMgr::BULLET_DEAD);
				CSoundMgr::GetInstance()->MyPlaySound(L"RocketBullet_Hit_1.mp3", CSoundMgr::BULLET_DEAD);
				
			}

			else
			{
				pInstance = CEffect_BulletHit::Create(m_pGraphicDev, m_pCollider->Get_CenterPos());
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pInstance);
			}
		}

		return DEAD_OBJ;
	}

	m_pSubject = ENGINE::GetCameraSubject();

	m_pCollider->Set_OldPos(m_pTransform->GetPos());
	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();
	BulletType();
	
	KeyInput();

	if (m_eWeaponTag == ENGINE::LUNCHER || m_eWeaponTag == ENGINE::MONSTER_LUNCHER)
	{
		m_fSpeed += m_pTimeMgr->GetDelta() * 70 * m_pTimeMgr->GetDelta() * 70;

		CGameObject* pInstance = CEffect_RocketSmoke::Create(m_pGraphicDev, m_pCollider->Get_CenterPos());
		m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pInstance);
	}

	return NO_EVENT;
}

void CBullet::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();

	D3DXMatrixIdentity(&m_matView);

	D3DXMATRIX Localmatrix, Cameramatrix;													  //  로컬, 카메라 행렬 
	D3DXVECTOR3 vSize;																		  // 대상의 사이즈 
	Localmatrix = m_pTransform->GetWorldMatrix();
	Cameramatrix = m_pObserver->GetViewMatrix();
	
	vSize = m_pTransform->GetSize();


	m_pBillborad->Billborad_Front(Localmatrix, Cameramatrix, vSize);                          // 빌보드 설정
	m_matView = m_pBillborad->GetWorldMatrix_Billborad();


	m_pCollider->LateUpdate(m_pTransform->GetPos());
	m_fLifetime -= m_pTimeMgr->GetDelta();

	if (m_fLifetime < 0)
		m_bIsDead = true;
}

void CBullet::Render()
{
	

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matView);
	m_pTexture->Render(0);
	if (m_eWeaponTag == ENGINE::MONSTER_WAVE)
	{
		m_pAnimator->RenderSet(m_pTimeMgr->GetDelta());
		m_pTexture->Render(m_pAnimator->Get_Frame());
	}
	m_pBuffer->Render();
	


}

HRESULT CBullet::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(0.25f, 0.25f, 0.25f));

	// 물리적 콜라이더
	m_pCollider->Set_Radius({ 0.5f , 0.5f, 0.5f });			// 각 축에 해당하는 반지름을 설정
	m_pCollider->Set_Dynamic(true);							// 동적, 정적 Collider 유무
	m_pCollider->Set_Trigger(true);						// 트리거 유무
	m_pCollider->Set_CenterPos(m_pTransform->GetPos());		// Collider 의 정중앙좌표
	m_pCollider->Set_UnderPos();							// Collider 의 하단중앙 좌표
	m_pCollider->SetUp_Box();								// 설정된 것들을 Collider 에 반영합니다.


	// 리지드 바디 세팅
	m_pRigid->Set_UseGravity(true);							// 중력의 영향 유무

	m_pRigid->Set_IsGround(false);							// 지상인지 체크
	m_pRigid->Set_IsAir(true);								// 공중인지 체크
	m_pRigid->Set_IsFall(true);								// 낙하중인지 체크
	m_pRigid->Set_IsJump(false);

	m_pRigid->Set_fMass(1.f);								// 물체의 무게
	m_pRigid->Set_fPower(5.f);								// 점프 파워

	m_pRigid->Set_Speed({ 1.f , 1.f , 1.f });				// 각 축에 해당하는 속도
	m_pRigid->Set_Accel({ 0.f, -1.f, 0.f });					// 각 축에 해당하는 Accel 값
	m_pRigid->Set_MaxAccel({ 1.f , 1.f , 1.f });			// 각 축에 해당하는 MaxAccel 값

	m_fLifetime = 2.f;

	m_pAnimator->Set_FrameAmp(1.f);									// 배속재생
	m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_ZERO);
	m_pAnimator->Set_Reverse(false);
	m_pAnimator->Set_MaxFrame(0);
	m_pAnimator->Stop_Animation(false);

	return S_OK;
}

HRESULT CBullet::LateInit()
{
	m_pObserver = CCameraObserver::Create();
	NULL_CHECK_RETURN(m_pObserver, E_FAIL);

	m_pSubject->Subscribe(m_pObserver);

	return S_OK;
}

void CBullet::Release()
{
	m_pSubject->UnSubscribe(m_pObserver);
	ENGINE::Safe_Delete(m_pObserver);
}

HRESULT CBullet::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	// Texture
		pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, L"Bullet_Yellow");
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

	// Rigid
	pComponent = ENGINE::CRigidBody::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"RigidBody", pComponent });

	m_pRigid = dynamic_cast<ENGINE::CRigidBody*>(pComponent);
	NULL_CHECK_RETURN(m_pRigid, E_FAIL);

	//빌보드 
	pComponent = ENGINE::CBillborad::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_pBillborad = dynamic_cast<ENGINE::CBillborad*>(pComponent);
	NULL_CHECK_RETURN(m_pBillborad, E_FAIL);
	m_mapComponent.insert({ L"BillBoard", pComponent });

	// conditoin  
	pComponent = ENGINE::CCondition::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Condition", pComponent });

	m_pCondition = static_cast<ENGINE::CCondition*>(pComponent);
	NULL_CHECK_RETURN(m_pCondition, E_FAIL);
	
	//animator
	pComponent = ENGINE::CAnimator::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Animator", pComponent });

	m_pAnimator = static_cast<ENGINE::CAnimator*>(pComponent);
	NULL_CHECK_RETURN(m_pAnimator, E_FAIL);




	return S_OK;
}

void CBullet::KeyInput()
{
	
}

void CBullet::BulletType()
{
	switch (m_eWeaponTag)
	{
	case ENGINE::RIFLE:
	case ENGINE::REVOLVER:
	case ENGINE::SHOTGUN:
	case ENGINE::SMG:
	{
		m_pTransform->MovePos(m_fSpeed * m_pTimeMgr->GetDelta());
		break;
	}
	case ENGINE::LUNCHER:
	{
		m_pTransform->MovePos(m_fSpeed * m_pTimeMgr->GetDelta());

		D3DXVECTOR3 JumpLength = { 0, m_pRigid->Set_Fall(m_pTransform->GetPos(), m_pTimeMgr->GetDelta()) * 5, 0 };
		m_pTransform->Move_AdvancedPos_Vec3(JumpLength);

		break;
	}
	case ENGINE::MONSTER_LUNCHER:
	{
		m_pTransform->Move_AdvancedPos(m_dir, m_fSpeed * m_pTimeMgr->GetDelta());

		D3DXVECTOR3 JumpLength = { 0, m_pRigid->Set_Fall(m_pTransform->GetPos(), m_pTimeMgr->GetDelta()) * 5, 0 };
		m_pTransform->Move_AdvancedPos_Vec3(JumpLength);
	}

	case ENGINE::MONSTER_REVOLVER:
	{
		m_pTransform->Move_AdvancedPos(m_dir,m_fSpeed * m_pTimeMgr->GetDelta());
		break;
	}
	case ENGINE::MONSTER_WAVE:
	{
		
		m_pTransform->SetSize({ 3.f,3.f,3.f });
		m_pCollider->Set_Radius({ 3.f,3.f,3.f });
		ChangeTex(L"Monster_Bullet");
		m_pAnimator->Set_FrameAmp(2.f);
		m_pTransform->Move_AdvancedPos(m_dir, m_fSpeed * m_pTimeMgr->GetDelta());
		
		break;
	}
	}
}

void CBullet::ChangeTex(wstring _wstrTex)
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

void CBullet::Set_Target(CGameObject * _Target)
{
	m_pTarget = _Target;
}

void CBullet::Set_Device(LPDIRECT3DDEVICE9 pGraphicDev)
{
	m_pGraphicDev = pGraphicDev;
}

void CBullet::Set_Pos(D3DXVECTOR3 _Pos)
{
	m_pTransform->SetPos(_Pos);
}

void CBullet::Set_Dir(D3DXVECTOR3 _Dir)
{
	m_pTransform->SetDir(_Dir);
}

void CBullet::Set_Angle(float * _Angle)
{
	m_pTransform->SetAngle(_Angle[0], ENGINE::ANGLE_X);
	m_pTransform->SetAngle(_Angle[1], ENGINE::ANGLE_Y);
	m_pTransform->SetAngle(_Angle[2], ENGINE::ANGLE_Z);
}

void CBullet::Set_Attack(float _Attack)
{
	m_pCondition->Set_Damage(_Attack);
}

CBullet* CBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos, D3DXVECTOR3 _Dir, float* _Angle, float _Speed , ENGINE::WEAPON_TAG _WeaponTag ,float _Attack)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CBullet* pInstance = new CBullet(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	pInstance->Set_Pos(_Pos);
	pInstance->Set_Dir(_Dir);
	pInstance->m_dir = _Dir;
	pInstance->Set_Angle(_Angle);
	pInstance->Set_Speed(_Speed);
	pInstance->Set_WeaponTag(_WeaponTag);
	pInstance->Set_Attack(_Attack);

	return pInstance;
}
