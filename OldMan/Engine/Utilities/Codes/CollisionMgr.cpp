#include <iostream>
#include "CollisionMgr.h"
#include "GameObject.h"
#include "Component.h"
#include "Collider.h"
#include "Trasform.h"
#include "RigidBody.h"
#include "Condition.h"
USING(ENGINE)

CCollisionMgr::CCollisionMgr()
{
}


CCollisionMgr::~CCollisionMgr()
{
}

void CCollisionMgr::CollisionRect(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList)
{
	RECT rc = {};
	for (auto& rDst : rDstList)
	{
		for (auto& rSrc : rSrcList)
		{

		}
	}
}

void CCollisionMgr::CollisionRectEX(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList)
{
	float fMoveX = 0.f, fMoveY = 0.f;
	for (auto& rDst : rDstList)
	{
		for (auto& rSrc : rSrcList)
		{
		}
	}
}

void CCollisionMgr::CollisionSphere(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList)
{
	for (auto& rDst : rDstList)
	{
		for (auto& rSrc : rSrcList)
		{
			ENGINE::CCollider* rDstCol = static_cast<CCollider*>(rDst->Get_Component(L"Collider"));
			ENGINE::CCollider* rSrcCol = static_cast<CCollider*>(rSrc->Get_Component(L"Collider"));

			ENGINE::CTransform* rDstTrans = static_cast<CTransform*>(rDst->Get_Component(L"Transform"));
			ENGINE::CTransform* rSrcTrans = static_cast<CTransform*>(rSrc->Get_Component(L"Transform"));

			if(Check_AABB_to_PLANE(rDst, rSrc , rDstCol , rSrcCol))
			{
				cout << "진짜 겹친다고!" << endl;
				rSrcCol->Set_IsCollision(true);
 				//rDstTrans->SetPos(rDstTrans->GetPos() + rDstCol->Get_Length());
				//rSrcTrans->SetPos(rSrcTrans->GetPos() + rSrcCol->Get_Length());
			}
		}
	}
}

void CCollisionMgr::CollisionPlayer_To_Other(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList)
{
	for (auto& rDst : rDstList)
	{
		for (auto& rSrc : rSrcList)
		{
			ENGINE::CTransform* rDstTrans = static_cast<CTransform*>(rDst->Get_Component(L"Transform"));
			ENGINE::CTransform* rSrcTrans = static_cast<CTransform*>(rSrc->Get_Component(L"Transform"));

			float a = D3DXVec3Length(&(rDstTrans->GetPos() - rSrcTrans->GetPos()));

			if (a > 5)
				continue;

			ENGINE::CCollider* rDstCol = static_cast<CCollider*>(rDst->Get_Component(L"Collider"));
			ENGINE::CCollider* rDstGCol = static_cast<CCollider*>(rDst->Get_Component(L"GCheck_Collider"));
			ENGINE::CCollider* rSrcCol = static_cast<CCollider*>(rSrc->Get_Component(L"Collider"));

			if (Check_AABB(rDst, rSrc, rDstCol, rSrcCol))
			{
				rDstTrans->SetPos(rDstTrans->GetPos() + rDstCol->Get_Length());
				rSrcTrans->SetPos(rSrcTrans->GetPos() + rSrcCol->Get_Length());


				rDstCol->LateUpdate(rDstTrans->GetPos());
				rDstGCol->LateUpdate({ rDstTrans->GetPos().x ,
					rDstTrans->GetPos().y - rDstCol->Get_Radius().y,
					rDstTrans->GetPos().z });

				rSrcCol->LateUpdate(rSrcTrans->GetPos());

				rDstCol->Set_Length({ 0,0,0 });
				rSrcCol->Set_Length({ 0,0,0 });
			}
		}
	}
}

