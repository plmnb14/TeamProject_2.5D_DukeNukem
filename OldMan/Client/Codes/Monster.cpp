#include "stdafx.h"
#include "Monster.h"
#include "Trasform.h"
#include "Collider.h"
#include "Billborad.h"
#include "CameraObserver.h"
CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr), m_pCollider(nullptr),
	m_pSubject(ENGINE::GetCameraSubject()),
	m_pObserver(nullptr), m_pBillborad(nullptr) 
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

	m_pCollider->Set_UnderPos(m_pTransform->GetPos());
	m_pCollider->SetUp_Box();
	
	cout << m_bShot << endl;
	// �ǰݽ� �ѹ� ����-> ������ �Ǵٽ� �ǰ� �� ���ƿ� �׳� �ѹ� �����ص��ޤ� �̰� 
	
	if (m_bShot)
	{
		m_eNextState = MONSTER_SHOT;
	}
	else
	{
		Monster_Range();
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

	m_pBillborad->Billborad_Yagnle(Localmatrix, Cameramatrix,vSize);                          // ������ ����
	m_matView = m_pBillborad->GetWorldMatrix_Billborad();                                    // �����忡�� ������ ����� �޾ƿ´�. 
	m_bShot = m_pCollider->Get_IsCollision();
	
	
	Monster_State_Set();
	//Monster_State_Set2();
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

	m_fMaxRange = 10.0f;//�ִ��Ÿ�
	m_MonsterDir = { 0.f,0.f,0.f };
	//m_fRange = 0.f;
	m_fMinRange = 5.0f;
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
	//bilil
	pComponent = ENGINE::CBillborad::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	
	m_pBillborad = dynamic_cast<ENGINE::CBillborad*>(pComponent);
	NULL_CHECK_RETURN(m_pCollider, E_FAIL);
	
	float Radius[3] = { 2.f , 2.f , 2.f };

	m_pCollider->Set_UnderPos(m_pTransform->GetPos());
	m_pCollider->Set_Radius({ 1.f , 1.f, 1.f });
	m_pCollider->Set_CenterPos();
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Trigger(false);
	m_pCollider->SetUp_Box();


	return S_OK;
}

void CMonster::Player_Pursue()
{

	D3DXVECTOR3 vMonsterDir_Fowrd = m_pTransform->GetDir();				 // ���� ���⺤��

	//�÷��̾��� ���� ����

	//�¿� ���⺤�ͷ� ������ ���ϰ�  �װ����� ���� ������ �����ϴ� ���� ���ؼ� �������� ���Ը���°� �ٽ� 


	D3DXVec3Normalize(&vMonsterDir_Fowrd, &vMonsterDir_Fowrd);
	D3DXVec3Normalize(&m_MonsterDir, &m_MonsterDir);

	float Monster_Dot = D3DXVec3Dot(&vMonsterDir_Fowrd, &m_MonsterDir);
	D3DXVec3Cross(&m_MonsterCroos, &m_MonsterDir, &vMonsterDir_Fowrd);
	// ����϶� ���� �����϶� ������ �̴�. y ���� �ݿ��Ǹ� �ȴ�. 
	 m_MoveSpeed = 3.f * m_pTimeMgr->GetDelta();

	m_pTransform->MoveAngle(ENGINE::ANGLE_Y, D3DXToDegree(m_MonsterCroos.y));
	m_pTransform->Move_AdvancedPos(m_MonsterDir, m_MoveSpeed);
	cout << m_fRange << endl;


}

// fsm  ���� ����� 

// �ֱ��� ���¸� �����Ͽ��ٰ� -> �� ���¸� �����ϰ� -> �� ���� ���°� ������ �װ����� ü���� �ϴ� ��?
void CMonster::Monster_Range()
{
	D3DXVECTOR3 vTempPos = dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // �÷��̾���ġ
	D3DXVECTOR3 vMonsterPos = m_pTransform->GetPos();					// ���� ��ġ

	m_MonsterDir = vTempPos - vMonsterPos;
	m_fRange = D3DXVec3Length(&(vMonsterPos - vTempPos));		
		
	if (m_fRange < m_fMaxRange && m_fRange>m_fMinRange)
	{
		m_eNextState = MONSTER_PURSUE;
	}
	else
	{
		m_eNextState = MONSTER_IDLE;

	}
	
}	
	
void CMonster::Monster_Idle()
{
	m_pTransform->MovePos(0.f);

}
void CMonster::Monster_Shot()
{
	m_pTransform->MovePos(0.f);

}
void CMonster::Object_Serch()
{
}
void CMonster::Monster_State_Set()
{
	if (m_eCurState != m_eNextState || m_eCurState == m_eCurState)
	{
		switch (m_eNextState)
		{
		case MONSTER_IDLE:
			cout << "��" << endl;
			Monster_Idle();
			break;
		case MONSTER_PURSUE:
			cout << "�߰�" << endl;
			Player_Pursue();// �Һ����� �ϳ� ���� ������ 
			break;
		case MONSTER_FIRE:
			cout << "���" << endl;
			break;
		case MONSTER_MILL:
			break;
		case MONSTER_SHOT:
			cout << "�ǰ�" << endl;
			break;
		default:
			break;

		}
		m_eCurState = m_eNextState;
	}

}

void CMonster::Monster_State_Set2()
{
	if (m_eCurState2 != m_eNextState2 || m_eCurState2 == m_eCurState2)
	{
		switch (m_eNextState2)
		{
		case MONSTER_SHOT:
			cout << "�ǰ�" << endl;
			break;
		case MONSTER_FIRE:
			cout << "���" << endl;
			break;
		case MONSTER_MILL:
			break;
		default:
			break;

		}
		m_eCurState2 = m_eNextState2;
	}




}


	
// �����Ÿ��� ���� ������ ��ȯ ���� ���� 







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
