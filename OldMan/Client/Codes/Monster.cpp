#include "stdafx.h"
#include "Monster.h"
#include "Trasform.h"
#include "Collider.h"
#include "Billborad.h"
#include "CameraObserver.h"
#include "RigidBody.h"
#include "Bullet.h"
#include "Condition.h"
#include "Animator.h"


CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr), m_pCollider(nullptr),
	m_pSubject(ENGINE::GetCameraSubject()),
	m_pObserver(nullptr), m_pBillborad(nullptr), m_bShot(false), m_pRigid(nullptr),
	m_pMelleCollider(nullptr), m_pCondition(nullptr), m_pGroundChekCollider(nullptr), m_pAnimator(nullptr)
	, m_fSizeY(0), m_fSizeX(0), m_fFrame(0), m_bAttack(false), m_fDeadTimer(0)
{
}
CMonster::~CMonster()
{
	Release();
}

int CMonster::Update()
{
	Check_Push();
	Check_Physic();

	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();
	// �������� ����� 1. ������ 2. ������ 
	Monster_Foward();
	Monster_State_Set();


	return NO_EVENT;
}

void CMonster::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
	D3DXMatrixIdentity(&m_matView);

	D3DXMATRIX Localmatrix, Cameramatrix;													  //  ����, ī�޶� ��� 
	D3DXVECTOR3 vSize;																		  // ����� ������ 
	Localmatrix = m_pTransform->GetWorldMatrix();

	if (m_pObserver != nullptr)                                                                    //üũ 
		Cameramatrix = m_pObserver->GetViewMatrix();                                              //üũ   ���ϸ� ���� 

	vSize = m_pTransform->GetSize();

	m_pBillborad->Billborad_Yagnle(Localmatrix, Cameramatrix, vSize);                          // ������ ����
	m_matView = m_pBillborad->GetWorldMatrix_Billborad();                                    // �����忡�� ������ ����� �޾ƿ´�. 

	m_bShot = m_pCondition->Get_Hit();

	m_pCollider->LateUpdate(m_pTransform->GetPos());
	m_pMelleCollider->LateUpdate(m_pTransform->GetPos());


	// �̷��� ������ ������ ������ �Ѱ��� 1���� �� �α� �����̴�. �ǰݽ� ��� �ൿ�� �����ȴ�. �׸��� �ǰ��� 0.5 ���� ����Ž���� �����Ų��. 
	if (m_pCondition->Get_Hp() <= 0)
	{
		if (m_eNextState != MONSTER_DEAD)
		{
			m_eNextState = MONSTER_DEAD;
			m_pCollider->Set_MaxY(2.f);
		}
	}
	else
	{
		if (m_bShot)
		{
			m_eNextState = MONSTER_SHOT;
		}
		else
		{
			m_fRangeTiem += m_pTimeMgr->GetDelta();

			if (m_fRangeTiem > 0.5)
			{
				Monster_Range();
				m_fRangeTiem = 0;
			}
		}
	}

	m_bAttack = m_pMelleCollider->Get_IsCollision();


	m_pGroundChekCollider->LateUpdate({ m_pTransform->GetPos().x ,
		m_pTransform->GetPos().y - m_pCollider->Get_Radius().y,
		m_pTransform->GetPos().z });
}

void CMonster::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matView);
	m_pAnimator->RenderSet(m_pTimeMgr->GetDelta());
	m_pTexture->Render(m_pAnimator->Get_Frame());

	m_pBuffer->Render();
}

