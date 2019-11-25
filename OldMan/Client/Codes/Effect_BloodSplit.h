#pragma once

#ifndef __EFFECT_BLOODSPLIT_H__

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

class CEffect_BloodSplit : public CVfx
{
protected:
	explicit CEffect_BloodSplit(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CEffect_BloodSplit();

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

protected:
	HRESULT AddComponent();

public:
	static CEffect_BloodSplit* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos);

private:
	WORD	m_wFrame;
};

#define __EFFECT_BLOODSPLIT_01_H__
#endif