void CCollisionMgr::CollisionPlayer_To_Item(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList)
{
	for (auto& rDst : rDstList)
	{
		for (auto& rSrc : rSrcList)
		{
			ENGINE::CTransform* rDstTrans = static_cast<CTransform*>(rDst->Get_Component(L"Transform"));
			ENGINE::CTransform* rSrcTrans = static_cast<CTransform*>(rSrc->Get_Component(L"Transform"));

			float a = D3DXVec3Length(&(rDstTrans->GetPos() - rSrcTrans->GetPos()));

			if (a > 5)
				continue;

			ENGINE::CCollider* rDstCol = static_cast<CCollider*>(rDst->Get_Component(L"Collider"));
			ENGINE::CCollider* rSrcCol = static_cast<CCollider*>(rSrc->Get_Component(L"Collider"));

			if (Check_AABB(rDst, rSrc, rDstCol, rSrcCol))
			{
				rSrc->SetDead();
			}
		}
	}
}

void CCollisionMgr::CollisionTarget_To_Ground(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList)
{
	for (auto& rDst : rDstList)
	{
		for (auto& rSrc : rSrcList)
		{
			ENGINE::CTransform* rDstTrans = static_cast<CTransform*>(rDst->Get_Component(L"Transform"));
			ENGINE::CTransform* rSrcTrans = static_cast<CTransform*>(rSrc->Get_Component(L"Transform"));

			float a = D3DXVec3Length(&(rDstTrans->GetPos() - rSrcTrans->GetPos()));

			//if (a > 5)
			//	continue;

			ENGINE::CCollider* rDstCol = static_cast<CCollider*>(rDst->Get_Component(L"GCheck_Collider"));
			ENGINE::CCollider* rSrcCol = static_cast<CCollider*>(rSrc->Get_Component(L"Collider"));

			if (Check_AABB(rDst, rSrc, rDstCol, rSrcCol))
			{
				if (static_cast<CRigidBody*>(rDst->Get_Component(L"RigidBody"))->Get_IsJump())
					return;

				static_cast<CRigidBody*>(rDst->Get_Component(L"RigidBody"))->Set_Accel({ 0,0,0 });
				static_cast<CRigidBody*>(rDst->Get_Component(L"RigidBody"))->Set_IsJump(false);
				static_cast<CRigidBody*>(rDst->Get_Component(L"RigidBody"))->Set_IsFall(false);
				static_cast<CRigidBody*>(rDst->Get_Component(L"RigidBody"))->Set_IsAir(false);
				static_cast<CRigidBody*>(rDst->Get_Component(L"RigidBody"))->Set_IsGround(true);

				//cout << "응! 닿음!" << endl;

				return;
			}
		}


	//	cout << "땅에 닿지 않음" << endl;
		static_cast<CRigidBody*>(rDst->Get_Component(L"RigidBody"))->Set_IsGround(false);
		static_cast<CRigidBody*>(rDst->Get_Component(L"RigidBody"))->Set_IsFall(true);

		static_cast<CRigidBody*>(rDst->Get_Component(L"RigidBody"))->Set_IsGround(false);
		static_cast<CRigidBody*>(rDst->Get_Component(L"RigidBody"))->Set_IsFall(true);
	}
}

void CCollisionMgr::CollisionBullet_To_Other(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList)
{
	for (auto& rDst : rDstList)
	{
		for (auto& rSrc : rSrcList)
		{
			if (rDst->GetDead())
				continue;

			ENGINE::CTransform* rDstTrans = static_cast<CTransform*>(rDst->Get_Component(L"Transform"));
			ENGINE::CTransform* rSrcTrans = static_cast<CTransform*>(rSrc->Get_Component(L"Transform"));

			float a = D3DXVec3Length(&(rDstTrans->GetPos() - rSrcTrans->GetPos()));

			if (a > 5)
				continue;

			ENGINE::CCollider* rDstCol = static_cast<CCollider*>(rDst->Get_Component(L"Collider"));
			ENGINE::CCollider* rSrcCol = static_cast<CCollider*>(rSrc->Get_Component(L"Collider"));

			if (Check_AABB_Bullet(rDst, rSrc, rDstCol, rSrcCol))
			{
				rDst->SetDead();
				cout << "충돌" << endl;
				continue;
			}
		}

	}
}