HRESULT CMonster::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(5.f, 5.f, 8.f));
	m_pTransform->SetSize(D3DXVECTOR3(4.f, 4.f, 4.f));

	m_fMaxRange = 61.0f;//�ִ��Ÿ�
	m_fRange = 0.f;
	m_fMinRange = 35.0f;
	m_fAttack = 19.0f;
	// ������ �ݶ��̴�
	m_pCollider->Set_Radius({ 2.f , 4.f, 2.f });			// �� �࿡ �ش��ϴ� �������� ����
	m_pCollider->Set_Dynamic(true);						// ����, ���� Collider ����
	m_pCollider->Set_Trigger(false);						// Ʈ���� ����
	m_pCollider->Set_CenterPos(m_pTransform->GetPos());		// Collider �� ���߾���ǥ
	m_pCollider->Set_UnderPos();							// Collider �� �ϴ��߾� ��ǥ
	m_pCollider->SetUp_Box();								// ������ �͵��� Collider �� �ݿ��մϴ�.
	m_pCollider->Set_Type(ENGINE::COLLISION_AABB);
	

	//������ �ٵ� ���� 
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
															// Ʈ���� �ݶ��̴� �׶��� 

	m_pGroundChekCollider->Set_Radius({ 0.3f , 0.2f, 0.3f });
	m_pGroundChekCollider->Set_Dynamic(true);
	m_pGroundChekCollider->Set_Trigger(true);
	m_pGroundChekCollider->Set_CenterPos({ m_pTransform->GetPos().x ,
		m_pTransform->GetPos().y - m_pCollider->Get_Radius().y,
		m_pTransform->GetPos().z });
	m_pGroundChekCollider->Set_UnderPos();
	m_pGroundChekCollider->SetUp_Box();

	// Ʈ���� �ݶ��̴�     �νĹ����� ����ϰ� �ʿ��ϴ� 
	m_pMelleCollider->Set_Radius({ 1.6f , 1.6f, 1.6f });
	m_pMelleCollider->Set_Dynamic(true);
	m_pMelleCollider->Set_Trigger(true);
	m_pMelleCollider->Set_CenterPos({ m_pTransform->GetPos().x ,
		m_pTransform->GetPos().y - m_pCollider->Get_Radius().y,
		m_pTransform->GetPos().z });
	m_pMelleCollider->Set_UnderPos();
	m_pMelleCollider->SetUp_Box();
	m_pMelleCollider->Set_Enabled(false);
	m_pMelleCollider->Set_Type(ENGINE::COLLISION_AABB);


	//����� 
	m_pCondition->Set_Hp(100.f);

	m_pCondition->Set_Dodge(false);         //ȸ�� 
	m_pCondition->Set_Cinematic(false);     // ���� 
	m_pCondition->Set_Hit(false);
	m_pCondition->Set_Attack(false);
	m_pCondition->Set_Invincible(false);  //����
	m_pCondition->Set_JetPack(false);
	m_pCondition->Set_MeleeAttack(true);
	m_pCondition->Set_RangeAttack(true);
	m_pCondition->Set_MoveSpeed(10.f);
	m_pCondition->Set_MoveAccel(1.f);


	m_pAnimator->Set_FrameAmp(1.f);									// ������
	m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_ZERO);
	m_pAnimator->Set_Reverse(false);
	m_pAnimator->Set_MaxFrame(0);
	m_pAnimator->Stop_Animation(false);

	m_fSizeX = 50.f;
	m_fSizeY = 50.f;

	m_fHitTime = 0;

	return S_OK;
}

HRESULT CMonster::LateInit()
{
	m_pObserver = CCameraObserver::Create();
	NULL_CHECK_RETURN(m_pObserver, E_FAIL);

	m_pSubject->Subscribe(m_pObserver);

	return S_OK;
}

void CMonster::Release()
{
	m_pSubject->UnSubscribe(m_pObserver);
	ENGINE::Safe_Delete(m_pObserver);
}

