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
	, m_fSizeY(0), m_fSizeX(0), m_fFrame(0), m_bAttack(false)
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
	// 근접공격 만들기 1. 때리기 2. 물어뜯기 
	//Monster_Foward();
	
	Monster_State_Set();
	
	//Player_Pursue();
	//cout << m_pCondition->Get_Hp() << endl;
	


	return NO_EVENT;
}

void CMonster::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
	D3DXMatrixIdentity(&m_matView);

	D3DXMATRIX Localmatrix, Cameramatrix;													  //  로컬, 카메라 행렬 
	D3DXVECTOR3 vSize;																		  // 대상의 사이즈 
	Localmatrix = m_pTransform->GetWorldMatrix();

	if(m_pObserver != nullptr)
		Cameramatrix = m_pObserver->GetViewMatrix();

	vSize = m_pTransform->GetSize();

	m_pBillborad->Billborad_Yagnle(Localmatrix, Cameramatrix, vSize);                          // 빌보드 설정
	m_matView = m_pBillborad->GetWorldMatrix_Billborad();                                    // 빌보드에서 설정된 행렬을 받아온다. 

	m_bShot = m_pCondition->Get_Hit();

	m_pCollider->LateUpdate(m_pTransform->GetPos());
	m_pMelleCollider->LateUpdate(m_pTransform->GetPos());

	//cout << m_pCondition->Get_Hp() << endl;

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

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 10.f, 8.f));
	m_pTransform->SetSize(D3DXVECTOR3(4.f, 4.f, 4.f));

	m_fMaxRange = 19.0f;//최대사거리
	m_fRange = 0.f;
	m_fMinRange = 11.0f;
	m_fAttack = 5.0f;
	// 물리적 콜라이더
	m_pCollider->Set_Radius({ 2.f , 4.f, 2.f });			// 각 축에 해당하는 반지름을 설정
	m_pCollider->Set_Dynamic(true);						// 동적, 정적 Collider 유무
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
	NULL_CHECK_RETURN(m_pCollider, E_FAIL);

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


//정면 보는지 안보는지 하나 만들긴 해야됨 -> 각도에 따라서 렌더가 다르게 되야되기 때문이다. 
// 360/ 8 /45 간격으로 그림을 출력하게 만들어야 됨 사격이 되는 각도는 
// 4방향 에서는 사격이 가능해야 함 -> 45, 90,135,180 
// 뒤를 볼 경우 사격을 안한다 -> 인지범위 도달 했을시 뒤이면 앞으로 돌게 설정 
// 뒤일경우 데미지 더 받는 몬스터 하나 있음 
//도는 순서를 정해야 한다. -> 왼쪽 -> 오른쪽 판단후 돌게 만들면된다. 
// 
void CMonster::Player_Pursue(float _move)
{
	D3DXVECTOR3 vPlayer_Pos = static_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // 플레이어위치
	D3DXVECTOR3 vMonster_Pos = m_pTransform->GetPos();
	D3DXVECTOR3 vPlayer_Pos_Top = { vPlayer_Pos.x, vPlayer_Pos.y + 1,vPlayer_Pos.z };
	D3DXVECTOR3 vMonster_Player_Dir = vPlayer_Pos_Top - vMonster_Pos;
	//좌우 방향벡터로 내적을 구하고  그것으로 도는 방향을 결정하는 값을 구해서 그쪽으로 돌게만드는게 핵심 
	// 플레이어의 중점을 받아서 가면 몬스터가 점점 가라앉는 문제가 생겨서 top으로 위치를 다르게 줌 

	//DXVECTOR3 vMonster2 = { vMonster.x,m_pTransform->GetPos().y,vMonster.z };  //   통통이-> 통통 튀면서 오는 경우 와이값이 들어가서 그런듯 
	m_MoveSpeed = 2.f* _move * m_pTimeMgr->GetDelta();   // 속도

	m_pTransform->Move_AdvancedPos(vMonster_Player_Dir, m_MoveSpeed);

}

void CMonster::Monster_Foward()
{

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

	if (m_fFowardDealy > 0.1) {
		if (acos(fDot_Player_Monster_Forward) * 90 < 250)
		{
			m_pAnimator->Stop_Animation(false);
			ChangeTex(L"PigMan_WalkFront");
			m_pAnimator->Set_FrameAmp(5.f);
			m_fFowardDealy = 0;
			Player_Pursue(1.f);
		}
		else if (acos(fDot_Player_Monster_Forward) * 90 > 250)
		{
			m_pAnimator->Stop_Animation(false);
			ChangeTex(L"PigMan_WalkBack");
			m_pAnimator->Set_FrameAmp(5.f);
			m_pTransform->MoveAngle(ENGINE::ANGLE_Y, 10.f);
			Player_Pursue(0.f);
			m_fFowardDealy = 0;
		}
	}

}

