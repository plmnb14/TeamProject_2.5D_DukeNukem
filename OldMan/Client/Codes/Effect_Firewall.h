#pragma once

#ifndef __EFFECT_FIREWALL_H__

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

class CEffect_Firewall : public CVfx
{
protected:
	explicit CEffect_Firewall(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CEffect_Firewall();

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
	static CEffect_Firewall* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos, D3DXVECTOR3 _Size);

private:
	WORD	m_wFrame;
	D3DXVECTOR3 m_vSize;
};

#define __EFFECT_FIREWALL_H__
#endif