HRESULT CMonster::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;
	//texture
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, L"PigMan_WalkFront");
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

	//������ 
	pComponent = ENGINE::CBillborad::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_pBillborad = static_cast<ENGINE::CBillborad*>(pComponent);
	NULL_CHECK_RETURN(m_pBillborad, E_FAIL);
	m_mapComponent.insert({ L"BillBoard", pComponent });

	// Rigid
	pComponent = ENGINE::CRigidBody::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"RigidBody", pComponent });

	m_pRigid = static_cast<ENGINE::CRigidBody*>(pComponent);
	NULL_CHECK_RETURN(m_pRigid, E_FAIL);
	// MEELE
	pComponent = ENGINE::CCollider::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Monster_Mell", pComponent });

	m_pMelleCollider = static_cast<ENGINE::CCollider*>(pComponent);
	NULL_CHECK_RETURN(m_pCollider, E_FAIL);

	// conditoin  
	pComponent = ENGINE::CCondition::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Condition", pComponent });

	m_pCondition = static_cast<ENGINE::CCondition*>(pComponent);
	NULL_CHECK_RETURN(m_pCondition, E_FAIL);

	// �׶��� 
	pComponent = ENGINE::CCollider::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"GCheck_Collider", pComponent });

	m_pGroundChekCollider = static_cast<ENGINE::CCollider*>(pComponent);
	NULL_CHECK_RETURN(m_pGroundChekCollider, E_FAIL);
	//ANIMATER
	pComponent = ENGINE::CAnimator::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Animator", pComponent });

	m_pAnimator = static_cast<ENGINE::CAnimator*>(pComponent);
	NULL_CHECK_RETURN(m_pAnimator, E_FAIL);


	return S_OK;
}


void CMonster::Player_Pursue(float _move)
{
	D3DXVECTOR3 vPlayer_Pos = static_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // �÷��̾���ġ
	D3DXVECTOR3 vMonster_Pos = m_pTransform->GetPos();
	D3DXVECTOR3 vPlayer_Pos_Top = { vPlayer_Pos.x, vPlayer_Pos.y + 1,vPlayer_Pos.z };
	D3DXVECTOR3 vMonster_Player_Dir = vPlayer_Pos_Top - vMonster_Pos;
	//�¿� ���⺤�ͷ� ������ ���ϰ�  �װ����� ���� ������ �����ϴ� ���� ���ؼ� �������� ���Ը���°� �ٽ� 
	// �÷��̾��� ������ �޾Ƽ� ���� ���Ͱ� ���� ����ɴ� ������ ���ܼ� top���� ��ġ�� �ٸ��� �� 

	//DXVECTOR3 vMonster2 = { vMonster.x,m_pTransform->GetPos().y,vMonster.z };  //   ������-> ���� Ƣ�鼭 ���� ��� ���̰��� ���� �׷��� 
	m_MoveSpeed = 1.f* _move * m_pTimeMgr->GetDelta();   // �ӵ�
	m_pAnimator->Stop_Animation(false);
	m_pTransform->Move_AdvancedPos(vMonster_Player_Dir, m_MoveSpeed);

}

