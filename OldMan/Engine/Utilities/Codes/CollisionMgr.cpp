#include "CollisionMgr.h"
#include "GameObject.h"
#include "Component.h"
#include "Collider.h"
#include "Trasform.h"

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

			if (rDstCol == nullptr || rSrcCol == nullptr)
				continue;

			if(Check_AABB(rDstCol, rSrcCol))
			{
 				rDstTrans->SetPos(rDstTrans->GetPos() + rDstCol->Get_Length());
				rSrcTrans->SetPos(rSrcTrans->GetPos() + rSrcCol->Get_Length());
			}
		}
	}
}

bool CCollisionMgr::Check_AABB(ENGINE::CCollider* rDst , ENGINE::CCollider* rSrc)
{
	if (rDst == nullptr || rSrc == nullptr)
		return false;

	ENGINE::BOXCOL* rDstBox = rDst->Get_BoxCollider();
	ENGINE::BOXCOL* rSrtBox = rSrc->Get_BoxCollider();

	if (rDstBox->vMinPos.x <= rSrtBox->vMaxPos.x && rDstBox->vMaxPos.x >= rSrtBox->vMinPos.x &&
		rDstBox->vMinPos.y <= rSrtBox->vMaxPos.y && rDstBox->vMaxPos.y >= rSrtBox->vMinPos.y &&
		rDstBox->vMinPos.z <= rSrtBox->vMaxPos.z && rDstBox->vMaxPos.z >= rSrtBox->vMinPos.z)
	{
		rDst->Set_IsCollision(true);
		rSrc->Set_IsCollision(true);

		if (!rDstBox->bIsTrigger && !rDstBox->bIsTrigger)
		{
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

				rDst->Set_Length(tmpLength_Dst);
				rSrc->Set_Length(tmpLength_Srt);
			}

			// A 가 Dynamic 이고 , B 는 Static 일 때
			else if (rDstBox->bIsDynamic && !rSrtBox->bIsDynamic)
			{
				D3DXVECTOR3 tmpLength = Get_Length(rDstBox, rSrtBox);
				
				(rDstBox->vCenterPos.x < rSrtBox->vCenterPos.x ? tmpLength.x *= -1.f : tmpLength.x *= 1.f);
				(rDstBox->vCenterPos.y < rSrtBox->vCenterPos.y ? tmpLength.y *= -1.f : tmpLength.y *= 1.f);
				(rDstBox->vCenterPos.z < rSrtBox->vCenterPos.z ? tmpLength.z *= -1.f : tmpLength.z *= 1.f);

				rDst->Set_Length(tmpLength);
			}

			// A 가 Static 이고 , B 는 Dynamic 일 때
			else if (!rDstBox->bIsDynamic && rSrtBox->bIsDynamic)
			{
				rSrc->Set_Length(Get_Length(rDstBox, rSrtBox));
			}
		}

		return true;
	}

	else
	{
		rDst->Set_IsCollision(false);
		rSrc->Set_IsCollision(false);

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
