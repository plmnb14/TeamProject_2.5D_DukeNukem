#pragma once

#ifndef __Collider_H__

#include "Component.h"

BEGIN(ENGINE)

class CGameObject;
class ENGINE_DLL CCollider : public CComponent
{
private:
	explicit CCollider(D3DXVECTOR3 _Pos, D3DXVECTOR3 _MinPos, D3DXVECTOR3 _MaxPos);

public:
	virtual ~CCollider();

public:
	virtual void LateUpdate();

public:
	static CCollider* Create(D3DXVECTOR3 _Pos , D3DXVECTOR3 _MinPos, D3DXVECTOR3 _MaxPos);

private:
	BOXCOL m_tBoxCollider;
	CGameObject* m_pTarget;
};

END

#define __Collider_H__
#endif