void CCollisionMgr::CollisionTarget_To_Monstr(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList)
{
	for (auto& rDst : rDstList)
	{
		for (auto& rSrc : rSrcList)
		{
			ENGINE::CCollider* rDstCol = dynamic_cast<CCollider*>(rDst->Get_Component(L"Collider"));
			ENGINE::CCollider* rSrcCol = dynamic_cast<CCollider*>(rSrc->Get_Component(L"Collider"));

			ENGINE::CTransform* rDstTrans = dynamic_cast<CTransform*>(rDst->Get_Component(L"Transform"));
			ENGINE::CTransform* rSrcTrans = dynamic_cast<CTransform*>(rSrc->Get_Component(L"Transform"));

			if (Check_AABB(rDst, rSrc, rDstCol, rSrcCol))
			{
				//cout << "충돌체크 됩니다" << endl;

				dynamic_cast<CCondition*>(rDst->Get_Component(L"Condition"))->Set_Hit(true);
				return;
			}
			else
			{
			//cout << "충돌 안함" << endl;
				dynamic_cast<CCondition*>(rDst->Get_Component(L"Condition"))->Set_Hit(false);

			}
		}


	}

}


bool CCollisionMgr::Check_AABB(ENGINE::CGameObject* rDst , ENGINE::CGameObject* rSrc, CCollider* _rDstCol , CCollider* _rSrcCol)
{
	if (_rSrcCol->Get_CollisionType() != ENGINE::COLLISION_AABB)
		return false;

	ENGINE::BOXCOL* rDstBox = _rDstCol->Get_BoxCollider();
	ENGINE::BOXCOL* rSrtBox = _rSrcCol->Get_BoxCollider();

	if (rDstBox->vMinPos.x <= rSrtBox->vMaxPos.x && rDstBox->vMaxPos.x >= rSrtBox->vMinPos.x &&
		rDstBox->vMinPos.y <= rSrtBox->vMaxPos.y && rDstBox->vMaxPos.y >= rSrtBox->vMinPos.y &&
		rDstBox->vMinPos.z <= rSrtBox->vMaxPos.z && rDstBox->vMaxPos.z >= rSrtBox->vMinPos.z)
	{
		// 콜리전 하는 물체끼리 체크
		if (!rDstBox->bIsTrigger && !rSrtBox->bIsTrigger)
		{
			_rDstCol->Set_IsCollision(true);

			// A 가 Dynamic 이고 , B 도 Dynamic 일 때
			if (rDstBox->bIsDynamic && rSrtBox->bIsDynamic)
			{
				D3DXVECTOR3 tmpLength_Dst = Get_Length(rDstBox, rSrtBox, true);
				D3DXVECTOR3 tmpLength_Srt = Get_Length(rDstBox, rSrtBox, true);

				(rDstBox->vCenterPos.x < rSrtBox->vCenterPos.x ? tmpLength_Dst.x *= -1.f : tmpLength_Dst.x *= 1.f);
				(rDstBox->vCenterPos.y < rSrtBox->vCenterPos.y ? tmpLength_Dst.y *= -1.f : tmpLength_Dst.y *= 1.f);
				(rDstBox->vCenterPos.z < rSrtBox->vCenterPos.z ? tmpLength_Dst.z *= -1.f : tmpLength_Dst.z *= 1.f);

				(rDstBox->vCenterPos.x < rSrtBox->vCenterPos.x ? tmpLength_Srt.x *= 1.f : tmpLength_Srt.x *= -1.f);
				(rDstBox->vCenterPos.y < rSrtBox->vCenterPos.y ? tmpLength_Srt.y *= 1.f : tmpLength_Srt.y *= -1.f);
				(rDstBox->vCenterPos.z < rSrtBox->vCenterPos.z ? tmpLength_Srt.z *= 1.f : tmpLength_Srt.z *= -1.f);

				_rDstCol->Set_Length(tmpLength_Dst);
				_rSrcCol->Set_Length(tmpLength_Srt);
			}

			// A 가 Dynamic 이고 , B 는 Static 일 때
			else if (rDstBox->bIsDynamic && !rSrtBox->bIsDynamic)
			{
				D3DXVECTOR3 tmpLength = Get_Length(rDstBox, rSrtBox);
				
				(rDstBox->vCenterPos.x < rSrtBox->vCenterPos.x ? tmpLength.x *= -1.f : tmpLength.x *= 1.f);
				(rDstBox->vCenterPos.y < rSrtBox->vCenterPos.y ? tmpLength.y *= -1.f : tmpLength.y *= 1.f);
				(rDstBox->vCenterPos.z < rSrtBox->vCenterPos.z ? tmpLength.z *= -1.f : tmpLength.z *= 1.f);

				_rDstCol->Set_Length(tmpLength);
			}

			// A 가 Static 이고 , B 는 Dynamic 일 때
			else if (!rDstBox->bIsDynamic && rSrtBox->bIsDynamic)
			{
				_rSrcCol->Set_Length(Get_Length(rDstBox, rSrtBox));
			}
		}

		// Dst 는 트리거, Src 는 트리거 아님
		else if (rDstBox->bIsTrigger && !rSrtBox->bIsTrigger)
		{
			return true;
		}

		return true;
	}

	else
	{


		return false;
	}

	return false;
}

