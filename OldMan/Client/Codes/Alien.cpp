#include "stdafx.h"
#include "Alien.h"
#include "Trasform.h"
#include "Collider.h"
#include "Billborad.h"
#include "CameraObserver.h"
#include "RigidBody.h"
#include "Bullet.h"
#include "Condition.h"
#include "Animator.h"


CAlien::CAlien(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
{
}
CAlien::~CAlien()
{
	Release();
}

int CAlien::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;
	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();
	Check_Physic();
	Check_Push();
	// 근접공격 만들기 1. 때리기 2. 물어뜯기 
	//Monster_Foward();
	Monster_State_Set();
	//Player_Pursue(0.2f);
	//cout << m_pCondition->Get_Hp() << endl;
	//Monster_Jump();
	return NO_EVENT;
}

void CAlien::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
	D3DXMatrixIdentity(&m_matView);

	D3DXMATRIX Localmatrix, Cameramatrix;													  //  로컬, 카메라 행렬 
	D3DXVECTOR3 vSize;
	Localmatrix = m_pTransform->GetWorldMatrix();

	if (m_pObserver != nullptr)                                                                    //체크 
		Cameramatrix = m_pObserver->GetViewMatrix();

	vSize = m_pTransform->GetSize();

	m_pBillborad->Billborad_Yagnle(Localmatrix, Cameramatrix, vSize);                          // 빌보드 설정
	m_matView = m_pBillborad->GetWorldMatrix_Billborad();                                    // 빌보드에서 설정된 행렬을 받아온다. 

	m_bShot = m_pCondition->Get_Hit();

	m_pCollider->LateUpdate(m_pTransform->GetPos());
	m_pMelleCollider->LateUpdate(m_pTransform->GetPos());

	// 이러한 구조를 가지는 이유는 총격을 1순위 로 두기 때문이다. 피격시 모든 행동은 중지된다. 그리고 피격후 0.5 초후 범위탐색을 진행시킨다. 
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

			if (m_fRangeTiem > 0.1)
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

void CAlien::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matView);
	m_pAnimator->RenderSet(m_pTimeMgr->GetDelta());
	m_pTexture->Render(m_pAnimator->Get_Frame());

	m_pBuffer->Render();
}

HRESULT CAlien::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(5.f, 12.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(8.f, 8.f, 8.f));

	m_fMaxRange = 20.0f;//최대사거리
	m_fRange = 0.f;
	m_fMinRange = 15.f;
	
	m_fAttack = 5.0f;
	// 물리적 콜라이더
	m_pCollider->Set_Radius({ 2.f , 8.f, 2.f });			// 각 축에 해당하는 반지름을 설정
	m_pCollider->Set_Dynamic(true);						// 동적, 정적 Collider 유무
	m_pCollider->Set_Trigger(false);						// 트리거 유무
	m_pCollider->Set_CenterPos(m_pTransform->GetPos());		// Collider 의 정중앙좌표
	m_pCollider->Set_UnderPos();							// Collider 의 하단중앙 좌표
	m_pCollider->SetUp_Box();								// 설정된 것들을 Collider 에 반영합니다.
	m_pCollider->Set_Type(ENGINE::COLLISION_AABB);
//	m_pCollider->Set_MaxY();
	//리지드 바디 세팅 
	m_pRigid->Set_UseGravity(true);							// 중력의 영향 유무

	m_pRigid->Set_IsGround(false);							// 지상인지 체크
	m_pRigid->Set_IsAir(true);								// 공중인지 체크
	m_pRigid->Set_IsFall(true);								// 낙하중인지 체크
	m_pRigid->Set_IsJump(true);

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
	//점프 트리거 콜라이더 필요함
	// 트리거 콜라이더     인식범위랑 비슷하게 필요하다 
	m_pMelleCollider->Set_Radius({ 10.f , 10.f, 10.f });
	m_pMelleCollider->Set_Dynamic(true);
	m_pMelleCollider->Set_Trigger(true);
	m_pMelleCollider->Set_CenterPos({ m_pTransform->GetPos().x ,
		m_pTransform->GetPos().y ,
		m_pTransform->GetPos().z });
	m_pMelleCollider->Set_UnderPos();
	m_pMelleCollider->SetUp_Box();
	m_pMelleCollider->Set_Enabled(false);
	m_pMelleCollider->Set_Type(ENGINE::COLLISION_AABB);


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


	m_pAnimator->Set_FrameAmp(1.f);									// 배속재생
	m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_ZERO);
	m_pAnimator->Set_Reverse(false);
	m_pAnimator->Set_MaxFrame(0);
	m_pAnimator->Stop_Animation(false);

	m_fSizeX = 50.f;
	m_fSizeY = 50.f;

	m_fHitTime = 0;

	return S_OK;
}

