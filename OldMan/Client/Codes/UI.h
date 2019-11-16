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
	class CCollider;
}

class CCameraObserver;
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
	void ChangeTex(wstring _wstrTex);
	void SetSize(float _fSizeX, float _fSizeY);
	void SetPos(D3DXVECTOR3 _vPos);

protected:
	HRESULT AddComponent();

public:
	static CUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, wstring wstrTex = L"");

protected:
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;

	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;
	ENGINE::CCollider*		m_pCollider;

	ENGINE::CCameraSubject*	m_pCameraSubject;
	CCameraObserver*		m_pCameraObserver;

	wstring					m_wstrTex;
	float					m_fSizeX;
	float					m_fSizeY;
	D3DXVECTOR3				m_vPos;
};

#define __TERRAIN_H__
#endif