void CMonster::Monster_Range()
{
	D3DXVECTOR3 vPlayer_Pos = static_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // 플레이어위치
	D3DXVECTOR3 vMonster_Pos = m_pTransform->GetPos();

	D3DXVECTOR3 vMonster_Player_Dir = vPlayer_Pos - vMonster_Pos;
	float  fRange;
	fRange = D3DXVec3Length(&(vMonster_Pos - vPlayer_Pos));				 // 사정거리

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
void CMonster::Monster_Idle()
{
	m_pTransform->MovePos(0.f);
}
void CMonster::Monster_Shot()
{
	D3DXVECTOR3 vPlayer_Pos = static_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();  // 플레이어위치
	D3DXVECTOR3 vMonster_Pos = m_pTransform->GetPos();
	D3DXVECTOR3 vPlayer_Pos_Top = { vPlayer_Pos.x, vPlayer_Pos.y + 1,vPlayer_Pos.z };
	D3DXVECTOR3 vPlayer_Pos_Top_Top = { vPlayer_Pos.x, vPlayer_Pos.y + 15,vPlayer_Pos.z };
	D3DXVECTOR3 vMonster_Player_Dir = vPlayer_Pos_Top - vMonster_Pos;
	D3DXVECTOR3 vMonster_Dir_Top = vPlayer_Pos_Top_Top - vMonster_Pos;
	D3DXVECTOR3 vMonsterDir_Forward = { 1.f, 0.f,0.f };                // 몬스터의 룩 벡터 X축 기준 룩 벡터 

	D3DXVECTOR3 vMonsterDir_Get = m_pTransform->GetDir();                // 몬스터의 룩 벡터 X축 기준 룩 벡터 
	D3DXVECTOR3 vMonster_RIght_Dir, Mon_Left_Dir, cross;
	D3DXVec3Normalize(&vMonsterDir_Forward, &vMonsterDir_Forward);
	D3DXVec3Normalize(&vMonster_Player_Dir, &vMonster_Player_Dir);
	D3DXVec3Normalize(&vMonsterDir_Get, &vMonsterDir_Get);
	D3DXVec3Cross(&vMonster_RIght_Dir, &vMonster_Player_Dir, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	float fDot_Player_Monster_Forward;
	fDot_Player_Monster_Forward = D3DXVec3Dot(&vMonster_Player_Dir, &vMonsterDir_Forward);
	//m_pCondition->Add_Hp(-1);

	//딜레이 만들기 
	m_pTransform->MovePos(0.f);
	ChangeTex(L"PigMan_Dead");
	m_pAnimator->Set_Frame(0);
	m_pAnimator->Stop_Animation(true);
	//m_pCondition->Add_Hp(-1);


	D3DXVECTOR3 vMonsterPos_ShotPoint = { vMonster_Pos.x - vMonster_RIght_Dir.x, vMonster_Pos.y - vMonster_RIght_Dir.y  ,vMonster_Pos.z - vMonster_RIght_Dir.z };

	D3DXVECTOR3 vMonster = vPlayer_Pos_Top - vMonsterPos_ShotPoint;
	float fAngle[3] = { 0.f };                      //각도 0도로 줘야함 안주면 총알 빌보드 문제 있음 
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



void CMonster::Monster_Fire2()
{
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
	
	//cout << Mon_RIght_Dir.x << "x" << endl;
	//cout << Mon_RIght_Dir.y << "y" << endl;
	//cout << Mon_RIght_Dir.z <<"z"<< endl;
	//cout << vMonsterDir_Fowrd2.x<<"몬" << endl;
	//	cout << vMonsterDir_Fowrd2.y << "몬" << endl;
	//cout << vMonsterDir_Fowrd2.z << "몬" << endl;
	//	cout << <<"y"<< endl;
	//cout << Mon_RIght_Dir.z	<<"z"<< endl;
	//cout << vMonsterPos.x<<"x2" << endl;
	//cout << vMonsterPos.y<<"y2" << endl;
	m_pAnimator->Stop_Animation(false);

	ChangeTex(L"PigMan_PreFire");
	m_pAnimator->Set_FrameAmp(0.5f);

	//cout << fDot_Player_Monster_Forward << endl;
	if (acos(fDot_Player_Monster_Forward) * 90 < 250)                                         // 정면일 경우만 사격한다. 
	{
		if (m_fTime > 1.8f)
		{
			ChangeTex(L"PigMan_Fire");
			m_pAnimator->Set_FrameAmp(1.f);
			m_pAnimator->Stop_Animation(false);

		}
		if (m_fTime > 2)
		{
			CGameObject* pInstance = CBullet::Create(m_pGraphicDev, vMonsterPos_ShotPoint, vMonster, fAngle, fMove, ENGINE::MONSTER_REVOLVER);
			pInstance->Set_MapLayer(m_mapLayer);
			m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::BULLET_MONSTER, pInstance);
			m_fTime = 0;
		}
	}
}

void CMonster::Monster_Dead()
{
	m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_STOP);
		ChangeTex(L"PigMan_Dead");
		m_pAnimator->Set_FrameAmp(1.f);
		m_pAnimator->Set_Frame(5);

}

void CMonster::Monster_Attack()
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
		//m_pCondition->Add_Hp(+1);
	}


	//cout << "거림" << endl;


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
	// 전부다 받아온다. 
	// 그래서 그 각도에 겹치는게 없는지 판단하는게 맞다. 
	// 벽 판단하기 
	// 텍스처 넣기 


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
	if(m_pRigid->Get_IsPush())
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

// 상태기계 오류 피격 당했을때 피격을 여러번 해버려서 문제가 생김 
void CMonster::Monster_State_Set()
{
	if (m_eCurState != m_eNextState || m_eCurState == m_eNextState)
	{
		switch (m_eNextState)
		{
		case MONSTER_IDLE:
			Monster_Idle();
			break;
		case MONSTER_PURSUE:
			Monster_Foward();
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
