#include "stdafx.h"
#include "Monster.h"
#include "Trasform.h"
#include "Collider.h"
#include "Billborad.h"
#include "CameraObserver.h"
#include "RigidBody.h"
#include "Bullet.h"
#include "Condition.h"


CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr), m_pCollider(nullptr),
	m_pSubject(ENGINE::GetCameraSubject()),
	m_pObserver(nullptr), m_pBillborad(nullptr), m_bShot(false),m_pRigid(nullptr),
	m_pMelleCollider(nullptr),m_pCondition(nullptr), m_pGroundChekCollider(nullptr)
	{
	}


CMonster::~CMonster()
{
	Release();
}

int CMonster::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();
	//Player_Pursue();
	Check_Physic();
	m_fTime += m_pTimeMgr->GetDelta();
// �������� ����� 1. ������ 2. ������ 
	m_bShot = m_pCondition->Get_Hit();
	Monster_Foward();
	

	return NO_EVENT;
}

void CMonster::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();    
	D3DXMatrixIdentity(&m_matView);       
	
	D3DXMATRIX Localmatrix, Cameramatrix;													  //  ����, ī�޶� ��� 
	D3DXVECTOR3 vSize;																		  // ����� ������ 
	Localmatrix = m_pTransform->GetWorldMatrix();
	Cameramatrix = m_pObserver->GetViewMatrix();
	vSize = m_pTransform->GetSize();

	m_pBillborad->Billborad_Front(Localmatrix, Cameramatrix,vSize);                          // ������ ����
	m_matView = m_pBillborad->GetWorldMatrix_Billborad();                                    // �����忡�� ������ ����� �޾ƿ´�. 
	//m_matView = Localmatrix;

	m_pCollider->LateUpdate(m_pTransform->GetPos());

	Monster_State_Set();

	if (m_pCondition->Get_Hit())
	{
		m_eNextState = MONSTER_SHOT;
		m_pCondition->Set_Hit(false);

	}
	else
	{
		Monster_Range();

	}
	if (m_fTime > 1)
	{
		Monster_Fire();
		m_fTime = 0;
	}

	m_pGroundChekCollider->LateUpdate({ m_pTransform->GetPos().x ,
		m_pTransform->GetPos().y - m_pCollider->Get_Radius().y,
		m_pTransform->GetPos().z });

}

void CMonster::Render()
{

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matView);
	m_pTexture->Render((int)m_fFrame);
	m_pBuffer->Render();
}

