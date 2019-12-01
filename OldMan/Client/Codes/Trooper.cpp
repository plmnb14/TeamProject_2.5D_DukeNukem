#include "stdafx.h"
#include "Trooper.h"
#include "Trasform.h"
#include "Collider.h"
#include "Billborad.h"
#include "CameraObserver.h"
#include "RigidBody.h"
#include "Bullet.h"
#include "Condition.h"
#include "Animator.h"


CTrooper::CTrooper(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
{
}
CTrooper::~CTrooper()
{
	Release();
}

int CTrooper::Update()
{

	if (m_bIsDead)
		return DEAD_OBJ;
	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();
	Check_Physic();
	// �������� ����� 1. ������ 2. ������ 
	//Monster_Foward();

	Monster_State_Set();

	//Player_Pursue();
	//cout << m_pCondition->Get_Hp() << endl;



	return NO_EVENT;
}

void CTrooper::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
	D3DXMatrixIdentity(&m_matView);

	D3DXMATRIX Localmatrix, Cameramatrix;													  //  ����, ī�޶� ��� 
	D3DXVECTOR3 vSize;																		  // ����� ������ 
	Localmatrix = m_pTransform->GetWorldMatrix();
	Cameramatrix = m_pObserver->GetViewMatrix();
	vSize = m_pTransform->GetSize();

	m_pBillborad->Billborad_Yagnle(Localmatrix, Cameramatrix, vSize);                          // ������ ����
	m_matView = m_pBillborad->GetWorldMatrix_Billborad();                                    // �����忡�� ������ ����� �޾ƿ´�. 

	m_bShot = m_pCondition->Get_Hit();

	m_pCollider->LateUpdate(m_pTransform->GetPos());
	m_pMelleCollider->LateUpdate(m_pTransform->GetPos());

	cout << m_pCondition->Get_Hp() << endl;

	// �̷��� ������ ������ ������ �Ѱ��� 1���� �� �α� �����̴�. �ǰݽ� ��� �ൿ�� �����ȴ�. �׸��� �ǰ��� 0.5 ���� ����Ž���� �����Ų��. 
	if (m_pCondition->Get_Hp() <= 0)
	{
		m_eNextState = MONSTER_DEAD;

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

void CTrooper::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matView);
	m_pAnimator->RenderSet(m_pTimeMgr->GetDelta());
	m_pTexture->Render(m_pAnimator->Get_Frame());

	m_pBuffer->Render();
}

HRESULT CTrooper::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 12.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(4.f, 4.f, 4.f));

	m_fMaxRange = 19.0f;//�ִ��Ÿ�
	m_fRange = 0.f;
	m_fMinRange = 11.0f;
	m_fAttack = 5.0f;
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
		m_pTransform->GetPos().y,
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

HRESULT CTrooper::LateInit()
{
	m_pObserver = CCameraObserver::Create();
	NULL_CHECK_RETURN(m_pObserver, E_FAIL);

	m_pSubject->Subscribe(m_pObserver);

	return S_OK;
}

void CTrooper::Release()
{
	m_pSubject->UnSubscribe(m_pObserver);
	ENGINE::Safe_Delete(m_pObserver);
}

HRESULT CTrooper::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;
	//texture
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, L"PigMan_Fire");
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
	NULL_CHECK_RETURN(m_pMelleCollider, E_FAIL);

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


//���� ������ �Ⱥ����� �ϳ� ����� �ؾߵ� -> ������ ���� ������ �ٸ��� �ǾߵǱ� �����̴�. 
// 360/ 8 /45 �������� �׸��� ����ϰ� ������ �� ����� �Ǵ� ������ 
// 4���� ������ ����� �����ؾ� �� -> 45, 90,135,180 
// �ڸ� �� ��� ����� ���Ѵ� -> �������� ���� ������ ���̸� ������ ���� ���� 
// ���ϰ�� ������ �� �޴� ���� �ϳ� ���� 
//���� ������ ���ؾ� �Ѵ�. -> ���� -> ������ �Ǵ��� ���� �����ȴ�. 
// 
void CTrooper::Player_Pursue(float _move)
{
	D3DXVECTOR3 vPlayer_Pos = static_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // �÷��̾���ġ
	D3DXVECTOR3 vMonster_Pos = m_pTransform->GetPos();
	D3DXVECTOR3 vPlayer_Pos_Top = { vPlayer_Pos.x, vPlayer_Pos.y + 1,vPlayer_Pos.z };
	D3DXVECTOR3 vMonster_Player_Dir = vPlayer_Pos_Top - vMonster_Pos;
	//�¿� ���⺤�ͷ� ������ ���ϰ�  �װ����� ���� ������ �����ϴ� ���� ���ؼ� �������� ���Ը���°� �ٽ� 
	// �÷��̾��� ������ �޾Ƽ� ���� ���Ͱ� ���� ����ɴ� ������ ���ܼ� top���� ��ġ�� �ٸ��� �� 

	//DXVECTOR3 vMonster2 = { vMonster.x,m_pTransform->GetPos().y,vMonster.z };  //   ������-> ���� Ƣ�鼭 ���� ��� ���̰��� ���� �׷��� 
	m_MoveSpeed = 2.f* _move * m_pTimeMgr->GetDelta();   // �ӵ�

	m_pTransform->Move_AdvancedPos(vMonster_Player_Dir, m_MoveSpeed);

}

