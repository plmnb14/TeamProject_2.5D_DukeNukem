#pragma once

#ifndef __CCOLLISIONMGR_H__

#include "Engine_Include.h"

BEGIN(ENGINE)

class CGameObject;
class CCollider;
class ENGINE_DLL CCollisionMgr
{
public:
	CCollisionMgr();
	~CCollisionMgr();

public:
	static void CollisionRect(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList);
	static void CollisionRectEX(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList);
	static void CollisionSphere(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList);

	static void CollisionPlayer_To_Other(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList);
	static void CollisionPlayer_To_Item(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList);
	static void CollisionTarget_To_Ground(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList);

public:
	static bool Check_AABB(ENGINE::CGameObject* rDst, ENGINE::CGameObject* rSrc , CCollider* _rDstCol, CCollider* _rSrcCol);
	static bool Check_AABB_to_PLANE(ENGINE::CGameObject * rDst, ENGINE::CGameObject * rSrc, CCollider * _rDstCol, CCollider * _rSrcCol);
	static bool Check_Collision(ENGINE::CGameObject * rDst, ENGINE::CGameObject * rSrc, CCollider * _rDstCol, CCollider * _rSrcCol);

public:
	static D3DXVECTOR3 Get_Length(ENGINE::BOXCOL * _DistCollider, ENGINE::BOXCOL * _TargetCollider, bool _Dynamic = false);

};


END

#define __CCOLLISIONMGR_H__
#endif