HRESULT CAlien::LateInit()
{
	m_pObserver = CCameraObserver::Create();
	NULL_CHECK_RETURN(m_pObserver, E_FAIL);

	m_pSubject->Subscribe(m_pObserver);

	return S_OK;
}

void CAlien::Release()
{
	m_pSubject->UnSubscribe(m_pObserver);
	ENGINE::Safe_Delete(m_pObserver);
}

HRESULT CAlien::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;
	//texture
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, L"Alien_Idle");
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
	//빌보드 
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

	// 그라운드 
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


void CAlien::Player_Pursue(float _move)
{
	D3DXVECTOR3 vPlayer_Pos = static_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // 플레이어위치
	D3DXVECTOR3 vMonster_Pos = m_pTransform->GetPos();
	D3DXVECTOR3 vPlayer_Pos_Top = { vPlayer_Pos.x, vPlayer_Pos.y + 1,vPlayer_Pos.z };
	D3DXVECTOR3 vMonster_Player_Dir = vPlayer_Pos_Top - vMonster_Pos;

	//DXVECTOR3 vMonster2 = { vMonster.x,m_pTransform->GetPos().y,vMonster.z };  //   통통이-> 통통 튀면서 오는 경우 와이값이 들어가서 그런듯 
	m_MoveSpeed = 1.f* _move * m_pTimeMgr->GetDelta();   // 속도
	ChangeTex(L"Alien_RunFront");
	m_pAnimator->Set_FrameAmp(5.f);
	m_pTransform->Move_AdvancedPos(vMonster_Player_Dir, m_MoveSpeed);

}

void CAlien::Monster_Foward()
{

	//공격 진행하고 나면 공격 취소할수 없게 - 공격 속도는 좀 느리게 만들자 
	// 공격 딜레이 걸기 
	D3DXVECTOR3 vPlayer_Pos = static_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // 플레이어위치
	D3DXVECTOR3 vMonster_Pos = m_pTransform->GetPos();

	D3DXVECTOR3 vMonster_Player_Dir = vPlayer_Pos - vMonster_Pos;
	D3DXVECTOR3 vMonster_Player_Dir_Free = vPlayer_Pos - vMonster_Pos;

	D3DXVECTOR3 vMonsterDir_Get = m_pTransform->GetDir();              // 몬스터가 보는 방향 

	D3DXVECTOR3 vMonsterDir_Forward = { 1.f, 0.f,0.f };                // 몬스터의 룩 벡터 X축 기준 룩 벡터 
																	   //m_pTransform->SetDir(vMonster_Player_Dir);

	D3DXVec3Normalize(&vMonster_Player_Dir, &vMonster_Player_Dir);
	D3DXVec3Normalize(&vMonsterDir_Forward, &vMonsterDir_Forward);
	D3DXVec3Normalize(&vMonsterDir_Get, &vMonsterDir_Get);
	float fDot_Player_Monster_Forward, fDot_Monster_Right, fMonster_Get_Angle;
	fDot_Player_Monster_Forward = D3DXVec3Dot(&vMonster_Player_Dir, &vMonsterDir_Get);

	D3DXVECTOR3 vMonster_RIght_Dir, Mon_Left_Dir, cross;
	D3DXVec3Cross(&vMonster_RIght_Dir, &vMonsterDir_Forward, &D3DXVECTOR3(0.0f, 1.0f, 0.0f)); // 우향 벡터를 구하기 위한 외적 
																							  //D3DXVec3Cross(&Mon_Left_Dir, &vMonsterDir_Fowrd2, &D3DXVECTOR3(0.0f, -1.0f, 0.0f)); // 좌향 벡터를 구하기 위한 외적 

	fDot_Monster_Right = D3DXVec3Dot(&vMonster_RIght_Dir, &vMonster_Player_Dir);                 // - 일때 오른쪽 +왼쪽이다. 
	m_fFowardDealy += m_pTimeMgr->GetDelta();



}