void CMonster::Monster_Foward()
{

	Monster_Callcul();

	D3DXVECTOR3 vMonster_Player_Dir_Free = m_vPlayer_Pos - m_vMonster_Pos;    //���ʿ��ϴ���?

	D3DXVECTOR3 vMonsterDir_Forward = { 1.f, 0.f,0.f };                // ������ �� ���� X�� ���� �� ���� 
																	   //m_pTransform->SetDir(vMonster_Player_Dir);
	D3DXVec3Normalize(&m_vMonster_Player_Dir, &m_vMonster_Player_Dir);
	D3DXVec3Normalize(&vMonsterDir_Forward, &vMonsterDir_Forward);
	D3DXVec3Normalize(&m_vMonsterDir_Fowrd_Get, &m_vMonsterDir_Fowrd_Get);

	float fDot_Player_Monster_Forward, fDot_Monster_Right, fMonster_Get_Angle;
	fDot_Player_Monster_Forward = D3DXVec3Dot(&m_vMonster_Player_Dir, &m_vMonsterDir_Fowrd_Get);
	D3DXVECTOR3 vMonsterDir_Right = { m_vMonsterDir_Fowrd_Get.x, m_vMonsterDir_Fowrd_Get.y + 1.0f,m_vMonsterDir_Fowrd_Get.z };      																   //m_pTransform->SetDir(vMonster_Player_Dir);

	D3DXVECTOR3 vMonster_RIght_Dir, Mon_Left_Dir, cross;

	D3DXVec3Cross(&vMonster_RIght_Dir, &m_vMonsterDir_Fowrd_Get, &vMonsterDir_Right); // ���� ���͸� ���ϱ� ���� ���� ECTOR3(0.0f, -1.0f, 0.0f)); // ���� ���͸� ���ϱ� ���� ���� 

	fDot_Monster_Right = D3DXVec3Dot(&vMonster_RIght_Dir, &m_vMonster_Player_Dir);                 // - �϶� ������ +�����̴�. 
	m_fFowardDealy += m_pTimeMgr->GetDelta();

	if (m_fFowardDealy > 0.3)
	{
		if (fDot_Player_Monster_Forward * 90 > 67.5 && fDot_Player_Monster_Forward * 90 < 90 && fDot_Monster_Right>0)
		{//2��

			if (0 < fDot_Monster_Right * 90 && fDot_Monster_Right * 90 < 22.5)            // ��- ����
			{
				m_pAnimator->Stop_Animation(false);
				ChangeTex(L"PigMan_WalkFront");
				m_pAnimator->Set_FrameAmp(5.f);
				cout << "����- ������1 " << endl;
				m_fFowardDealy = 0;
			}

		}
		else if (fDot_Player_Monster_Forward * 90 > 67.5 && fDot_Player_Monster_Forward * 90 < 90 && fDot_Monster_Right<0)
		{
			if (-22.5 < fDot_Monster_Right * 90 && fDot_Monster_Right * 180 < 0)                          //�� - ����
			{
				m_pAnimator->Stop_Animation(false);
				ChangeTex(L"PigMan_WalkFront");
				m_pAnimator->Set_FrameAmp(5.f);
				cout << "�� -����2 " << endl;
			//	m_pTransform->MoveAngle(ENGINE::ANGLE_Y, 8.f);
			//	m_pTransform->MoveAngle(ENGINE::ANGLE_X, 5.f);
			
				m_fFowardDealy = 0;
			}

		}
		else if (fDot_Player_Monster_Forward * 90 > 22.5 && fDot_Player_Monster_Forward * 90 < 67.5&& fDot_Monster_Right > 0)
		{
			if (22.5 < fDot_Monster_Right * 90 && fDot_Monster_Right * 90 <67.5)                          //�� - ����
			{
				m_pAnimator->Stop_Animation(false);
				ChangeTex(L"PigMan_WalkSideFrontLeft");
				m_pAnimator->Set_FrameAmp(5.f);
				cout << "�� -����3 " << endl;
				m_pTransform->SetDir(vMonster_Player_Dir_Free);
	//		m_pTransform->MoveAngle(ENGINE::ANGLE_Y, 8.f);
			//	m_pTransform->MoveAngle(ENGINE::ANGLE_X, 5.f);
				m_fFowardDealy = 0;
			}

		}
		else if (fDot_Player_Monster_Forward * 90 > 22.5 && fDot_Player_Monster_Forward * 90 < 67.5&& fDot_Monster_Right < 0)
		{
			if (-67.5 < fDot_Monster_Right * 90 && fDot_Monster_Right * 90 < -22.5)                          //�� - ����
			{
				m_pAnimator->Stop_Animation(false);
				ChangeTex(L"PigMan_WalkSideFrontRight");
				m_pAnimator->Set_FrameAmp(5.f);
				cout << "�� -����4 " << endl;
			//	m_pTransform->MoveAngle(ENGINE::ANGLE_Y, 8.f);
			//	m_pTransform->MoveAngle(ENGINE::ANGLE_X, 5.f);
				m_pTransform->SetDir(vMonster_Player_Dir_Free);


				m_fFowardDealy = 0;
			}

		}
		else if (fDot_Player_Monster_Forward * 90 > -22.5 && fDot_Player_Monster_Forward * 90 < 0 && fDot_Monster_Right < 0)
		{
			if (-90 < fDot_Monster_Right * 90 && fDot_Monster_Right * 90 < -67.5)                          //�� - ����
			{
				m_pAnimator->Stop_Animation(false);
				ChangeTex(L"PigMan_WalkSideRight");
				m_pAnimator->Set_FrameAmp(5.f);
				cout << "�� -����5 " << endl;
			//	m_pTransform->MoveAngle(ENGINE::ANGLE_Y, 8.f);
			//	m_pTransform->MoveAngle(ENGINE::ANGLE_X, 5.f);
				m_pTransform->SetDir(vMonster_Player_Dir_Free);


				m_fFowardDealy = 0;
			}

		}
		else if (fDot_Player_Monster_Forward * 90 > -22.5 && fDot_Player_Monster_Forward * 90 < 0 && fDot_Monster_Right > 0)
		{
			if (67.5 < fDot_Monster_Right * 90 && fDot_Monster_Right * 90 <90)                          //��- ����
			{
				m_pAnimator->Stop_Animation(false);
				ChangeTex(L"PigMan_WalkSideLeft");
				m_pAnimator->Set_FrameAmp(5.f);
				cout << "�� -����6 " << endl;
			//	m_pTransform->MoveAngle(ENGINE::ANGLE_Y, 8.f);
			//	m_pTransform->MoveAngle(ENGINE::ANGLE_X, 5.f);
				m_pTransform->SetDir(vMonster_Player_Dir_Free);


				m_fFowardDealy = 0;
			}

		}
		else if (fDot_Player_Monster_Forward * 90 > -67.5 && fDot_Player_Monster_Forward * 90 < -22.5 && fDot_Monster_Right < 0)
		{
			if (-67.5 < fDot_Monster_Right * 90 && fDot_Monster_Right * 90 < -22.5)                          //�� - ������
			{
				m_pAnimator->Stop_Animation(false);
				ChangeTex(L"PigMan_WalkSideBackRigt");

				m_pAnimator->Set_FrameAmp(5.f);
			//	m_pTransform->MoveAngle(ENGINE::ANGLE_Y, 8.f);
			//	m_pTransform->MoveAngle(ENGINE::ANGLE_X, 5.f);
				m_pTransform->SetDir(vMonster_Player_Dir_Free);


				cout << "�� -����7 " << endl;
				m_fFowardDealy = 0;
			}

		}
		else if (fDot_Player_Monster_Forward * 90 > -67.5 && fDot_Player_Monster_Forward * 90 < -22.5 && fDot_Monster_Right >  0)
		{
			if (22.5 < fDot_Monster_Right * 90 && fDot_Monster_Right * 90 < 67.5)                          //�� - ������
			{
				m_pAnimator->Stop_Animation(false);
				ChangeTex(L"PigMan_WalkSideBack");

				m_pAnimator->Set_FrameAmp(5.f);
				cout << "�� -����8 " << endl;
				m_pTransform->MoveAngle(ENGINE::ANGLE_Y, 8.f);
			//	m_pTransform->MoveAngle(ENGINE::ANGLE_X, 5.f);
				//	m_pTransform->SetAngle(90.f,ENGINE::ANGLE_Y);

				m_pTransform->SetDir(vMonster_Player_Dir_Free);

				m_fFowardDealy = 0;
			}

		}
		else if (fDot_Player_Monster_Forward * 90 > -90 && fDot_Player_Monster_Forward * 90 < -67.5 && fDot_Monster_Right >  0)
		{
			if (0 < fDot_Monster_Right * 90 && fDot_Monster_Right * 90 < 22.5)                          //�� - ������
			{
				m_pAnimator->Stop_Animation(false);
				ChangeTex(L"PigMan_WalkBack");
				m_pAnimator->Set_FrameAmp(5.f);
				cout << "�� -����9 " << endl;
		//		m_pTransform->MoveAngle(ENGINE::ANGLE_Y, 8.f);
				//m_pTransform->MoveAngle(ENGINE::ANGLE_X, 5.f);
				m_pTransform->SetDir(vMonster_Player_Dir_Free);

				//	m_pTransform->SetAngle(90.f,ENGINE::ANGLE_Y);

				m_fFowardDealy = 0;
			}

		}
		else if (fDot_Player_Monster_Forward * 90 > -90 && fDot_Player_Monster_Forward * 90 < -67.5 && fDot_Monster_Right < 0)
		{
			if (-22.5 < fDot_Monster_Right * 90 && fDot_Monster_Right * 90 < 0)                          //�� - ������
			{
				m_pAnimator->Stop_Animation(false);
				ChangeTex(L"PigMan_WalkBack");
				m_pAnimator->Set_FrameAmp(5.f);
				cout << "�� -����10 " << endl;
		//	m_pTransform->MoveAngle(ENGINE::ANGLE_Y, 8.f);
				//m_pTransform->MoveAngle(ENGINE::ANGLE_X, 5.f);
				//	m_pTransform->SetAngle(90.f,ENGINE::ANGLE_Y);
			m_pTransform->SetDir(vMonster_Player_Dir_Free);


				m_fFowardDealy = 0;
			}

		}
	}
	
}

