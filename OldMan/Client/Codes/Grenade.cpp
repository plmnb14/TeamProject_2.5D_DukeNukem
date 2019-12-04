#include "stdafx.h"
#include "Grenade.h"
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
#include "Effect_Guide.h"
#include "SoundMgr.h"


CGrenade::CGrenade(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr),
	m_pCollider(nullptr), m_pRigid(nullptr), m_pBillborad(nullptr), m_pObserver(nullptr),
	m_eWeaponTag(ENGINE::WEAPON_TAG::MELLE), m_pSubject(ENGINE::GetCameraSubject()),
	m_pCondition(nullptr), m_fOrigin_YAngle(0)

{
}

CGrenade::~CGrenade()
{
	Release();
}

int CGrenade::Update()
{
	if (m_bIsDead)
	{
		CGameObject* pInstance = CEffect_Explosion_Rocket::Create(m_pGraphicDev, m_pTransform->GetPos(), 30);
		m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::EXPLOSION, pInstance);

		int a = rand() % 3;

		CSoundMgr::GetInstance()->SetVolume(CSoundMgr::EXPLOSION_GRENADE, 0.5f);
		CSoundMgr::GetInstance()->StopSound(CSoundMgr::EXPLOSION_GRENADE);

		switch (a)
		{
		case 0:
		{
			CSoundMgr::GetInstance()->MyPlaySound(L"EXPL_10.ogg", CSoundMgr::EXPLOSION_GRENADE);
			break;
		}
		case 1:
		{
			CSoundMgr::GetInstance()->MyPlaySound(L"EXPL_11.ogg", CSoundMgr::EXPLOSION_GRENADE);
			break;
		}
		case 2:
		{
			CSoundMgr::GetInstance()->MyPlaySound(L"EXPL_13.ogg", CSoundMgr::EXPLOSION_GRENADE);
			break;
		}
		}

		return DEAD_OBJ;
	}

	m_pSubject = ENGINE::GetCameraSubject();

	m_pCollider->Set_OldPos(m_pTransform->GetPos());
	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();

	KeyInput();

	m_pTransform->MovePos(m_fSpeed * m_pTimeMgr->GetDelta());

	if (m_pTransform->GetAngle(ENGINE::ANGLE_X) <= 90 && m_pTransform->GetAngle(ENGINE::ANGLE_X) >= -90)
	{
		m_pTransform->SetAngle(m_pTransform->GetAngle(ENGINE::ANGLE_X) + m_pTimeMgr->GetDelta() * fabs(m_fOrigin_YAngle) * 4.f, ENGINE::ANGLE_X);
	}

	else
	{
		m_pTransform->SetAngle(m_pTransform->GetAngle(ENGINE::ANGLE_X) - m_pTimeMgr->GetDelta() * fabs(m_fOrigin_YAngle) * 4.f, ENGINE::ANGLE_X);
	}

	CGameObject* pInstance = CEffect_Guide::Create(m_pGraphicDev, m_pCollider->Get_CenterPos(), {0.3f,0.3f,0.3f});
	m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pInstance);

	return NO_EVENT;
}

void CGrenade::LateUpdate()
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

	if (m_pCollider->Get_Length().x != 0 || m_pCollider->Get_Length().y != 0 || m_pCollider->Get_Length().z != 0)
	{
		if (m_pCollider->Get_Length().x != 0 || m_pCollider->Get_Length().z != 0)
		{
			if (m_pTransform->GetAngle(ENGINE::ANGLE_X) < 0)
			{
				m_pTransform->SetAngle(-180 - m_pTransform->GetAngle(ENGINE::ANGLE_X), ENGINE::ANGLE_X);
			}

			else if (m_pTransform->GetAngle(ENGINE::ANGLE_X) >= 0)
			{
				m_pTransform->SetAngle(180 - m_pTransform->GetAngle(ENGINE::ANGLE_X), ENGINE::ANGLE_X);
			}
		}
		
		else
			m_pTransform->SetAngle(-m_pTransform->GetAngle(ENGINE::ANGLE_X), ENGINE::ANGLE_X);

		m_fSpeed *= 0.7f;
		m_fOrigin_YAngle = m_pTransform->GetAngle(ENGINE::ANGLE_X);

		m_pCollider->Set_Length({ 0,0,0 });
	}
}

void CGrenade::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matView);
	m_pTexture->Render(0);
	m_pBuffer->Render();
}

HRESULT CGrenade::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));

	// 물리적 콜라이더
	m_pCollider->Set_Radius({ 0.5f , 0.5f, 0.5f });			// 각 축에 해당하는 반지름을 설정
	m_pCollider->Set_Dynamic(true);							// 동적, 정적 Collider 유무
	m_pCollider->Set_Trigger(false);						// 트리거 유무
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

	m_pRigid->Set_Speed({ 1.f , 1.2f , 1.f });				// 각 축에 해당하는 속도
	m_pRigid->Set_Accel({ 0.f, -1.2f, 0.f });					// 각 축에 해당하는 Accel 값
	m_pRigid->Set_MaxAccel({ 1.f , 1.2f , 1.f });			// 각 축에 해당하는 MaxAccel 값

	m_fLifetime = 2.f;

	return S_OK;
}

HRESULT CGrenade::LateInit()
{
	m_pObserver = CCameraObserver::Create();
	NULL_CHECK_RETURN(m_pObserver, E_FAIL);

	m_pSubject->Subscribe(m_pObserver);

	return S_OK;
}

void CGrenade::Release()
{
	m_pSubject->UnSubscribe(m_pObserver);
	ENGINE::Safe_Delete(m_pObserver);
}

HRESULT CGrenade::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	// Texture
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, L"Bullet_Grenade");
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

	return S_OK;
}

void CGrenade::KeyInput()
{

}

void CGrenade::BulletType()
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
		m_pTransform->Move_AdvancedPos(m_dir, m_fSpeed * m_pTimeMgr->GetDelta());
		break;
	}
	}
}

void CGrenade::Set_Target(CGameObject * _Target)
{
	m_pTarget = _Target;
}

void CGrenade::Set_Device(LPDIRECT3DDEVICE9 pGraphicDev)
{
	m_pGraphicDev = pGraphicDev;
}

void CGrenade::Set_Pos(D3DXVECTOR3 _Pos)
{
	m_pTransform->SetPos(_Pos);
}

void CGrenade::Set_Dir(D3DXVECTOR3 _Dir)
{
	m_pTransform->SetDir(_Dir);
}

void CGrenade::Set_Angle(float * _Angle)
{
	m_fOrigin_YAngle = _Angle[0];

	m_pTransform->SetAngle(_Angle[0], ENGINE::ANGLE_X);
	m_pTransform->SetAngle(_Angle[1], ENGINE::ANGLE_Y);
	m_pTransform->SetAngle(_Angle[2], ENGINE::ANGLE_Z);
}

void CGrenade::Set_Attack(float _Attack)
{
	m_pCondition->Set_Damage(_Attack);
}

CGrenade* CGrenade::Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos, D3DXVECTOR3 _Dir, float* _Angle, float _Speed, ENGINE::WEAPON_TAG _WeaponTag, float _Attack)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CGrenade* pInstance = new CGrenade(pGraphicDev);

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