void CAlien::Monster_Range()
{
	D3DXVECTOR3 vPlayer_Pos = static_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // 플레이어위치
	D3DXVECTOR3 vMonster_Pos = m_pTransform->GetPos();

	D3DXVECTOR3 vMonster_Player_Dir = vPlayer_Pos - vMonster_Pos;
	float  fRange, fJumpRange,fJumpTime;
	fRange = D3DXVec3Length(&(vMonster_Pos - vPlayer_Pos));				 // 사정거리
	fJumpTime = 0;
	fJumpRange = 12.f;
	if (fJumpTime == 0)
	{
		if (fRange < m_fMaxRange && fRange >m_fMinRange)
		{
			m_pRigid->Set_Accel({ 1, 1.5f, 1 });
			m_pRigid->Set_IsJump(true);
			m_pRigid->Set_IsGround(false);
			m_eNextState = MONSTER_JUMP;
			fJumpTime++;
		}
	}
	fJump += m_pTimeMgr->GetDelta();
	if (fJumpTime == 1) {
		if (fJump > 5)
		{
			fJumpTime = 0;

		}
	}

	if (fRange <m_fMinRange && fRange>fRange)
	{
		m_eNextState = MONSTER_PURSUE;

	}
	else if (fRange<m_fAttack)
	{
		m_eNextState = MONSTER_MILL;

	}
}
void CAlien::Monster_Idle()
{
	m_pTransform->MovePos(0.f);
}
void CAlien::Monster_Shot()
{

	//딜레이 만들기 
	m_pTransform->MovePos(0.f);
	ChangeTex(L"Alien_Dead");
	m_pAnimator->Set_Frame(0.f);
	m_pAnimator->Stop_Animation(true);
	//m_pCondition->Add_Hp(-1);

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


void CAlien::Monster_Fire2()
{
	Monster_Callcul();
	D3DXVECTOR3 vPlayer_Pos = static_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();
	D3DXVECTOR3 vMonster_Pos = m_pTransform->GetPos();
	D3DXVECTOR3 vPlayer_Pos_Top = { vPlayer_Pos.x, vPlayer_Pos.y + 1,vPlayer_Pos.z };
	D3DXVECTOR3 vMonster_Player_Dir = vPlayer_Pos_Top - vMonster_Pos;
	D3DXVECTOR3 vMonsterDir_Fowrd_Get = m_pTransform->GetDir();
	D3DXVECTOR3 vMonsterDir_Forward = { 1.f, 0.f,0.f };                // 몬스터의 룩 벡터 X축 기준 룩 벡터 

	D3DXVECTOR3 vMonster_RIght_Dir, Mon_Left_Dir, cross;
	D3DXVec3Normalize(&vMonsterDir_Forward, &vMonsterDir_Forward);
	D3DXVec3Normalize(&vMonsterDir_Fowrd_Get, &vMonsterDir_Fowrd_Get);
	D3DXVec3Normalize(&vMonster_Player_Dir, &vMonster_Player_Dir);

	D3DXVec3Cross(&vMonster_RIght_Dir, &vMonster_Player_Dir, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));        //우향벡터를 구하기 위한 식 

	D3DXVECTOR3 vMonsterPos_ShotPoint = { vMonster_Pos.x - vMonster_RIght_Dir.x, vMonster_Pos.y - vMonster_RIght_Dir.y  ,vMonster_Pos.z - vMonster_RIght_Dir.z };
	D3DXVECTOR3 vMonster = vPlayer_Pos_Top - vMonsterPos_ShotPoint;

	float fMove;
	fMove = 100.f * m_pTimeMgr->GetDelta();			 // 속도
	float fAngle[3] = { 0.f };                      //각도 0도로 줘야함 안주면 총알 빌보드 문제 있음 
	m_fTime += m_pTimeMgr->GetDelta();

	float fDot_Player_Monster_Forward;                        // 몬스터의 정면을 구하기 위한 내적값 -> +정면 -후면 이다. 
	fDot_Player_Monster_Forward = D3DXVec3Dot(&vMonster_Player_Dir, &vMonsterDir_Fowrd_Get);

	m_pAnimator->Stop_Animation(false);

	ChangeTex(L"Alien_Idle");
	m_pAnimator->Set_FrameAmp(0.5f);

	//cout << fDot_Player_Monster_Forward << endl;

}