bool CCollisionMgr::Check_AABB_Bullet(ENGINE::CGameObject * rDst, ENGINE::CGameObject * rSrc, CCollider * _rDstCol, CCollider * _rSrcCol)
{
	if (_rSrcCol->Get_CollisionType() != ENGINE::COLLISION_AABB)
		return false;

	ENGINE::BOXCOL* rDstBox = _rDstCol->Get_BoxCollider();
	ENGINE::BOXCOL* rSrtBox = _rSrcCol->Get_BoxCollider();

	if (rDstBox->vMinPos.x <= rSrtBox->vMaxPos.x && rDstBox->vMaxPos.x >= rSrtBox->vMinPos.x &&
		rDstBox->vMinPos.y <= rSrtBox->vMaxPos.y && rDstBox->vMaxPos.y >= rSrtBox->vMinPos.y &&
		rDstBox->vMinPos.z <= rSrtBox->vMaxPos.z && rDstBox->vMaxPos.z >= rSrtBox->vMinPos.z)
	{
		return true;
	}

	else 
	{
		// 현재 거리와 과거 거리의 차이를 구한다. 그리고, 차이의 일정 비율만큼 빼준다.

		D3DXVECTOR3 vTempGap = _rDstCol->Get_OldPos() - rDstBox->vCenterPos;
		D3DXVECTOR3 vTemPos[19] = {};
		vTemPos[0] = rDstBox->vCenterPos + vTempGap * 0.95f;
		vTemPos[1] = rDstBox->vCenterPos + vTempGap * 0.9f;
		vTemPos[2] = rDstBox->vCenterPos + vTempGap * 0.85f;
		vTemPos[3] = rDstBox->vCenterPos + vTempGap * 0.8f;
		vTemPos[4] = rDstBox->vCenterPos + vTempGap * 0.75f;
		vTemPos[5] = rDstBox->vCenterPos + vTempGap * 0.7f;
		vTemPos[6] = rDstBox->vCenterPos + vTempGap * 0.65f;
		vTemPos[7] = rDstBox->vCenterPos + vTempGap * 0.6f;
		vTemPos[8] = rDstBox->vCenterPos + vTempGap * 0.55f;
		vTemPos[9] = rDstBox->vCenterPos + vTempGap * 0.5f;
		vTemPos[10] = rDstBox->vCenterPos+ vTempGap  * 0.45f;
		vTemPos[11] = rDstBox->vCenterPos+ vTempGap  * 0.4f;
		vTemPos[12] = rDstBox->vCenterPos+ vTempGap  * 0.35f;
		vTemPos[13] = rDstBox->vCenterPos+ vTempGap  * 0.3f;
		vTemPos[14] = rDstBox->vCenterPos+ vTempGap  * 0.25f;
		vTemPos[15] = rDstBox->vCenterPos+ vTempGap  * 0.2f;
		vTemPos[16] = rDstBox->vCenterPos+ vTempGap  * 0.15f;
		vTemPos[17] = rDstBox->vCenterPos+ vTempGap  * 0.1f;
		vTemPos[18] = rDstBox->vCenterPos+ vTempGap  * 0.05f; 

		//D3DXVECTOR3 vTemPos[19] = {};
		//vTemPos[0] = rDstBox->vCenterPos - (_rDstCol->Get_OldPos() * 0.95f);
		//vTemPos[1] = rDstBox->vCenterPos - (_rDstCol->Get_OldPos() * 0.9f);
		//vTemPos[2] = rDstBox->vCenterPos - (_rDstCol->Get_OldPos() * 0.85f);
		//vTemPos[3] = rDstBox->vCenterPos - (_rDstCol->Get_OldPos() * 0.8f);
		//vTemPos[4] = rDstBox->vCenterPos - (_rDstCol->Get_OldPos() * 0.75f);
		//vTemPos[5] = rDstBox->vCenterPos - (_rDstCol->Get_OldPos() * 0.7f);
		//vTemPos[6] = rDstBox->vCenterPos - (_rDstCol->Get_OldPos() * 0.65f);
		//vTemPos[7] = rDstBox->vCenterPos - (_rDstCol->Get_OldPos() * 0.6f);
		//vTemPos[8] = rDstBox->vCenterPos - (_rDstCol->Get_OldPos() * 0.55f);
		//vTemPos[9] = rDstBox->vCenterPos - (_rDstCol->Get_OldPos() * 0.5f);
		//vTemPos[10] = rDstBox->vCenterPos - (_rDstCol->Get_OldPos() * 0.45f);
		//vTemPos[11] = rDstBox->vCenterPos - (_rDstCol->Get_OldPos() * 0.4f);
		//vTemPos[12] = rDstBox->vCenterPos - (_rDstCol->Get_OldPos() * 0.35f);
		//vTemPos[13] = rDstBox->vCenterPos - (_rDstCol->Get_OldPos() * 0.3f);
		//vTemPos[14] = rDstBox->vCenterPos - (_rDstCol->Get_OldPos() * 0.25f);
		//vTemPos[15] = rDstBox->vCenterPos - (_rDstCol->Get_OldPos() * 0.2f);
		//vTemPos[16] = rDstBox->vCenterPos - (_rDstCol->Get_OldPos() * 0.15f);
		//vTemPos[17] = rDstBox->vCenterPos - (_rDstCol->Get_OldPos() * 0.1f);
		//vTemPos[18] = rDstBox->vCenterPos - (_rDstCol->Get_OldPos() * 0.05f); 

		//D3DXVECTOR3 vTemPos[19] = {};
		//vTemPos[0] = (_rDstCol->Get_OldPos() - rDstBox->vCenterPos * 0.95f);
		//vTemPos[1] = (_rDstCol->Get_OldPos() - rDstBox->vCenterPos * 0.9f);
		//vTemPos[2] = (_rDstCol->Get_OldPos() - rDstBox->vCenterPos * 0.85f);
		//vTemPos[3] = (_rDstCol->Get_OldPos() - rDstBox->vCenterPos * 0.8f);
		//vTemPos[4] = (_rDstCol->Get_OldPos() - rDstBox->vCenterPos * 0.75f);
		//vTemPos[5] = (_rDstCol->Get_OldPos() - rDstBox->vCenterPos * 0.7f);
		//vTemPos[6] = (_rDstCol->Get_OldPos() - rDstBox->vCenterPos * 0.65f);
		//vTemPos[7] = (_rDstCol->Get_OldPos() - rDstBox->vCenterPos * 0.6f);
		//vTemPos[8] = (_rDstCol->Get_OldPos() - rDstBox->vCenterPos * 0.55f);
		//vTemPos[9] = (_rDstCol->Get_OldPos() - rDstBox->vCenterPos * 0.5f);
		//vTemPos[10] = (_rDstCol->Get_OldPos() - rDstBox->vCenterPos * 0.45f);
		//vTemPos[11] = (_rDstCol->Get_OldPos() - rDstBox->vCenterPos * 0.4f);
		//vTemPos[12] = (_rDstCol->Get_OldPos() - rDstBox->vCenterPos * 0.35f);
		//vTemPos[13] = (_rDstCol->Get_OldPos() - rDstBox->vCenterPos * 0.3f);
		//vTemPos[14] = (_rDstCol->Get_OldPos() - rDstBox->vCenterPos * 0.25f);
		//vTemPos[15] = (_rDstCol->Get_OldPos() - rDstBox->vCenterPos * 0.2f);
		//vTemPos[16] = (_rDstCol->Get_OldPos() - rDstBox->vCenterPos * 0.15f);
		//vTemPos[17] = (_rDstCol->Get_OldPos() - rDstBox->vCenterPos * 0.1f);
		//vTemPos[18] = (_rDstCol->Get_OldPos() - rDstBox->vCenterPos * 0.05f);

		for (int i = 0; i < 19; ++i)
		{
			_rDstCol->Set_CenterPos(vTemPos[i]);
			_rDstCol->SetUp_Box();

			if (rDstBox->vMinPos.x <= rSrtBox->vMaxPos.x && rDstBox->vMaxPos.x >= rSrtBox->vMinPos.x &&
				rDstBox->vMinPos.y <= rSrtBox->vMaxPos.y && rDstBox->vMaxPos.y >= rSrtBox->vMinPos.y &&
				rDstBox->vMinPos.z <= rSrtBox->vMaxPos.z && rDstBox->vMaxPos.z >= rSrtBox->vMinPos.z)
			{
				return true;
			}
		}
		return false;
	}

	return false;
}

