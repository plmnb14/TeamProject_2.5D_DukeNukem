#pragma once

#ifndef __UI_H__

#include "GameObject.h"

namespace ENGINE
{
	class CResourceMgr;
	class CTimeMgr;
	class CVIBuffer;
	class CTexture;
	class CTransform;
	class CBillborad;
	class CCollider;
}

class CUI : public ENGINE::CGameObject
{
protected:
	explicit CUI(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CUI();

public:
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

protected:
	virtual HRESULT Initialize() override;
	virtual HRESULT LateInit() override;
	virtual void Release() override;

public:
	virtual void ChangeTex(wstring _wstrTex);

protected:
	HRESULT AddComponent();
	void KeyInput();

public:
	static CUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, wstring wstrTex = L"");

protected:
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;

	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;
	ENGINE::CBillborad*		m_pBillboard;
	ENGINE::CCollider*		m_pCollider;

	wstring					m_wstrTex;
};

#define __TERRAIN_H__
#endif