void CMonster::Monster_Range()
{
	D3DXVECTOR3 vPlayer_Pos = static_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // �÷��̾���ġ
	D3DXVECTOR3 vMonster_Pos = m_pTransform->GetPos();

	D3DXVECTOR3 vMonster_Player_Dir = vPlayer_Pos - vMonster_Pos;
	float  fRange;
	fRange = D3DXVec3Length(&(vMonster_Pos - vPlayer_Pos));				 // �����Ÿ�

	if (fRange < m_fMaxRange && fRange >m_fMinRange)
	{
		m_eNextState = MONSTER_PURSUE;
	}
	else if (fRange <m_fMinRange && fRange>m_fAttack)
	{
		m_eNextState = MONSTER_FIRE;
	}
	else {
		m_eNextState = MONSTER_IDLE;
	}
}
void CMonster::Monster_Idle()
{
	m_pTransform->MovePos(0.f);
}
void CMonster::Monster_Shot()
{
	D3DXVECTOR3 vPlayer_Pos = static_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // �÷��̾���ġ
	D3DXVECTOR3 vMonster_Pos = m_pTransform->GetPos();
	D3DXVECTOR3 vPlayer_Pos_Top = { vPlayer_Pos.x, vPlayer_Pos.y + 1,vPlayer_Pos.z };
	D3DXVECTOR3 vPlayer_Pos_Top_Top = { vPlayer_Pos.x, vPlayer_Pos.y + 15,vPlayer_Pos.z };
	D3DXVECTOR3 vMonster_Player_Dir = vPlayer_Pos_Top - vMonster_Pos;
	D3DXVECTOR3 vMonster_Dir_Top = vPlayer_Pos_Top_Top - vMonster_Pos;
	D3DXVECTOR3 vMonsterDir_Forward = { 1.f, 0.f,0.f };                // ������ �� ���� X�� ���� �� ���� 

	D3DXVECTOR3 vMonsterDir_Get = m_pTransform->GetDir();                // ������ �� ���� X�� ���� �� ���� 
	D3DXVECTOR3 vMonster_RIght_Dir, Mon_Left_Dir, cross;
	D3DXVec3Normalize(&vMonsterDir_Forward, &vMonsterDir_Forward);
	D3DXVec3Normalize(&vMonster_Player_Dir, &vMonster_Player_Dir);
	D3DXVec3Normalize(&vMonsterDir_Get, &vMonsterDir_Get);
	D3DXVec3Cross(&vMonster_RIght_Dir, &vMonster_Player_Dir, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	float fDot_Player_Monster_Forward;
	fDot_Player_Monster_Forward = D3DXVec3Dot(&vMonster_Player_Dir, &vMonsterDir_Forward);
	//m_pCondition->Add_Hp(-1);

	//������ ����� 
	m_pTransform->MovePos(0.f);
	ChangeTex(L"PigMan_Dead");
	m_pAnimator->Set_Frame(0);
	m_pAnimator->Stop_Animation(true);
	//m_pCondition->Add_Hp(-1);


	D3DXVECTOR3 vMonsterPos_ShotPoint = { vMonster_Pos.x - vMonster_RIght_Dir.x, vMonster_Pos.y - vMonster_RIght_Dir.y  ,vMonster_Pos.z - vMonster_RIght_Dir.z };

	D3DXVECTOR3 vMonster = vPlayer_Pos_Top - vMonsterPos_ShotPoint;
	float fAngle[3] = { 0.f };                      //���� 0���� ����� ���ָ� �Ѿ� ������ ���� ���� 
	float fRange = D3DXVec3Length(&(vMonster_Pos - vPlayer_Pos));

	m_fHitTime += m_pTimeMgr->GetDelta();

	float fMove;
	fMove = 100.f * m_pTimeMgr->GetDelta();
	if (m_fHitTime > 0.3)
	{
		m_bShot = false;

		m_pAnimator->Stop_Animation(false);
		m_pCondition->Set_Hit(false);
		m_eNextState = MONSTER_PURSUE;
		m_fHitTime = 0;
	
	}
	
}



void CMonster::Monster_Fire2()
{
	D3DXVECTOR3 vPlayer_Pos = static_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();
	D3DXVECTOR3 vMonster_Pos = m_pTransform->GetPos();
	D3DXVECTOR3 vPlayer_Pos_Top = { vPlayer_Pos.x, vPlayer_Pos.y + 1,vPlayer_Pos.z };
	D3DXVECTOR3 vMonster_Player_Dir = vPlayer_Pos_Top - vMonster_Pos;
	D3DXVECTOR3 vMonsterDir_Fowrd_Get = m_pTransform->GetDir();
	D3DXVECTOR3 vMonsterDir_Forward = { 1.f, 0.f,0.f };                // ������ �� ���� X�� ���� �� ���� 

	D3DXVECTOR3 vMonster_RIght_Dir, Mon_Left_Dir, cross;
	D3DXVec3Normalize(&vMonsterDir_Forward, &vMonsterDir_Forward);
	D3DXVec3Normalize(&vMonsterDir_Fowrd_Get, &vMonsterDir_Fowrd_Get);
	D3DXVec3Normalize(&vMonster_Player_Dir, &vMonster_Player_Dir);

	D3DXVec3Cross(&vMonster_RIght_Dir, &vMonster_Player_Dir, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));        //���⺤�͸� ���ϱ� ���� �� 

	D3DXVECTOR3 vMonsterPos_ShotPoint = { vMonster_Pos.x - vMonster_RIght_Dir.x, vMonster_Pos.y - vMonster_RIght_Dir.y  ,vMonster_Pos.z - vMonster_RIght_Dir.z };
	D3DXVECTOR3 vMonster = vPlayer_Pos_Top - vMonsterPos_ShotPoint;

	float fMove;
	fMove = 180.f * m_pTimeMgr->GetDelta();			 // �ӵ�
	float fAngle[3] = { 0.f };                      //���� 0���� ����� ���ָ� �Ѿ� ������ ���� ���� 
	m_fTime += m_pTimeMgr->GetDelta();

	float fDot_Player_Monster_Forward;                        // ������ ������ ���ϱ� ���� ������ -> +���� -�ĸ� �̴�. 
	fDot_Player_Monster_Forward = D3DXVec3Dot(&vMonster_Player_Dir, &vMonsterDir_Fowrd_Get);

	m_pAnimator->Stop_Animation(false);

	ChangeTex(L"PigMan_PreFire");
	m_pAnimator->Set_FrameAmp(0.5f);

	//cout << fDot_Player_Monster_Forward << endl;
	                                      // ������ ��츸 ����Ѵ�. 
		if (m_fTime > 0.9f)
		{
			ChangeTex(L"PigMan_Fire");
			m_pAnimator->Set_FrameAmp(2.f);
			m_pAnimator->Stop_Animation(false);

		}
		if (m_fTime > 1)
		{
			for (int i = 0; i < 5;i++) 
			{
				float xRand = rand() % 500 * 0.01f;
				float yRand = rand() % 500 * 0.01f;
				float zRand = rand() % 500 * 0.01f;
				D3DXVECTOR3 vMonster_Shot{ vMonster.x + xRand,vMonster.y+ yRand,vMonster.z+ zRand };


				CGameObject* pInstance = CBullet::Create(m_pGraphicDev, vMonsterPos_ShotPoint, vMonster_Shot, fAngle, fMove, ENGINE::MONSTER_REVOLVER);
				pInstance->Set_MapLayer(m_mapLayer);
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::BULLET_MONSTER, pInstance);
			}
			m_fTime = 0;
		}
}


