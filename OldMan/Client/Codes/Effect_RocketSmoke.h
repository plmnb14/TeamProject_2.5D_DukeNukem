#pragma once

#ifndef __EFFECT_ROCKETSMOKE_H__

#include "Vfx.h"

namespace ENGINE
{
	class CResourceMgr;
	class CTimeMgr;
	class CVIBuffer;
	class CTexture;
	class CTransform;
	class CCollider;
}

class CEffect_RocketSmoke : public CVfx
{
protected:
	explicit CEffect_RocketSmoke(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CEffect_RocketSmoke();

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
	void Set_Size(D3DXVECTOR3 _Size);

protected:
	HRESULT AddComponent();

public:
	static CEffect_RocketSmoke* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos, D3DXVECTOR3 _Size = { 1,1,1 });

private:
	WORD	m_wFrame;
	float	m_fLifetime;
};

#define __EFFECT_ROCKETSMOKE_H__
#endif