HRESULT CMonster::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(5.f, 10.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(2.f, 2.f, 2.f));

	m_fMaxRange = 15.0f;//�ִ��Ÿ�
	m_MonsterDir = { 0.f,0.f,0.f };
	m_fRange = 0.f;
	m_fMinRange = 3.0f;

	// ������ �ݶ��̴�
	m_pCollider->Set_Radius({ 2.f , 2.f, 2.f });			// �� �࿡ �ش��ϴ� �������� ����
	m_pCollider->Set_Dynamic(false);						// ����, ���� Collider ����
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
	m_pMelleCollider->Set_Radius({ 1.3f , 1.2f, 1.3f });
	m_pMelleCollider->Set_Dynamic(true);
	m_pMelleCollider->Set_Trigger(true);
	m_pMelleCollider->Set_CenterPos({ m_pTransform->GetPos().x ,
		m_pTransform->GetPos().y - m_pCollider->Get_Radius().y,
		m_pTransform->GetPos().z });
	m_pMelleCollider->Set_UnderPos();
	m_pMelleCollider->SetUp_Box();
	//�ϴ� Ʈ���� ������ �Ǿ����� -> �ǹ��� �浹�� ���������� 
	// �и� ���� ���´� ������� �����ؾ� ����? 
	// 1. ���� ������ ��Ÿ��� �Ǿ��������� �̵����� �÷��̾ �����Ѵ�. 
	// 2. ���⼭ ������? �� �̵��ߴٰ� ���ߴ°��� ���ϴ°�?
	// 3. �׷� �ǰݽ� �÷��̾ �ڷ� �и��� ���ΰ� ? 
	// 4.  �ٽ� �������� ������ �Ÿ����� ������ �ϴ°� �켱�� �ؾ��� 
	// 5. ���� ������ ���� ��Ÿ����� ���������� 

	//�������� ���Ѱ� �ؾ��Ѵ�. 
	
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
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_DYNAMIC, L"PigMan_Walk_Front");
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Texture", pComponent });

	m_pTexture = dynamic_cast<ENGINE::CTexture*>(pComponent);
	NULL_CHECK_RETURN(m_pTexture, E_FAIL);

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
	//������ 
	pComponent = ENGINE::CBillborad::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	
	m_pBillborad = dynamic_cast<ENGINE::CBillborad*>(pComponent);
	NULL_CHECK_RETURN(m_pBillborad, E_FAIL);
	m_mapComponent.insert({ L"BillBoard", pComponent });

	// Rigid
	pComponent = ENGINE::CRigidBody::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"RigidBody", pComponent });

	m_pRigid = dynamic_cast<ENGINE::CRigidBody*>(pComponent);
	NULL_CHECK_RETURN(m_pRigid, E_FAIL);
	// MEELE
	pComponent = ENGINE::CCollider::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Monster_Mell", pComponent });
	
	m_pMelleCollider = dynamic_cast<ENGINE::CCollider*>(pComponent);
	NULL_CHECK_RETURN(m_pCollider, E_FAIL);

	// conditoin  
	pComponent = ENGINE::CCondition::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Condition", pComponent });

	m_pCondition = dynamic_cast<ENGINE::CCondition*>(pComponent);
	NULL_CHECK_RETURN(m_pCondition, E_FAIL);

	// �׶��� 
	pComponent = ENGINE::CCollider::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"GCheck_Collider", pComponent });

	m_pGroundChekCollider = dynamic_cast<ENGINE::CCollider*>(pComponent);
	NULL_CHECK_RETURN(m_pGroundChekCollider, E_FAIL);
		

	return S_OK;
}


//���� ������ �Ⱥ����� �ϳ� ����� �ؾߵ� -> ������ ���� ������ �ٸ��� �ǾߵǱ� �����̴�. 
// 360/ 8 /45 �������� �׸��� ����ϰ� ������ �� ����� �Ǵ� ������ 
// 4���� ������ ����� �����ؾ� �� -> 45, 90,135,180 
// �ڸ� �� ��� ����� ���Ѵ� -> �������� ���� ������ ���̸� ������ ���� ���� 
// ���ϰ�� ������ �� �޴� ���� �ϳ� ���� 
//���� ������ ���ؾ� �Ѵ�. -> ���� -> ������ �Ǵ��� ���� �����ȴ�. 
// 
void CMonster::Player_Pursue()
{
	D3DXVECTOR3 vTempPos = dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // �÷��̾���ġ
	D3DXVECTOR3 vMonsterPos = m_pTransform->GetPos();

	//�÷��̾��� ���� ����
	D3DXVECTOR3 vMonster = vTempPos - vMonsterPos;
	D3DXVECTOR3 vMonster2 = {vMonster.x,m_pTransform->GetDir().y,vMonster.z };
	//�¿� ���⺤�ͷ� ������ ���ϰ�  �װ����� ���� ������ �����ϴ� ���� ���ؼ� �������� ���Ը���°� �ٽ� 
	
	//D3DXVECTOR3 vMonster2 = { vMonster.x,m_pTransform->GetPos().y,vMonster.z };     ������-> ���� Ƣ�鼭 ���� ��� ���̰��� ���� �׷��� 
	m_MoveSpeed = 0.5f * m_pTimeMgr->GetDelta();   // �ӵ�
	
	m_pTransform->Move_AdvancedPos(vMonster2, m_MoveSpeed);
	
		
}

