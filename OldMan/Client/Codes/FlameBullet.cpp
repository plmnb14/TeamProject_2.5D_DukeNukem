#include "stdafx.h"
#include "FlameBullet.h"
#include "Camera.h"
#include "Trasform.h"
#include "Camera_Component.h"
#include "RigidBody.h"
#include "Collider.h"
#include "Billborad.h"
#include "Condition.h"
#include "Animator.h"
#include "CameraObserver.h"
#include "Effect_BulletHit.h"
#include "Effect_BulletHole.h"
#include "Effect_BloodSplit.h"
#include "Effect_RocketSmoke.h"
#include "Effect_Explosion_Rocket.h"
#include "Effect_Fireball.h"
#include "SoundMgr.h"


CFlameBullet::CFlameBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr),
	m_pCollider(nullptr), m_pRigid(nullptr), m_pBillborad(nullptr), m_pObserver(nullptr),
	m_eWeaponTag(ENGINE::WEAPON_TAG::MELLE), m_pSubject(ENGINE::GetCameraSubject()),
	m_pCondition(nullptr)

{
}

CFlameBullet::~CFlameBullet()
{
	Release();
}

int CFlameBullet::Update()
{
	if (m_bIsDead)
	{
		if (m_pCollider->Get_IsCollision())
		{
			CGameObject* pInstance = nullptr;

			if (m_eTag == ENGINE::TERRAIN)
			{
				D3DXVECTOR3 vTempDir = -m_pTransform->GetDir();
				D3DXVECTOR3 vTempPos = { m_pCollider->Get_CenterPos().x + vTempDir.x * 10 ,
					m_pCollider->Get_CenterPos().y + vTempDir.y * 10 ,
					m_pCollider->Get_CenterPos().z + vTempDir.z * 10 };

				for (int i = 0; i < 12; ++i)
				{
					int Angle = rand() % 30;

					pInstance = CEffect_Fireball::Create(m_pGraphicDev, vTempPos, Angle * (i + 1));
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX_HIT, pInstance);
					pInstance->Set_MapLayer(m_mapLayer);
				}
				CSoundMgr::GetInstance()->SetVolume(CSoundMgr::BULLET_DEAD, 1.0f);
				CSoundMgr::GetInstance()->StopSound(CSoundMgr::BULLET_DEAD);
				CSoundMgr::GetInstance()->MyPlaySound(L"RocketBullet_Hit_1.ogg", CSoundMgr::BULLET_DEAD);
			}

			if (m_eTag == ENGINE::PLAYER)
			{
				CSoundMgr::GetInstance()->SetVolume(CSoundMgr::PLAYER_VOICE, 1.0f);
				CSoundMgr::GetInstance()->StopSound(CSoundMgr::PLAYER_VOICE);
				int iSound = rand() % 4;

				switch (iSound)
				{
				case 0:
					CSoundMgr::GetInstance()->MyPlaySound(L"Player_Hitted_1.wav", CSoundMgr::PLAYER_VOICE);
					break;
				case 1:
					CSoundMgr::GetInstance()->MyPlaySound(L"Player_Hitted_2.ogg", CSoundMgr::PLAYER_VOICE);
					break;
				case 2:
					CSoundMgr::GetInstance()->MyPlaySound(L"Player_Hitted_3.ogg", CSoundMgr::PLAYER_VOICE);
					break;
				case 3:
					CSoundMgr::GetInstance()->MyPlaySound(L"Player_Hitted_4.ogg", CSoundMgr::PLAYER_VOICE);
					break;
				}

				CSoundMgr::GetInstance()->SetVolume(CSoundMgr::PLAYER, 1.0f);
				CSoundMgr::GetInstance()->StopSound(CSoundMgr::PLAYER);
				CSoundMgr::GetInstance()->MyPlaySound(L"Player_Hitted_BloodSound_1.ogg", CSoundMgr::PLAYER);

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

void CFlameBullet::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();

	D3DXMatrixIdentity(&m_matView);

	D3DXMATRIX Localmatrix, Cameramatrix;													  //  ����, ī�޶� ��� 
	D3DXVECTOR3 vSize;																		  // ����� ������ 
	Localmatrix = m_pTransform->GetWorldMatrix();
	Cameramatrix = m_pObserver->GetViewMatrix();

	vSize = m_pTransform->GetSize();


	m_pBillborad->Billborad_Front(Localmatrix, Cameramatrix, vSize);                          // ������ ����
	m_matView = m_pBillborad->GetWorldMatrix_Billborad();


	m_pCollider->LateUpdate(m_pTransform->GetPos());
	m_fLifetime -= m_pTimeMgr->GetDelta();

	if (m_fLifetime < 0)
		m_bIsDead = true;
}

void CFlameBullet::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matView);
	m_pTexture->Render(m_pAnimator->RenderSet(m_pTimeMgr->GetDelta()));
	m_pBuffer->Render();
}

