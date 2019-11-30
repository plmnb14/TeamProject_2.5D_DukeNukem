#pragma once

#ifndef __EFFECT_EXPLOSION_ROCKET_H__

#include "Vfx.h"

namespace ENGINE
{
	class CResourceMgr;
	class CTimeMgr;
	class CVIBuffer;
	class CTexture;
	class CTransform;
	class CCollider;
	class CCondition;
}

class CEffect_Explosion_Rocket : public CVfx
{
protected:
	explicit CEffect_Explosion_Rocket(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CEffect_Explosion_Rocket();

public:
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

protected:
	virtual HRESULT Initialize() override;
	virtual HRESULT LateInit() override;
	virtual void Release() override;

public:
	virtual void Set_Pos(D3DXVECTOR3 _Pos);
	virtual void Set_Damage(float _Damage);

protected:
	HRESULT AddComponent();

public:
	static CEffect_Explosion_Rocket* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos, float _Attack);

protected:
	WORD	m_wFrame;
	ENGINE::CCollider*		m_pBombCollider;
	ENGINE::CCondition*		m_pCondition;
};

#define __EFFECT_EXPLOSION_ROCKET_H__
#endif