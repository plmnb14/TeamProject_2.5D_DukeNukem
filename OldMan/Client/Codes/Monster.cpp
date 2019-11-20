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
	m_pMelleCollider(nullptr),m_pCondition(nullptr)
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
// 근접공격 만들기 1. 때리기 2. 물어뜯기 
	m_bShot = m_pCondition->Get_Hit();
	
	

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
		//Monster_Range();

	}
	if (m_fTime > 1)
	{
		//	Monster_Fire();
		m_fTime = 0;
	}


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
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));

	m_fMaxRange = 15.0f;//최대사거리
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
	m_pCollider->Set_Type(ENGINE::COLLISION_AABB);

	m_pRigid->Set_IsHit(false);

	// 트리거 콜라이더     인식범위랑 비슷하게 필요하다 
	m_pMelleCollider->Set_Radius({ 1.3f , 1.2f, 1.3f });
	m_pMelleCollider->Set_Dynamic(true);
	m_pMelleCollider->Set_Trigger(true);
	m_pMelleCollider->Set_CenterPos({ m_pTransform->GetPos().x ,
		m_pTransform->GetPos().y - m_pCollider->Get_Radius().y,
		m_pTransform->GetPos().z });
	m_pMelleCollider->Set_UnderPos();
	m_pMelleCollider->SetUp_Box();
	//일단 트리거 설정은 되었지만 -> 의문점 충돌을 판정하지만 
	// 밀리 공격 상태는 어떤식으로 설정해야 하지? 
	// 1. 물리 공격의 사거리가 되었을때까지 이동한후 플레이어를 공격한다. 
	// 2. 여기서 공격은? 더 이동했다가 멈추는것을 말하는가?
	// 3. 그럼 피격시 플레이어가 뒤로 밀리는 것인가 ? 
	// 4.  다시 근접공격 가능한 거리까지 추적을 하는것 우선시 해야함 
	// 5. 근접 가능한 공격 사거리까지 도달했을때 

	//공중으로 안쫓게 해야한다. 

	m_pCondition->Set_Hit(false);
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
	// 컴포넌트 하나 추가 
	m_pCollider = dynamic_cast<ENGINE::CCollider*>(pComponent);
	NULL_CHECK_RETURN(m_pCollider, E_FAIL);

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




	return S_OK;
}

void CMonster::Player_Pursue()
{
	D3DXVECTOR3 vTempPos = dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // 플레이어위치

	D3DXVECTOR3 vMonsterDir_Fowrd = m_pTransform->GetDir();				 // 전방 방향벡터
	D3DXVECTOR3 vMonsterPos = m_pTransform->GetPos();
	//플레이어의 방향 벡터
	D3DXVECTOR3 vMonster = vTempPos - vMonsterPos;

	//좌우 방향벡터로 내적을 구하고  그것으로 도는 방향을 결정하는 값을 구해서 그쪽으로 돌게만드는게 핵심 
	

	float Monster_Dot = D3DXVec3Dot(&vMonsterDir_Fowrd, &m_MonsterDir);
	D3DXVec3Cross(&m_MonsterCroos, &m_MonsterDir,&vMonsterDir_Fowrd);
	// 양수일때 왼쪽 음수일때 오른쪽 이다.
	//y 값이 반영되면 된다. 
	m_MoveSpeed = 1.f * m_pTimeMgr->GetDelta();   // 속도
	
	m_pTransform->Move_AdvancedPos(vMonster, m_MoveSpeed);
	
		
}

