#pragma once
#ifndef __SKYBOX_H__

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

class CSkybox : public ENGINE::CGameObject
{
protected:
	explicit CSkybox(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CSkybox();

public:
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

protected:
	virtual HRESULT Initialize() override;
	virtual void Release() override;

public:
	void ChangeTex(wstring _wstrTex);

protected:
	HRESULT AddComponent();

public:
	static CSkybox* Create(LPDIRECT3DDEVICE9 pGraphicDev, wstring _wstrTex, ENGINE::CGameObject* pPlayer);

protected:
	ENGINE::CManagement*	m_pManagement;
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;

	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;

	ENGINE::VTX_TEX*		m_myVtx;

	ENGINE::CGameObject*	m_pPlayer;
	wstring					m_wstrTex;

};


#define __SKYBOX_H__
#endif