bool CCollisionMgr::Check_AABB_to_PLANE(ENGINE::CGameObject* rDst, ENGINE::CGameObject* rSrc, CCollider* _rDstCol, CCollider* _rSrcCol)
{
	ENGINE::BOXCOL* rDstBox = _rDstCol->Get_BoxCollider();
	ENGINE::BOXCOL* rSrtBox = _rSrcCol->Get_BoxCollider();

	ENGINE::CTransform* rDstTrans = static_cast<CTransform*>(rDst->Get_Component(L"Transform"));
	ENGINE::CTransform* rSrcTrans = static_cast<CTransform*>(rSrc->Get_Component(L"Transform"));


	D3DXVECTOR3* tmpVtx =  _rSrcCol->Get_PlaneVtx();
	D3DXVECTOR3 tmpMax , tmpMin;
	
	
	D3DXPLANE tmpPlane;
	D3DXPlaneFromPoints(&tmpPlane, &tmpVtx[0] , &tmpVtx[1] , &tmpVtx[2]);

	if (rSrcTrans->GetDir().x >= 0)
	{
		tmpMax.x = rDstBox->vMaxPos.x;
		tmpMin.x = rDstBox->vMinPos.x;
	}
	
	else
	{
		tmpMax.x = rDstBox->vMinPos.x;
		tmpMin.x = rDstBox->vMaxPos.x;
	}


	if (rSrcTrans->GetDir().y >= 0)
	{
		tmpMax.y = rDstBox->vMaxPos.y;
		tmpMin.y = rDstBox->vMinPos.y;
	}

	else
	{
		tmpMax.y = rDstBox->vMinPos.y;
		tmpMin.y = rDstBox->vMaxPos.y;
	}


	if (rSrcTrans->GetDir().z >= 0)
	{
		tmpMax.z = rDstBox->vMaxPos.z;
		tmpMin.z = rDstBox->vMinPos.z;
	}

	else
	{
		tmpMax.z = rDstBox->vMinPos.z;
		tmpMin.z = rDstBox->vMaxPos.z;
	}
	
	if (D3DXPlaneDotCoord(&tmpPlane, &tmpMin) < 0)
	{
		cout << " 뒤에 있어" << endl;
		return false;
	}
	
	else if (D3DXPlaneDotCoord(&tmpPlane, &tmpMax) > 0)
	{
		cout << " 앞에 있어" << endl;
		return false;
	}

	else
	{
		cout << "겹 친다" << endl;
		return true;
	}

	return false;
}

