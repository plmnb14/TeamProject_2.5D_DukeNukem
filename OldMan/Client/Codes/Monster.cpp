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
	Monster_Range();

	return NO_EVENT;
}

void CMonster::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();    
	D3DXMatrixIdentity(&m_matView);       
	
	D3DXMATRIX Localmatrix, Cameramatrix;													  //  로컬, 카메라 행렬 
	D3DXVECTOR3 vSize;																		  // 대상의 사이즈 
	Localmatrix = m_pTransform->GetWorldMatrix();
	Cameramatrix = m_pObserver->GetViewMatrix();
	vSize = m_pTransform->GetSize();

	m_pBillborad->Billborad_Front(Localmatrix, Cameramatrix,vSize);                          // 빌보드 설정
	m_matView = m_pBillborad->GetWorldMatrix_Billborad();                                    // 빌보드에서 설정된 행렬을 받아온다. 
	
	m_pCollider->LateUpdate(m_pTransform->GetPos());
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

	m_fMaxRange = 8.0f;//최대사거리
	m_MonsterDir = { 0.f,0.f,0.f };
	m_fRange = 0.f;
	m_fMinRange = 3.0f;

	// 물리적 콜라이더
	m_pCollider->Set_Radius({ 1.0f , 1.0f, 1.0f });			// 각 축에 해당하는 반지름을 설정
	m_pCollider->Set_Dynamic(false);						// 동적, 정적 Collider 유무
	m_pCollider->Set_Trigger(false);						// 트리거 유무
	m_pCollider->Set_CenterPos(m_pTransform->GetPos());		// Collider 의 정중앙좌표
	m_pCollider->Set_UnderPos();							// Collider 의 하단중앙 좌표
	m_pCollider->SetUp_Box();								// 설정된 것들을 Collider 에 반영합니다.

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
	NULL_CHECK_RETURN(m_pBillborad, E_FAIL);
	m_mapComponent.insert({ L"BillBoard", pComponent });

	return S_OK;
}

void CMonster::Player_Pursue()
{

	D3DXVECTOR3 vMonsterDir_Fowrd = m_pTransform->GetDir();				 // 전방 방향벡터
	 
	//플레이어의 방향 벡터
	
	//좌우 방향벡터로 내적을 구하고  그것으로 도는 방향을 결정하는 값을 구해서 그쪽으로 돌게만드는게 핵심 


	D3DXVec3Normalize(&vMonsterDir_Fowrd, &vMonsterDir_Fowrd);
	D3DXVec3Normalize(&m_MonsterDir, &m_MonsterDir);

	float Monster_Dot = D3DXVec3Dot(&vMonsterDir_Fowrd, &m_MonsterDir);
	D3DXVec3Cross(&m_MonsterCroos, &m_MonsterDir,&vMonsterDir_Fowrd);
	// 양수일때 왼쪽 음수일때 오른쪽 이다. y 값이 반영되면 된다. 
	m_MoveSpeed = 3.f * m_pTimeMgr->GetDelta();   // 속도

	
	m_pTransform->MoveAngle(ENGINE::ANGLE_Y, D3DXToDegree(m_MonsterCroos.y));
	m_pTransform->Move_AdvancedPos(m_MonsterDir, m_MoveSpeed);
	
		
		
}

void CMonster::Monster_Range()
{
	D3DXVECTOR3 vTempPos = dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // 플레이어위치
	D3DXVECTOR3 vMonsterPos = m_pTransform->GetPos();					// 몬스터 위치

	m_MonsterDir = vTempPos - vMonsterPos;
	m_fRange = D3DXVec3Length(&(vMonsterPos - vTempPos));				 // 사정거리
	
	if (m_fRange < m_fMaxRange && m_fRange >m_fMinRange)
	{
		Monster_State_Set(MONSTER_PURSUE);
	}
	else
	{
		Monster_State_Set(MONSTER_IDLE);
	}

}
void CMonster::Monster_Idle()
{
	m_pTransform->MovePos(0.f);

}
void CMonster::Monster_State_Set(Monster_State _state)
{
	switch (_state)
	{
	case MONSTER_IDLE:
		Monster_Idle();
		break;

	case MONSTER_PURSUE:
		Player_Pursue();
		break;
	}

}


	
// 사정거리에 따른 기계상태 전환 구현 시작 







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