void CMonster::Monster_Range()
{
	D3DXVECTOR3 vTempPos = dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // 플레이어위치
	D3DXVECTOR3 vMonsterPos = m_pTransform->GetPos();					// 몬스터 위치

	m_MonsterDir = vTempPos - vMonsterPos;
	m_fRange = D3DXVec3Length(&(vMonsterPos - vTempPos));				 // 사정거리
	
	if (m_fRange < m_fMaxRange && m_fRange >m_fMinRange)
	{
		m_eNextState =MONSTER_PURSUE;
	}
	else
	{
		m_eNextState = MONSTER_IDLE;
		
	}


	//사격상태 만들기 


}
void CMonster::Monster_Idle()
{
	m_pTransform->MovePos(0.f);
	//cout << "sadfsadf" << endl;
	m_eNextState = MONSTER_PURSUE;

}
void CMonster::Monster_Fire()
{
	D3DXVECTOR3 vTempPos = dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // 플레이어위치
	D3DXVECTOR3 vMonsterPos = m_pTransform->GetPos();					// 몬스터 위치
	D3DXVECTOR3 vMonster =  vTempPos - vMonsterPos;						 // 방향 
	
	D3DXVec3Normalize(&vMonster, &vMonster);
	
	//정면이 되야 쏜다. 
	// 각도에 따른 렌더와 정면 사격 하게 만든다. 
	m_MoveSpeed = 300.f * m_pTimeMgr->GetDelta();   // 속도
	float fAngle[1] = { 0.f };

	CGameObject* pInstance = CBullet::Create(m_pGraphicDev, vMonsterPos, vMonster, fAngle, m_MoveSpeed,ENGINE::MONSTER_REVOLVER);
	m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::BULLET, pInstance);
	
	
}
void CMonster::Monster_Bogan()
{
	D3DXVECTOR3 vTempPos = dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // 플레이어위치

	D3DXVECTOR3 vMonsterPos = m_pTransform->GetPos();					// 몬스터 위치

	D3DXVECTOR3 vMonster = vTempPos - vMonsterPos;						 // 방향 

	D3DXVECTOR3 vMonsterDir_Fowrd = m_pTransform->GetDir();              // 몬스터가 보는 방향 

	D3DXVECTOR3 vMonsterDir_Fowrd2 = { 0.f, 0.f,1.f };                // 몬스터의 룩 벡터 

	D3DXVECTOR3 vMonsterCroos2;
	D3DXVec3Normalize(&vMonsterDir_Fowrd, &vMonsterDir_Fowrd);
	D3DXVec3Normalize(&vMonster, &vMonster);
	D3DXVec3Normalize(&vMonsterDir_Fowrd2, &vMonsterDir_Fowrd2);

	float Dot, Dot2, test, test2;    //  외적, 내적을 구하기 위한 것들 정리 필요 

	Dot = D3DXVec3Dot(&vMonster, &vMonsterDir_Fowrd);
	Dot2 = D3DXVec3Dot(&vMonster, &vMonsterDir_Fowrd2);

	D3DXVECTOR3 Mon_RIght_Dir, Mon_Left_Dir, cross;

	D3DXVec3Cross(&Mon_RIght_Dir, &vMonsterDir_Fowrd, &D3DXVECTOR3(0.0f, 1.0f, 0.0f)); // 우향 벡터를 구하기 위한 외적 
	D3DXVec3Cross(&Mon_Left_Dir, &vMonsterDir_Fowrd, &D3DXVECTOR3(0.0f, -1.0f, 0.0f)); // 좌향 벡터를 구하기 위한 외적 
	D3DXVec3Cross(&cross, &vMonsterDir_Fowrd, &vMonster);

	test = D3DXVec3Dot(&Mon_RIght_Dir, &vMonster);
	test2 = D3DXVec3Dot(&Mon_Left_Dir, &vMonster);

	//정면이 되야 쏜다. 
	// 각도에 따른 렌더와 정면 사격 하게 만든다. 
	m_MoveSpeed = 1.f * m_pTimeMgr->GetDelta();   // 속도
	float fAngle[1] = { 0.f };

	CGameObject* pInstance = CBullet::Create(m_pGraphicDev, vMonsterPos, vMonster, fAngle, m_MoveSpeed, ENGINE::REVOLVER);
	m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::BULLET, pInstance);

	
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
	// << fAngle[1] << " 일" << endl;      // 우좌 
	cout << D3DXToDegree(acosf(Dot)) << " 앞뒤" << endl;     // 좌우 
	cout << D3DXToDegree(acosf(cross.y)) << " 좌우" << endl;     // - 이면 뒤 +이면 앞이다 
	cout << m_pTransform->GetAngle(ENGINE::ANGLE_Y) << " 사" << endl;


}

// 상태기계 오류 피격 당했을때 피격을 여러번 해버려서 문제가 생김 
void CMonster::Monster_State_Set()
{
	if (m_eCurState != m_eNextState || m_eNextState == MONSTER_SHOT || m_eNextState == MONSTER_PURSUE)
	{
		switch (m_eCurState)
		{
		case MONSTER_IDLE:
			//cout << "쉽" << endl;
			Monster_Idle();
			break;
		case MONSTER_PURSUE:
		//	cout << "추" << endl;
			Player_Pursue();
			break;
		case MONSTER_SHOT:
			cout << "피" << endl;
			Monster_Idle();
			break;
		}
		m_eCurState = m_eNextState;
	}

	// 맞았을 경우 탐색이 켜진다. 
	
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
