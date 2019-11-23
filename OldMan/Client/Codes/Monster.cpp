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
// 근접공격 만들기 1. 때리기 2. 물어뜯기 
	m_bShot = m_pCondition->Get_Hit();
	Monster_Foward();
	

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

	m_fMaxRange = 15.0f;//최대사거리
	m_MonsterDir = { 0.f,0.f,0.f };
	m_fRange = 0.f;
	m_fMinRange = 3.0f;

	// 물리적 콜라이더
	m_pCollider->Set_Radius({ 2.f , 2.f, 2.f });			// 각 축에 해당하는 반지름을 설정
	m_pCollider->Set_Dynamic(false);						// 동적, 정적 Collider 유무
	m_pCollider->Set_Trigger(false);						// 트리거 유무
	m_pCollider->Set_CenterPos(m_pTransform->GetPos());		// Collider 의 정중앙좌표
	m_pCollider->Set_UnderPos();							// Collider 의 하단중앙 좌표
	m_pCollider->SetUp_Box();								// 설정된 것들을 Collider 에 반영합니다.
	m_pCollider->Set_Type(ENGINE::COLLISION_AABB);
	
	//리지드 바디 세팅 
	m_pRigid->Set_UseGravity(true);							// 중력의 영향 유무

	m_pRigid->Set_IsGround(false);							// 지상인지 체크
	m_pRigid->Set_IsAir(true);								// 공중인지 체크
	m_pRigid->Set_IsFall(true);								// 낙하중인지 체크
	m_pRigid->Set_IsJump(false);

	m_pRigid->Set_fMass(1.f);								// 물체의 무게
	m_pRigid->Set_fPower(10.f);								// 점프 파워

	m_pRigid->Set_Speed({ 10.f , 10.f , 10.f });				// 각 축에 해당하는 속도
	m_pRigid->Set_Accel({ 1.f, 0.f, 0.f });					// 각 축에 해당하는 Accel 값
	m_pRigid->Set_MaxAccel({ 2.f , 4.f , 2.f });			// 각 축에 해당하는 MaxAccel 값
	// 트리거 콜라이더 그라운드 
	
	m_pGroundChekCollider->Set_Radius({ 0.3f , 0.2f, 0.3f });
	m_pGroundChekCollider->Set_Dynamic(true);
	m_pGroundChekCollider->Set_Trigger(true);
	m_pGroundChekCollider->Set_CenterPos({ m_pTransform->GetPos().x ,
		m_pTransform->GetPos().y - m_pCollider->Get_Radius().y,
		m_pTransform->GetPos().z });
	m_pGroundChekCollider->Set_UnderPos();
	m_pGroundChekCollider->SetUp_Box();
			
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
	
	//컨디션 
	m_pCondition->Set_Hp(100.f);

	m_pCondition->Set_Dodge(false);         //회피 
	m_pCondition->Set_Cinematic(false);     // 연출 
	m_pCondition->Set_Hit(false);
	m_pCondition->Set_Attack(false);
	m_pCondition->Set_Invincible(false);  //무적
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
	//빌보드 
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

	// 그라운드 
	pComponent = ENGINE::CCollider::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"GCheck_Collider", pComponent });

	m_pGroundChekCollider = dynamic_cast<ENGINE::CCollider*>(pComponent);
	NULL_CHECK_RETURN(m_pGroundChekCollider, E_FAIL);
		

	return S_OK;
}


//정면 보는지 안보는지 하나 만들긴 해야됨 -> 각도에 따라서 렌더가 다르게 되야되기 때문이다. 
// 360/ 8 /45 간격으로 그림을 출력하게 만들어야 됨 사격이 되는 각도는 
// 4방향 에서는 사격이 가능해야 함 -> 45, 90,135,180 
// 뒤를 볼 경우 사격을 안한다 -> 인지범위 도달 했을시 뒤이면 앞으로 돌게 설정 
// 뒤일경우 데미지 더 받는 몬스터 하나 있음 
//도는 순서를 정해야 한다. -> 왼쪽 -> 오른쪽 판단후 돌게 만들면된다. 
// 
void CMonster::Player_Pursue()
{
	D3DXVECTOR3 vTempPos = dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // 플레이어위치
	D3DXVECTOR3 vMonsterPos = m_pTransform->GetPos();

	//플레이어의 방향 벡터
	D3DXVECTOR3 vMonster = vTempPos - vMonsterPos;
	D3DXVECTOR3 vMonster2 = {vMonster.x,m_pTransform->GetDir().y,vMonster.z };
	//좌우 방향벡터로 내적을 구하고  그것으로 도는 방향을 결정하는 값을 구해서 그쪽으로 돌게만드는게 핵심 
	
	//D3DXVECTOR3 vMonster2 = { vMonster.x,m_pTransform->GetPos().y,vMonster.z };     통통이-> 통통 튀면서 오는 경우 와이값이 들어가서 그런듯 
	m_MoveSpeed = 0.5f * m_pTimeMgr->GetDelta();   // 속도
	
	m_pTransform->Move_AdvancedPos(vMonster2, m_MoveSpeed);
	
		
}

