#include "stdafx.h"
#include "Bullet.h"
#include "Camera.h"
#include "Trasform.h"
#include "Camera_Component.h"
#include "RigidBody.h"
#include "Collider.h"


CBullet::CBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pKeyMgr(ENGINE::GetKeyMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr),
	m_pCollider(nullptr), m_pRigid(nullptr),
	m_eWeaponTag(ENGINE::WEAPON_TAG::MELLE)
{
}

CBullet::~CBullet()
{
	Release();
}

int CBullet::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::Update();

	m_pCollider->Set_OldPos(m_pTransform->GetPos());
	BulletType();
	
	KeyInput();

	return NO_EVENT;
}

void CBullet::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
	m_pCollider->LateUpdate(m_pTransform->GetPos());

	m_fLifetime -= m_pTimeMgr->GetDelta();
	
	if (m_fLifetime < 0)
		m_bIsDead = true;
}

void CBullet::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &(m_pTransform->GetWorldMatrix()));
	//m_pTexture->Render(0);
	m_pBuffer->Render();
}

HRESULT CBullet::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(0.2f, 0.2f, 0.2f));

	// 물리적 콜라이더
	m_pCollider->Set_Radius({ 0.3f , 0.3f, 0.3f });			// 각 축에 해당하는 반지름을 설정
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

	m_fLifetime = 10.f;

	return S_OK;
}

void CBullet::Release()
{
}

HRESULT CBullet::AddComponent()
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

	// Rigid
	pComponent = ENGINE::CRigidBody::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"RigidBody", pComponent });

	m_pRigid = dynamic_cast<ENGINE::CRigidBody*>(pComponent);
	NULL_CHECK_RETURN(m_pRigid, E_FAIL);

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

		D3DXVECTOR3 JumpLength = { 0, -m_pRigid->Set_Fall(m_pTransform->GetPos(), m_pTimeMgr->GetDelta()), 0 };
		m_pTransform->Move_AdvancedPos_Vec3(JumpLength);

		break;
	}
	case ENGINE::MONSTER_REVOLVER:
	{
		m_pTransform->Move_AdvancedPos(m_dir,m_fSpeed * m_pTimeMgr->GetDelta());
		break;
	}
	}
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

CBullet* CBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos, D3DXVECTOR3 _Dir, float* _Angle, float _Speed , ENGINE::WEAPON_TAG _WeaponTag)
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

	return pInstance;
}