void CMonster::Monster_Dead()
{
	m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_STOP);
	ChangeTex(L"PigMan_Dead");
	m_pAnimator->Set_FrameAmp(1.f);
	m_pAnimator->Set_Frame(5);



	m_fDeadTimer += m_pTimeMgr->GetDelta();

	if (m_fDeadTimer > 3)
	{
		m_bIsDead = true;
	}


}

void CMonster::Monster_Attack()
{

	float fMove;
	fMove = 100.f * m_pTimeMgr->GetDelta();
	m_pMelleCollider->Set_Enabled(true);


	//m_pTransform->Move_AdvancedPos(vMonster_Player_Dir, fMove);
	if (m_pMelleCollider->Get_IsCollision())
	{
		//m_pCondition->Add_Hp(+1);
	}





}

void CMonster::Monster_Callcul()
{
	m_vPlayer_Pos = static_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();
	m_vMonster_Pos = m_pTransform->GetPos();
	m_vPlayer_Pos_Top = { m_vPlayer_Pos.x, m_vPlayer_Pos.y + 3 ,m_vPlayer_Pos.z };
	m_vMonster_Player_Dir = m_vPlayer_Pos_Top - m_vMonster_Pos;
	m_vMonsterDir_Fowrd_Get = m_pTransform->GetDir();

}