void CMonster::Monster_Foward()
{
	D3DXVECTOR3 vTempPos = dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // 플레이어위치
	D3DXVECTOR3 vMonsterPos = m_pTransform->GetPos();

	D3DXVECTOR3 vMonster = vTempPos - vMonsterPos;
	D3DXVECTOR3 vMonsterDir_Fowrd = m_pTransform->GetDir();              // 몬스터가 보는 방향 

	D3DXVECTOR3 vMonsterDir_Fowrd2 = {1.f, 0.f,0.f };                // 몬스터의 룩 벡터 X축 기준 룩 벡터 

	D3DXVec3Normalize(&vMonster, &vMonster);
	D3DXVec3Normalize(&vMonsterDir_Fowrd, &vMonsterDir_Fowrd);
	D3DXVec3Normalize(&vMonsterDir_Fowrd2, &vMonsterDir_Fowrd2);
	float Dot2,test,test2;
	Dot2 = D3DXVec3Dot(&vMonster, &vMonsterDir_Fowrd2);

	float a = m_pTransform->GetAngle(ENGINE::ANGLE_X);
	//cout << a << endl;

//	cout << D3DXToDegree(acosf(Dot2)) << " 앞뒤" << endl;     // 앞뒤   0~90 도 앞 90~ 180 도 뒤 
															  // 왼, 오 만 구하면 충분히 완료  0~90 
	D3DXVECTOR3 Mon_RIght_Dir, Mon_Left_Dir, cross;
	D3DXVec3Cross(&Mon_RIght_Dir, &vMonsterDir_Fowrd2, &D3DXVECTOR3(0.0f, 1.0f, 0.0f)); // 우향 벡터를 구하기 위한 외적 
	//D3DXVec3Cross(&Mon_Left_Dir, &vMonsterDir_Fowrd2, &D3DXVECTOR3(0.0f, -1.0f, 0.0f)); // 좌향 벡터를 구하기 위한 외적 
	//D3DXVec3Cross(&cross, &vMonsterDir_Fowrd2, &vMonster);
	
	test = D3DXVec3Dot(&Mon_RIght_Dir, &vMonster);                 // - 일때 오른쪽 +왼쪽이다. 
	test2 = D3DXVec3Dot(&Mon_Left_Dir, &vMonster);
	
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
}
void CMonster::Monster_Shot()
{

	m_pTransform->MovePos(0.f);
	m_eNextState = MONSTER_PURSUE;

}
void CMonster::Monster_Fire()
{
	D3DXVECTOR3 vTempPos = dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // 플레이어위치
	D3DXVECTOR3 vMonsterPos = m_pTransform->GetPos();					// 몬스터 위치
	D3DXVECTOR3 vMonster =  vTempPos - vMonsterPos;						 // 방향 
	
	//정면이 되야 쏜다. 
	// 각도에 따른 렌더와 정면 사격 하게 만든다. 
	m_MoveSpeed = 300.f * m_pTimeMgr->GetDelta();   // 속도
	float fAngle[1] = { 0.f };

	CGameObject* pInstance = CBullet::Create(m_pGraphicDev, vMonsterPos, vMonster, fAngle, m_MoveSpeed,ENGINE::MONSTER_REVOLVER);
	m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::BULLET_MONSTER, pInstance);
	
	
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
	// 전부다 받아온다. 
	// 그래서 그 각도에 겹치는게 없는지 판단하는게 맞다. 

	m_pTarget


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
			//cout << "피" << endl;
			Monster_Shot();
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
