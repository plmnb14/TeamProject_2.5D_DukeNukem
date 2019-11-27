#pragma once

#ifndef __EFFECT_FIREBALL_H__

#include "Vfx.h"

namespace ENGINE
{
	class CResourceMgr;
	class CTimeMgr;
	class CVIBuffer;
	class CTexture;
	class CTransform;
	class CCollider;
	class CRigidBody;
}

class CEffect_Fireball : public CVfx
{
protected:
	explicit CEffect_Fireball(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CEffect_Fireball();

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
	void Set_Angle(float _Angle);
	void Check_Physic();

protected:
	HRESULT AddComponent();

public:
	static CEffect_Fireball* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos, float _Angle = 0);

private:
	WORD	m_wFrame;
	float	m_yAngle;
	float	m_fMove;
	float	m_fPower;
	float	m_fMaxAccel;
	int		m_iCount;
	bool	m_bCreate;
	float	m_fLifetime;
};

#define __EFFECT_FIREBALL_H__
#endif