#pragma once

#ifndef __Collider_H__

#include "Component.h"

BEGIN(ENGINE)

class CGameObject;
class ENGINE_DLL CCollider : public CComponent
{
private:
	explicit CCollider();

public:
	virtual ~CCollider();

public:
	virtual void LateUpdate();

public:
	void SetUp_Box();

public:
	void Set_Radius(float* _Radius);
	void Set_Length(float* _Length);
	void Set_CenterPos();
	void Set_UnderPos(D3DXVECTOR3 _UnderPos);

public:
	void Check_AABB(ENGINE::BOXCOL* _TargetCollider);

public:
	bool Get_IsCollision() { return m_tBoxCollider.bIsCollision; }
	BOXCOL* Get_BoxCollider() { return &m_tBoxCollider; }

public:
	static CCollider* Create();

private:
	BOXCOL m_tBoxCollider;
	CGameObject* m_pTarget;
};

END

#define __Collider_H__
#endif