bool CCollisionMgr::Check_Collision(ENGINE::CGameObject* rDst, ENGINE::CGameObject* rSrc, CCollider* _rDstCol, CCollider* _rSrcCol)
{
	if(_rDstCol->Get_CollisionType() == ENGINE::COLLISION_LINE)
	{
		switch (_rSrcCol->Get_CollisionType())
		{
		case ENGINE::COLLISION_LINE:
		{
			break;
		}

		case ENGINE::COLLISION_PLANE:
		{
			break;
		}

		case ENGINE::COLLISION_AABB:
		{
			break;
		}

		case ENGINE::COLLISION_SHPERE:
		{
			break;
		}

		case ENGINE::COLLISION_CAPSHULE:
		{
			break;
		}
		}
	}

	if (_rDstCol->Get_CollisionType() == ENGINE::COLLISION_PLANE)
	{
		switch (_rSrcCol->Get_CollisionType())
		{
		case ENGINE::COLLISION_LINE:
		{
			break;
		}

		case ENGINE::COLLISION_PLANE:
		{
			break;
		}

		case ENGINE::COLLISION_AABB:
		{
			break;
		}

		case ENGINE::COLLISION_SHPERE:
		{
			break;
		}

		case ENGINE::COLLISION_CAPSHULE:
		{
			break;
		}
		}
	}

	if (_rDstCol->Get_CollisionType() == ENGINE::COLLISION_AABB)
	{
		switch (_rSrcCol->Get_CollisionType())
		{
		case ENGINE::COLLISION_LINE:
		{
			break;
		}

		case ENGINE::COLLISION_PLANE:
		{
			break;
		}

		case ENGINE::COLLISION_AABB:
		{
			if (Check_AABB(rDst, rSrc, _rDstCol, _rSrcCol))
			{
				return true;
			}

			break;
		}

		case ENGINE::COLLISION_SHPERE:
		{
			break;
		}

		case ENGINE::COLLISION_CAPSHULE:
		{
			break;
		}
		}
	}

	if (_rDstCol->Get_CollisionType() == ENGINE::COLLISION_SHPERE)
	{
		switch (_rSrcCol->Get_CollisionType())
		{
		case ENGINE::COLLISION_LINE:
		{
			break;
		}

		case ENGINE::COLLISION_PLANE:
		{
			break;
		}

		case ENGINE::COLLISION_AABB:
		{
			break;
		}

		case ENGINE::COLLISION_SHPERE:
		{
			break;
		}

		case ENGINE::COLLISION_CAPSHULE:
		{
			break;
		}
		}
	}

	if (_rDstCol->Get_CollisionType() == ENGINE::COLLISION_CAPSHULE)
	{
		switch (_rSrcCol->Get_CollisionType())
		{
		case ENGINE::COLLISION_LINE:
		{
			break;
		}

		case ENGINE::COLLISION_PLANE:
		{
			break;
		}

		case ENGINE::COLLISION_AABB:
		{
			break;
		}

		case ENGINE::COLLISION_SHPERE:
		{
			break;
		}

		case ENGINE::COLLISION_CAPSHULE:
		{
			break;
		}
		}
	}

	return false;
}