void CTrooper::Monster_Foward()
{

	D3DXVECTOR3 vPlayer_Pos = static_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // �÷��̾���ġ
	D3DXVECTOR3 vMonster_Pos = m_pTransform->GetPos();

	D3DXVECTOR3 vMonster_Player_Dir = vPlayer_Pos - vMonster_Pos;
	D3DXVECTOR3 vMonster_Player_Dir_Free = vPlayer_Pos - vMonster_Pos;

	D3DXVECTOR3 vMonsterDir_Get = m_pTransform->GetDir();              // ���Ͱ� ���� ���� 

	D3DXVECTOR3 vMonsterDir_Forward = { 1.f, 0.f,0.f };                // ������ �� ���� X�� ���� �� ���� 
																	   //m_pTransform->SetDir(vMonster_Player_Dir);

	D3DXVec3Normalize(&vMonster_Player_Dir, &vMonster_Player_Dir);
	D3DXVec3Normalize(&vMonsterDir_Forward, &vMonsterDir_Forward);
	D3DXVec3Normalize(&vMonsterDir_Get, &vMonsterDir_Get);
	float fDot_Player_Monster_Forward, fDot_Monster_Right, fMonster_Get_Angle;
	fDot_Player_Monster_Forward = D3DXVec3Dot(&vMonster_Player_Dir, &vMonsterDir_Get);

	D3DXVECTOR3 vMonster_RIght_Dir, Mon_Left_Dir, cross;
	D3DXVec3Cross(&vMonster_RIght_Dir, &vMonsterDir_Forward, &D3DXVECTOR3(0.0f, 1.0f, 0.0f)); // ���� ���͸� ���ϱ� ���� ���� 
																							  //D3DXVec3Cross(&Mon_Left_Dir, &vMonsterDir_Fowrd2, &D3DXVECTOR3(0.0f, -1.0f, 0.0f)); // ���� ���͸� ���ϱ� ���� ���� 

	fDot_Monster_Right = D3DXVec3Dot(&vMonster_RIght_Dir, &vMonster_Player_Dir);                 // - �϶� ������ +�����̴�. 
	m_fFowardDealy += m_pTimeMgr->GetDelta();

	if (m_fFowardDealy > 0.1) {
		if (acos(fDot_Player_Monster_Forward) * 90 < 250)
		{
			m_pAnimator->Stop_Animation(false);
			ChangeTex(L"OctaBrain_Idle");
			m_pAnimator->Set_FrameAmp(5.f);
			m_fFowardDealy = 0;
			Player_Pursue(1.f);
		}
		else if (acos(fDot_Player_Monster_Forward) * 90 > 250)
		{
			m_pAnimator->Stop_Animation(false);
			ChangeTex(L"OctaBrain_Idle");
			m_pAnimator->Set_FrameAmp(5.f);
			m_pTransform->MoveAngle(ENGINE::ANGLE_Y, 10.f);
			Player_Pursue(0.f);
			m_fFowardDealy = 0;
		}
	}
	//	cout<<m_pTransform->GetAngle(ENGINE::ANGLE_X)*180 <<"X"<<endl;
	//	cout << m_pTransform->GetAngle(ENGINE::ANGLE_Y) << "Y" << endl;
	//	cout << m_pTransform->GetAngle(ENGINE::ANGLE_Z)*180<<"Z" << endl;
	//cout << acos(fDot_Monster_Right)*90 << endl;
	cout << acos(fDot_Player_Monster_Forward) * 90 << endl;
	//cout << vMonster_Player_Dir.x <<"X"<< endl;
	//cout << vMonster_Player_Dir.y <<"Y"<< endl;
	//cout << vMonster_Player_Dir.z <<"Z"<<endl;
	//m_pTransform->SetDir(vMonster_Player_Dir);
	//cout << test <<"�¿�"<< endl;  
	//cout << Dot2 <<"�յ�"<< endl;

}