void CMonster::Monster_Foward()
{
	D3DXVECTOR3 vTempPos = dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // �÷��̾���ġ
	D3DXVECTOR3 vMonsterPos = m_pTransform->GetPos();

	D3DXVECTOR3 vMonster = vTempPos - vMonsterPos;
	D3DXVECTOR3 vMonsterDir_Fowrd = m_pTransform->GetDir();              // ���Ͱ� ���� ���� 

	D3DXVECTOR3 vMonsterDir_Fowrd2 = {1.f, 0.f,0.f };                // ������ �� ���� X�� ���� �� ���� 

	D3DXVec3Normalize(&vMonster, &vMonster);
	D3DXVec3Normalize(&vMonsterDir_Fowrd, &vMonsterDir_Fowrd);
	D3DXVec3Normalize(&vMonsterDir_Fowrd2, &vMonsterDir_Fowrd2);
	float Dot2,test,test2;
	Dot2 = D3DXVec3Dot(&vMonster, &vMonsterDir_Fowrd2);

	float a = m_pTransform->GetAngle(ENGINE::ANGLE_X);
	//cout << a << endl;

//	cout << D3DXToDegree(acosf(Dot2)) << " �յ�" << endl;     // �յ�   0~90 �� �� 90~ 180 �� �� 
															  // ��, �� �� ���ϸ� ����� �Ϸ�  0~90 
	D3DXVECTOR3 Mon_RIght_Dir, Mon_Left_Dir, cross;
	D3DXVec3Cross(&Mon_RIght_Dir, &vMonsterDir_Fowrd2, &D3DXVECTOR3(0.0f, 1.0f, 0.0f)); // ���� ���͸� ���ϱ� ���� ���� 
	//D3DXVec3Cross(&Mon_Left_Dir, &vMonsterDir_Fowrd2, &D3DXVECTOR3(0.0f, -1.0f, 0.0f)); // ���� ���͸� ���ϱ� ���� ���� 
	//D3DXVec3Cross(&cross, &vMonsterDir_Fowrd2, &vMonster);
	
	test = D3DXVec3Dot(&Mon_RIght_Dir, &vMonster);                 // - �϶� ������ +�����̴�. 
	test2 = D3DXVec3Dot(&Mon_Left_Dir, &vMonster);
	
}