HRESULT CFlameBullet::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(0.25f, 0.25f, 0.25f));

	// ������ �ݶ��̴�
	m_pCollider->Set_Radius({ 0.5f , 0.5f, 0.5f });			// �� �࿡ �ش��ϴ� �������� ����
	m_pCollider->Set_Dynamic(true);							// ����, ���� Collider ����
	m_pCollider->Set_Trigger(true);						// Ʈ���� ����
	m_pCollider->Set_CenterPos(m_pTransform->GetPos());		// Collider �� ���߾���ǥ
	m_pCollider->Set_UnderPos();							// Collider �� �ϴ��߾� ��ǥ
	m_pCollider->SetUp_Box();								// ������ �͵��� Collider �� �ݿ��մϴ�.

	// ������ �ٵ� ����
	m_pRigid->Set_UseGravity(true);							// �߷��� ���� ����

	m_pRigid->Set_IsGround(false);							// �������� üũ
	m_pRigid->Set_IsAir(true);								// �������� üũ
	m_pRigid->Set_IsFall(true);								// ���������� üũ
	m_pRigid->Set_IsJump(false);

	m_pRigid->Set_fMass(1.f);								// ��ü�� ����
	m_pRigid->Set_fPower(5.f);								// ���� �Ŀ�

	m_pRigid->Set_Speed({ 1.f , 1.f , 1.f });				// �� �࿡ �ش��ϴ� �ӵ�
	m_pRigid->Set_Accel({ 0.f, -1.f, 0.f });					// �� �࿡ �ش��ϴ� Accel ��
	m_pRigid->Set_MaxAccel({ 1.f , 1.f , 1.f });			// �� �࿡ �ش��ϴ� MaxAccel ��

	m_fLifetime = 2.f;

	int Frame = dynamic_cast<ENGINE::CResources*>(m_pTexture)->Get_MaxFrame();
	m_pAnimator->Set_MaxFrame(Frame);
	m_pAnimator->Set_FrameAmp(40.f);
	m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_ZERO);
	m_pAnimator->Stop_Animation(false);

	return S_OK;
}

HRESULT CFlameBullet::LateInit()
{
	m_pObserver = CCameraObserver::Create();
	NULL_CHECK_RETURN(m_pObserver, E_FAIL);

	m_pSubject->Subscribe(m_pObserver);

	return S_OK;
}

void CFlameBullet::Release()
{
	m_pSubject->UnSubscribe(m_pObserver);
	ENGINE::Safe_Delete(m_pObserver);
}

HRESULT CFlameBullet::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	// Texture
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, L"FlameBall");
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

	//������ 
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

	// Animator
	pComponent = ENGINE::CAnimator::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Animator", pComponent });

	m_pAnimator = dynamic_cast<ENGINE::CAnimator*>(pComponent);
	NULL_CHECK_RETURN(m_pAnimator, E_FAIL);

	return S_OK;
}

void CFlameBullet::KeyInput()
{

}

void CFlameBullet::BulletType()
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

void CFlameBullet::Set_Target(CGameObject * _Target)
{
	m_pTarget = _Target;
}

void CFlameBullet::Set_Device(LPDIRECT3DDEVICE9 pGraphicDev)
{
	m_pGraphicDev = pGraphicDev;
}

void CFlameBullet::Set_Pos(D3DXVECTOR3 _Pos)
{
	m_pTransform->SetPos(_Pos);
}

void CFlameBullet::Set_Dir(D3DXVECTOR3 _Dir)
{
	m_pTransform->SetDir(_Dir);
}

void CFlameBullet::Set_Angle(float * _Angle)
{
	m_pTransform->SetAngle(_Angle[0], ENGINE::ANGLE_X);
	m_pTransform->SetAngle(_Angle[1], ENGINE::ANGLE_Y);
	m_pTransform->SetAngle(_Angle[2], ENGINE::ANGLE_Z);
}

void CFlameBullet::Set_Attack(float _Attack)
{
	m_pCondition->Set_Damage(_Attack);
}

CFlameBullet* CFlameBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos, D3DXVECTOR3 _Dir, float* _Angle, float _Speed, ENGINE::WEAPON_TAG _WeaponTag, float _Attack)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CFlameBullet* pInstance = new CFlameBullet(pGraphicDev);

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