void CTrooper::Monster_Range()
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
	else if (fRange<m_fAttack)
	{
		m_eNextState = MONSTER_MILL;

	}
}
void CTrooper::Monster_Idle()
{
	m_pTransform->MovePos(0.f);
}
void CTrooper::Monster_Shot()
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
	ChangeTex(L"Trooper_DeadFall");
	m_pAnimator->Set_Frame(0.f);
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

		//	if (acos(fDot_Player_Monster_Forward) * 90 > 250)
		{
			m_pAnimator->Stop_Animation(false);
			m_pCondition->Set_Hit(false);
			m_eNextState = MONSTER_PURSUE;
			m_fHitTime = 0;

			//if (fRange > 7)                                       //5 ��Ÿ� �ȿ��� �ǰݴ��Ұ�� ���� ����� �ϰ� ����� ����̴�. ������������
			{
				/*if (m_fDelayTime > 0.2)
				{
				CGameObject* pInstance = CBullet::Create(m_pGraphicDev, vMonsterPos_ShotPoint, vMonster_Dir_Top, fAngle, fMove, ENGINE::MONSTER_REVOLVER);
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::BULLET_MONSTER, pInstance);
				m_fHitTime = 0;
				m_fDelayTime = 0;
				m_eNextState = MONSTER_PURSUE;

				}*/
			}
		}
	}
}



void CTrooper::Monster_Fire2()
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
	fMove = 100.f * m_pTimeMgr->GetDelta();			 // �ӵ�
	float fAngle[3] = { 0.f };                      //���� 0���� ����� ���ָ� �Ѿ� ������ ���� ���� 
	m_fTime += m_pTimeMgr->GetDelta();

	float fDot_Player_Monster_Forward;                        // ������ ������ ���ϱ� ���� ������ -> +���� -�ĸ� �̴�. 
	fDot_Player_Monster_Forward = D3DXVec3Dot(&vMonster_Player_Dir, &vMonsterDir_Fowrd_Get);



	//cout << fDot_Player_Monster_Forward << endl;

		if (m_fTime > 1.8f)
		{
			ChangeTex(L"Trooper_FireFront");
			m_pAnimator->Set_FrameAmp(1.f);
			m_pAnimator->Stop_Animation(false);

		}
		if (m_fTime > 2)
		{
			CGameObject* pInstance = CBullet::Create(m_pGraphicDev, vMonsterPos_ShotPoint, vMonster, fAngle, fMove, ENGINE::MONSTER_REVOLVER);
			m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::BULLET_MONSTER, pInstance);
			m_fTime = 0;
		}
}


void CTrooper::Monster_Dead()
{
	m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_STOP);
	ChangeTex(L"Trooper_DeadFall");
	m_pAnimator->Set_FrameAmp(1.f);
	m_pAnimator->Set_Frame(5.f);

}

void CTrooper::Monster_Attack()
{

	D3DXVECTOR3 vPlayer_Pos = static_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();
	D3DXVECTOR3 vMonster_Pos = m_pTransform->GetPos();
	D3DXVECTOR3 vPlayer_Pos_Top = { vPlayer_Pos.x, vPlayer_Pos.y + 1,vPlayer_Pos.z };
	D3DXVECTOR3 vMonster_Player_Dir = vPlayer_Pos_Top - vMonster_Pos;
	float fMove;
	fMove = 100.f * m_pTimeMgr->GetDelta();
	m_pMelleCollider->Set_Enabled(true);


	//m_pTransform->Move_AdvancedPos(vMonster_Player_Dir, fMove);
	if (m_pMelleCollider->Get_IsCollision())
	{
		m_pCondition->Add_Hp(+1);
	}
}

void CTrooper::Check_Physic()
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

void CTrooper::Object_Collison()
{
	// ���δ� �޾ƿ´�. 
	// �׷��� �� ������ ��ġ�°� ������ �Ǵ��ϴ°� �´�. 
	// �� �Ǵ��ϱ� 
	// �ؽ�ó �ֱ� 


}


// ���±�� ���� �ǰ� �������� �ǰ��� ������ �ع����� ������ ���� 
void CTrooper::Monster_State_Set()
{
	switch (m_eNextState)
		{
		case MONSTER_IDLE:
			Monster_Idle();
			break;
		case MONSTER_PURSUE:
			Player_Pursue(0.9f);
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
	

}

CTrooper * CTrooper::Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _Target)
{

	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CTrooper* pInstance = new CTrooper(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}
	pInstance->Set_Target(_Target);

	return pInstance;
}
