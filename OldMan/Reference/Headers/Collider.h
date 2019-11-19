#pragma once

#ifndef __COLLIDER_H__

#include "Component.h"

BEGIN(ENGINE)

class CTransform;
class CGameObject;
class ENGINE_DLL CCollider : public CComponent
{
private:
	explicit CCollider();

public:
	virtual ~CCollider();

public:
	virtual void LateUpdate(D3DXVECTOR3 _Pos);

public:
	void SetUp_Box();

public:
	void Set_Radius(D3DXVECTOR3 _Radius);
	void Set_Length(D3DXVECTOR3 _Length);
	void Set_CenterPos(D3DXVECTOR3 _CenterPos);
	void Set_UnderPos();
	void Set_Dynamic(bool _Dynamic);
	void Set_Trigger(bool _Trigger);
	void Set_IsCollision(bool _Collision);
	void Set_Type(ENGINE::COLLISION_TYPE _Type) { m_eCollisionType = _Type; }
	void Set_CollisionVertex(ENGINE::CTransform* pTarget, ENGINE::VTX_TEX* pTerrainVtx);
	void Set_PlaneVtx(float* _Angle, D3DXMATRIX _World);


public:
	D3DXVECTOR3 Get_CenterPos() { return m_tBoxCollider.vCenterPos; }
	D3DXVECTOR3 Get_UnderPos() { return m_tBoxCollider.vUnderPos; }
	D3DXVECTOR3 Get_Length() { return m_tBoxCollider.vLength; };
	D3DXVECTOR3 Get_Radius() { return m_tBoxCollider.vRadius; }
	D3DXVECTOR3* Get_PlaneVtx() {return m_vPlaneVtx; }

	bool Get_IsCollision() { return m_tBoxCollider.bIsCollision; }

	BOXCOL* Get_BoxCollider() { return &m_tBoxCollider; }
	ENGINE::VTX_TEX* Get_VertexInfo() { if (m_pVtx == nullptr)return nullptr; else return m_pVtx; }
	ENGINE::COLLISION_TYPE Get_CollisionType() { return m_eCollisionType; }

public:
	static CCollider* Create();

private:
	D3DXVECTOR3 m_vPlaneVtx[4];
	BOXCOL m_tBoxCollider;
	CGameObject* m_pTarget;
	ENGINE::COLLISION_TYPE m_eCollisionType;
	ENGINE::VTX_TEX*	m_pVtx;
};

END

#define __COLLIDER_H__
#endif