void CAlien::Monster_Dead()
{
	m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_STOP);
	ChangeTex(L"Alien_Dead");
	m_pAnimator->Set_FrameAmp(1.f);
	m_pAnimator->Set_Frame(9.f);

	m_fDeadTimer += m_pTimeMgr->GetDelta();
	if (m_fDeadTimer > 3)
	{
		m_bIsDead = true;
	}

}

void CAlien::Monster_Attack()
{
	Monster_Callcul();

	D3DXVECTOR3 vPlayer_Pos = static_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();
	D3DXVECTOR3 vMonster_Pos = m_pTransform->GetPos();
	D3DXVECTOR3 vPlayer_Pos_Top = { vPlayer_Pos.x, vPlayer_Pos.y + 1,vPlayer_Pos.z };
	D3DXVECTOR3 vMonster_Player_Dir = vPlayer_Pos_Top - vMonster_Pos;
	float fMove;
	fMove = 100.f * m_pTimeMgr->GetDelta();
	m_pMelleCollider->Set_Enabled(true);
	//텍스처 입히기 

	if (m_pMelleCollider->Get_IsCollision())
	{
		m_pCondition->Add_Hp(+1);
	}




}
void CAlien::Set_Pos(D3DXVECTOR3 _Pos)
{
	m_pTransform->SetPos(_Pos);
}

void CAlien::Check_Physic()
{
	Monster_Callcul();
	float fMove;
	fMove = 1.f * m_pTimeMgr->GetDelta();

	if (m_pRigid->Get_IsJump() == true)
	{
		D3DXVECTOR3 JumpLength = { 0, m_pRigid->Set_Jump(m_pTransform->GetPos(), m_pTimeMgr->GetDelta()) , 0 };
		m_pTransform->Move_AdvancedPos_Vec3(JumpLength);
		m_pTransform->Move_AdvancedPos(m_vMonster_Player_Dir, fMove);
		ChangeTex(L"Alien_Jump");
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
			return;
		}

		D3DXVECTOR3 JumpLength = { 0, -m_pRigid->Set_Fall(m_pTransform->GetPos(), m_pTimeMgr->GetDelta()),0 };
		m_pTransform->Move_AdvancedPos_Vec3(JumpLength);
	}

}

void CAlien::Object_Collison()
{
	// 전부다 받아온다. 
	// 그래서 그 각도에 겹치는게 없는지 판단하는게 맞다. 
	// 벽 판단하기 
	// 텍스처 넣기 


}

void CAlien::Monster_Jump()
{
	Monster_Callcul();

	
	cout << "걸린다" << endl;
	if (m_pRigid->Get_IsJump() == true)
	{
		m_JumpTiem += m_pTimeMgr->GetDelta();
		ChangeTex(L"Alien_Jump");
		m_pAnimator->Set_FrameAmp(1.f);
		if (m_JumpTiem > 2.f)
		{
			float fMove;
			fMove = 1.f * m_pTimeMgr->GetDelta();
			m_pTransform->Move_AdvancedPos(m_vMonster_Player_Dir, fMove);
			D3DXVECTOR3 JumpLength = { 0,  m_pRigid->Set_Fall(m_pTransform->GetPos(), m_pTimeMgr->GetDelta()) * 5,0 };
			m_pTransform->Move_AdvancedPos_Vec3(JumpLength);
			m_JumpTiem = 0;
			m_eNextState = MONSTER_PURSUE;
		}
		if (m_pRigid->Get_Accel().y <= 0.f)
		{
			m_pRigid->Set_Accel({ 1,0,1 });
			m_pRigid->Set_IsFall(true);
			m_pRigid->Set_IsJump(false);

		}
	}

}


// 상태기계 오류 피격 당했을때 피격을 여러번 해버려서 문제가 생김 
void CAlien::Monster_State_Set()
{

	switch (m_eNextState)
	{
	case MONSTER_IDLE:
		Monster_Idle();
		break;
	case MONSTER_PURSUE:
		Player_Pursue(0.7f);
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
	case MONSTER_JUMP:
		//Monster_Jump();
		break;
	}


}

CAlien * CAlien::Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _Target, D3DXVECTOR3 _Pos)
{

	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CAlien* pInstance = new CAlien(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}
	pInstance->Set_Target(_Target);
	pInstance->Set_Pos(_Pos);

	return pInstance;
}
