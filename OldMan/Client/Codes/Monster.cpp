#include "stdafx.h"
#include "Monster.h"
#include "Trasform.h"
#include "Collider.h"
#include "Billborad.h"
#include "CameraObserver.h"
#include "RigidBody.h"
#include "Bullet.h"


CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr), m_pCollider(nullptr),
	m_pSubject(ENGINE::GetCameraSubject()),
	m_pObserver(nullptr), m_pBillborad(nullptr), m_bShot(false),m_pRigid(nullptr)
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
	m_fTime += m_pTimeMgr->GetDelta();
	// ��ݰŸ� �ϳ� �����  ��ݰŸ� �����ϸ� ����ϰ� ����� �׵� �ٽ� �ּҰŸ����� ���� �� 

	m_bShot = m_pRigid->Get_IsHit();
	
	if (m_pRigid->Get_IsHit())
	{
		m_eNextState = MONSTER_SHOT;
		m_pRigid->Set_IsHit(false);
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
	


}

void CMonster::Render()
{

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matView);
	//m_pTexture->Render(0);
	m_pBuffer->Render();
}

HRESULT CMonster::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(10.f, 2.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(2.f, 2.f, 2.f));

	m_fMaxRange = 15.0f;//�ִ��Ÿ�
	m_MonsterDir = { 0.f,0.f,0.f };
	m_fRange = 0.f;
	m_fMinRange = 3.0f;

	// ������ �ݶ��̴�
	m_pCollider->Set_Radius({ 1.0f , 1.0f, 1.0f });			// �� �࿡ �ش��ϴ� �������� ����
	m_pCollider->Set_Dynamic(false);						// ����, ���� Collider ����
	m_pCollider->Set_Trigger(false);						// Ʈ���� ����
	m_pCollider->Set_CenterPos(m_pTransform->GetPos());		// Collider �� ���߾���ǥ
	m_pCollider->Set_UnderPos();							// Collider �� �ϴ��߾� ��ǥ
	m_pCollider->SetUp_Box();								// ������ �͵��� Collider �� �ݿ��մϴ�.

	m_pRigid->Set_IsHit(false);

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

	// Texture
	//pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, L"Texture_Player");
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent.insert({ L"Texture", pComponent });

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

	pComponent = ENGINE::CBillborad::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	
	m_pBillborad = dynamic_cast<ENGINE::CBillborad*>(pComponent);
	NULL_CHECK_RETURN(m_pCollider, E_FAIL);

	// Rigid
	pComponent = ENGINE::CRigidBody::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"RigidBody", pComponent });

	m_pRigid = dynamic_cast<ENGINE::CRigidBody*>(pComponent);
	NULL_CHECK_RETURN(m_pRigid, E_FAIL);

	return S_OK;
}

void CMonster::Player_Pursue()
{

	D3DXVECTOR3 vMonsterDir_Fowrd = m_pTransform->GetDir();				 // ���� ���⺤��
	 
	//�÷��̾��� ���� ����
	
	//�¿� ���⺤�ͷ� ������ ���ϰ�  �װ����� ���� ������ �����ϴ� ���� ���ؼ� �������� ���Ը���°� �ٽ� 
	

	float Monster_Dot = D3DXVec3Dot(&vMonsterDir_Fowrd, &m_MonsterDir);
	D3DXVec3Cross(&m_MonsterCroos, &m_MonsterDir,&vMonsterDir_Fowrd);
	// ����϶� ���� �����϶� ������ �̴�.
	//y ���� �ݿ��Ǹ� �ȴ�. 
	m_MoveSpeed = 1.f * m_pTimeMgr->GetDelta();   // �ӵ�

	
	
	m_pTransform->Move_AdvancedPos(m_MonsterDir, m_MoveSpeed);
	
		
		
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
void CMonster::Monster_Fire()
{
	D3DXVECTOR3 vTempPos = dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // �÷��̾���ġ

	D3DXVECTOR3 vMonsterPos = m_pTransform->GetPos();					// ���� ��ġ
	
	D3DXVECTOR3 vMonster =  vTempPos- vMonsterPos;
	
	D3DXVECTOR3 vMonsterDir_Fowrd = m_pTransform->GetDir();

	D3DXVECTOR3 vMonsterCroos2;
	D3DXVec3Normalize(&vMonsterDir_Fowrd, &vMonsterDir_Fowrd);
	D3DXVec3Normalize(&vMonster, &vMonster);

	float Dot; 
	float Radian; 
	float test;
	float test2;

	Dot = D3DXVec3Dot(&vMonster,&vMonsterDir_Fowrd);
	Radian = (float)acos(Dot);

	D3DXVECTOR3 Mon_RIght_Dir;
	D3DXVECTOR3 Mon_Left_Dir;

	D3DXVec3Cross(&Mon_RIght_Dir, &vMonsterDir_Fowrd, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	
	D3DXVec3Cross(&Mon_Left_Dir, &vMonsterDir_Fowrd, &D3DXVECTOR3(0.0f, -1.0f, 0.0f));

	test = D3DXVec3Dot(&Mon_RIght_Dir, &vMonster);
	test2 = D3DXVec3Dot(&Mon_Left_Dir, &vMonster);
	
	m_pTransform->SetAngle(D3DXToDegree((float)acos(test)), ENGINE::ANGLE_X);
	m_pTransform->SetAngle(D3DXToDegree((float)acos(Dot)), ENGINE::ANGLE_Y);
	float fAngle[3];

	fAngle[0] = 
	fAngle[1] = 
	fAngle[2] =0;
	//���� ������ ���簡 �������� ��� -? ��
	float fan2 = 57.f;

	CGameObject* pInstance = CBullet::Create(m_pGraphicDev, vMonsterPos, vMonster, fAngle);
	m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::BULLET, pInstance);

	cout << fAngle[1] << " ��" << endl;      // ���� 
	cout << D3DXToDegree((float)acos(test2)) << " ��" << endl;     // �¿� 
	cout << D3DXToDegree((float)acos(Dot)) << " ��" << endl;     // - �̸� �� +�̸� ���̴� 
	cout << vTempPos.x << " ��" << endl;


}
void CMonster::Monster_State_Set()
{
	if (m_eCurState != m_eNextState || m_eNextState == MONSTER_PURSUE)
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
		//	cout << "��" << endl;
			Monster_Idle();
			break;
		}
		m_eCurState = m_eNextState;
	}

	// �¾��� ��� Ž���� ������. 
	
}


	
// �����Ÿ��� ���� ������ ��ȯ ���� ���� 

//�ǰݽ� �Ѵ°� 





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
