#include <iostream>
#include "CollisionMgr.h"
#include "GameObject.h"
#include "Component.h"
#include "Collider.h"
#include "Trasform.h"
#include "RigidBody.h"

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
			ENGINE::CCollider* rDstCol = dynamic_cast<CCollider*>(rDst->Get_Component(L"Collider"));
			ENGINE::CCollider* rSrcCol = dynamic_cast<CCollider*>(rSrc->Get_Component(L"Collider"));

			ENGINE::CTransform* rDstTrans = dynamic_cast<CTransform*>(rDst->Get_Component(L"Transform"));
			ENGINE::CTransform* rSrcTrans = dynamic_cast<CTransform*>(rSrc->Get_Component(L"Transform"));

			if(Check_AABB(rDst, rSrc , rDstCol , rSrcCol))
			{
 				rDstTrans->SetPos(rDstTrans->GetPos() + rDstCol->Get_Length());
				rSrcTrans->SetPos(rSrcTrans->GetPos() + rSrcCol->Get_Length());
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
			ENGINE::CCollider* rDstCol = dynamic_cast<CCollider*>(rDst->Get_Component(L"Collider"));
			ENGINE::CCollider* rSrcCol = dynamic_cast<CCollider*>(rSrc->Get_Component(L"Collider"));

			ENGINE::CTransform* rDstTrans = dynamic_cast<CTransform*>(rDst->Get_Component(L"Transform"));
			ENGINE::CTransform* rSrcTrans = dynamic_cast<CTransform*>(rSrc->Get_Component(L"Transform"));

			if (Check_AABB(rDst, rSrc, rDstCol, rSrcCol))
			{
				rDstTrans->SetPos(rDstTrans->GetPos() + rDstCol->Get_Length());
				rSrcTrans->SetPos(rSrcTrans->GetPos() + rSrcCol->Get_Length());
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
			ENGINE::CCollider* rDstCol = dynamic_cast<CCollider*>(rDst->Get_Component(L"GCheck_Collider"));
			ENGINE::CCollider* rSrcCol = dynamic_cast<CCollider*>(rSrc->Get_Component(L"Collider"));

			if (Check_AABB(rDst, rSrc, rDstCol, rSrcCol))
			{
				cout << "충돌체크 됩니다" << endl;

				dynamic_cast<CRigidBody*>(rDst->Get_Component(L"RigidBody"))->Set_Accel({ 0,0,0 });
				dynamic_cast<CRigidBody*>(rDst->Get_Component(L"RigidBody"))->Set_IsJump(false);
				dynamic_cast<CRigidBody*>(rDst->Get_Component(L"RigidBody"))->Set_IsFall(false);
				dynamic_cast<CRigidBody*>(rDst->Get_Component(L"RigidBody"))->Set_IsAir(false);
				dynamic_cast<CRigidBody*>(rDst->Get_Component(L"RigidBody"))->Set_IsGround(true);

				return;
			}
		}

		cout << "충돌 안함" << endl;
		dynamic_cast<CRigidBody*>(rDst->Get_Component(L"RigidBody"))->Set_IsGround(false);
		dynamic_cast<CRigidBody*>(rDst->Get_Component(L"RigidBody"))->Set_IsFall(true);
	}
}

bool CCollisionMgr::Check_AABB(ENGINE::CGameObject* rDst , ENGINE::CGameObject* rSrc, CCollider* _rDstCol , CCollider* _rSrcCol)
{
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