void CMonster::Check_Physic()
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
			//if (m_pCondition->Get_Run())
			//{
			//	m_pCondition->Set_MoveSpeed(16.f);
			//	D3DXVECTOR3 vTemp = { 0.3f , 0.2f , 0.3f };
			//	static_cast<CCamera*>(m_pCamera)->Set_CamShakePos(vTemp);
			//}

			return;
		}

		D3DXVECTOR3 JumpLength = { 0, -m_pRigid->Set_Fall(m_pTransform->GetPos(), m_pTimeMgr->GetDelta()),0 };
		m_pTransform->Move_AdvancedPos_Vec3(JumpLength);
	}


}

void CMonster::Object_Collison()
{
	// ���δ� �޾ƿ´�. 
	// �׷��� �� ������ ��ġ�°� ������ �Ǵ��ϴ°� �´�. 
	// �� �Ǵ��ϱ� 
	// �ؽ�ó �ֱ� 


}

void CMonster::ChangeTex(wstring _wstrTex)
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

void CMonster::Check_Push()
{
	if (m_pRigid->Get_IsPush())
	{
		float m_fSpeed = m_pRigid->Get_Distance() * m_pRigid->Get_Distance() * m_pTimeMgr->GetDelta();
		D3DXVECTOR3 vTmpDir = { m_pRigid->Get_PushDir().x , m_pRigid->Get_PushDir().y , m_pRigid->Get_PushDir().z };

		m_pTransform->Move_AdvancedPos(vTmpDir, m_fSpeed);
		m_pRigid->Set_Distance(m_pRigid->Get_Distance() - (m_pRigid->Get_Distance() * m_pRigid->Get_Distance() *m_pTimeMgr->GetDelta()));

		if (m_pRigid->Get_Distance() <= 0.2f)
		{
			m_pRigid->Set_Distance(0.f);
			m_pRigid->Set_IsPush(false);
		}
	}
}

// ���±�� ���� �ǰ� �������� �ǰ��� ������ �ع����� ������ ���� 
void CMonster::Monster_State_Set()
{
	switch (m_eNextState)
	{
	case MONSTER_IDLE:
		Monster_Idle();
		break;
	case MONSTER_PURSUE:
		Player_Pursue(0.4f);
		break;
	case MONSTER_SHOT:
		Monster_Shot();
		break;
	case MONSTER_FIRE:
		Monster_Fire2();
		break;
	case MONSTER_DEAD:
		Monster_Dead();
		break;
	case MONSTER_MILL:
		Monster_Attack();
		break;
	}

	// �¾��� ��� Ž���� ������. 

}

void CMonster::Set_Pos(D3DXVECTOR3 _Pos)
{
	m_pTransform->SetPos(_Pos);
}

CMonster * CMonster::Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _Target, D3DXVECTOR3 _Pos)
{

	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CMonster* pInstance = new CMonster(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}
	pInstance->Set_Target(_Target);
	pInstance->Set_Pos(_Pos);

	return pInstance;
}