D3DXVECTOR3 CCollisionMgr::Get_Length(ENGINE::BOXCOL * _DistCollider, ENGINE::BOXCOL * _TargetCollider, bool _Dynamic)
{
	D3DXVECTOR3 vMin = {};
	D3DXVECTOR3 vMax = {};
	D3DXVECTOR3 vCross = {};

	vMax.x = min(_DistCollider->vMaxPos.x, _TargetCollider->vMaxPos.x);
	vMax.y = min(_DistCollider->vMaxPos.y, _TargetCollider->vMaxPos.y);
	vMax.z = min(_DistCollider->vMaxPos.z, _TargetCollider->vMaxPos.z);

	vMin.x = max(_DistCollider->vMinPos.x, _TargetCollider->vMinPos.x);
	vMin.y = max(_DistCollider->vMinPos.y, _TargetCollider->vMinPos.y);
	vMin.z = max(_DistCollider->vMinPos.z, _TargetCollider->vMinPos.z);

	vCross.x = vMax.x - vMin.x;
	vCross.y = vMax.y - vMin.y;
	vCross.z = vMax.z - vMin.z;

	if (vCross.x == vCross.y && vCross.y == vCross.z)
	{
		return { 0,0,0 };
	}

	//(vCross.x < vCross.y ? vCross.x < vCross.z ? 
	//	vCross.y = 0 : vCross.z = 0 : 
	//	vCross.y < vCross.z ? vCross.z = 0 : vCross.y = 0);

	if (vCross.x < vCross.y)
	{
		if (vCross.x < vCross.z)
		{
			vCross.z = 0;
		}
	
		else
		{
			vCross.x = 0;
		}
	
		vCross.y = 0;
	}
	
	else
	{
		if (vCross.y < vCross.z)
		{
			vCross.z = 0;
		}
	
		else
		{
			vCross.y = 0;
		}
	
		vCross.x = 0;
	}

	if (_Dynamic)
		return vCross * 0.5f;

	else
		return vCross;
}