void CMonster::Monster_Range()
{
	D3DXVECTOR3 vTempPos = dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // �÷��̾���ġ
	D3DXVECTOR3 vMonsterPos = m_pTransform->GetPos();					// ���� ��ġ

	m_MonsterDir = vTempPos - vMonsterPos;
	m_fRange = D3DXVec3Length(&(vMonsterPos - vTempPos));				 // �����Ÿ�
	
	if (m_fRange < m_fMaxRange && m_fRange >m_fMinRange)
	{
		m_eNextState =MONSTER_PURSUE;
	}
	else
	{
		m_eNextState = MONSTER_IDLE;
		
	}

	//��ݻ��� ����� 


}
void CMonster::Monster_Idle()
{
	m_pTransform->MovePos(0.f);
}
void CMonster::Monster_Shot()
{

	m_pTransform->MovePos(0.f);
	m_eNextState = MONSTER_PURSUE;

}
void CMonster::Monster_Fire()
{
	D3DXVECTOR3 vTempPos = dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // �÷��̾���ġ
	D3DXVECTOR3 vMonsterPos = m_pTransform->GetPos();					// ���� ��ġ
	D3DXVECTOR3 vMonster =  vTempPos - vMonsterPos;						 // ���� 
	
	//������ �Ǿ� ���. 
	// ������ ���� ������ ���� ��� �ϰ� �����. 
	m_MoveSpeed = 300.f * m_pTimeMgr->GetDelta();   // �ӵ�
	float fAngle[1] = { 0.f };

	CGameObject* pInstance = CBullet::Create(m_pGraphicDev, vMonsterPos, vMonster, fAngle, m_MoveSpeed,ENGINE::MONSTER_REVOLVER);
	m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::BULLET_MONSTER, pInstance);
	
	
}
void CMonster::Monster_Bogan()
{
	D3DXVECTOR3 vTempPos = dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // �÷��̾���ġ

	D3DXVECTOR3 vMonsterPos = m_pTransform->GetPos();					// ���� ��ġ

	D3DXVECTOR3 vMonster = vTempPos - vMonsterPos;						 // ���� 

	D3DXVECTOR3 vMonsterDir_Fowrd = m_pTransform->GetDir();              // ���Ͱ� ���� ���� 

	D3DXVECTOR3 vMonsterDir_Fowrd2 = { 0.f, 0.f,1.f };                // ������ �� ���� 

	D3DXVECTOR3 vMonsterCroos2;
	D3DXVec3Normalize(&vMonsterDir_Fowrd, &vMonsterDir_Fowrd);
	D3DXVec3Normalize(&vMonster, &vMonster);
	D3DXVec3Normalize(&vMonsterDir_Fowrd2, &vMonsterDir_Fowrd2);

	float Dot, Dot2, test, test2;    //  ����, ������ ���ϱ� ���� �͵� ���� �ʿ� 

	Dot = D3DXVec3Dot(&vMonster, &vMonsterDir_Fowrd);
	Dot2 = D3DXVec3Dot(&vMonster, &vMonsterDir_Fowrd2);

	D3DXVECTOR3 Mon_RIght_Dir, Mon_Left_Dir, cross;

	D3DXVec3Cross(&Mon_RIght_Dir, &vMonsterDir_Fowrd, &D3DXVECTOR3(0.0f, 1.0f, 0.0f)); // ���� ���͸� ���ϱ� ���� ���� 
	D3DXVec3Cross(&Mon_Left_Dir, &vMonsterDir_Fowrd, &D3DXVECTOR3(0.0f, -1.0f, 0.0f)); // ���� ���͸� ���ϱ� ���� ���� 
	D3DXVec3Cross(&cross, &vMonsterDir_Fowrd, &vMonster);

	test = D3DXVec3Dot(&Mon_RIght_Dir, &vMonster);
	test2 = D3DXVec3Dot(&Mon_Left_Dir, &vMonster);
	
	//������ �Ǿ� ���. 
	// ������ ���� ������ ���� ��� �ϰ� �����. 
	m_MoveSpeed = 1.f * m_pTimeMgr->GetDelta();   // �ӵ�
	float fAngle[1] = { 0.f };

	CGameObject* pInstance = CBullet::Create(m_pGraphicDev, vMonsterPos, vMonster, fAngle, m_MoveSpeed, ENGINE::REVOLVER);
	m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::BULLET_MONSTER, pInstance);

	
	m_pTransform->SetAngle((float)45.f, ENGINE::ANGLE_Y);
	m_pTransform->MoveAngle(ENGINE::ANGLE_Y, m_MoveSpeed);
	float fShotDirTemp = D3DXToDegree((float)acosf(Dot2));
	if (vMonster.x < 0.f)
		fShotDirTemp = 360.f - fShotDirTemp;

	if (D3DXToDegree(acosf(Dot))<45)
	{
		fAngle[0] = 0.f;
		fAngle[1] = fShotDirTemp;
		fAngle[2] = 0.f;
	}


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
			//	dynamic_cast<CCamera*>(m_pCamera)->Set_CamShakePos(vTemp);
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

	m_pTarget


}

// ���±�� ���� �ǰ� �������� �ǰ��� ������ �ع����� ������ ���� 
void CMonster::Monster_State_Set()
{
	if (m_eCurState != m_eNextState || m_eNextState == MONSTER_SHOT || m_eNextState == MONSTER_PURSUE)
	{
		switch (m_eCurState)
		{
		case MONSTER_IDLE:
			//cout << "��" << endl;
			Monster_Idle();
			break;
		case MONSTER_PURSUE:
		//	cout << "��" << endl;
			Player_Pursue();
			break;
		case MONSTER_SHOT:
			//cout << "��" << endl;
			Monster_Shot();
			break;
		}
		m_eCurState = m_eNextState;
	}

	// �¾��� ��� Ž���� ������. 
	
}


CMonster * CMonster::Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _Target)
{

	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CMonster* pInstance = new CMonster(pGraphicDev);
	
	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}
	pInstance->Set_Target(_Target);

	return pInstance;
}
