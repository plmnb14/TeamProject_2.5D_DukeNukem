#pragma once

#ifndef __VFX_H__

#include "GameObject.h"

namespace ENGINE
{
	class CResourceMgr;
	class CTimeMgr;
	class CVIBuffer;
	class CTexture;
	class CTransform;
	class CCollider;
	class CAnimator;
	class CBillborad;
	class CCameraSubject;
	class CRigidBody;
	class CCondition;
}

class CCameraObserver;
class CVfx : public ENGINE::CGameObject
{
protected:
	explicit CVfx(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CVfx();

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

public:
	static CVfx* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	ENGINE::CManagement*	m_pManagement;
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;
	ENGINE::CRigidBody*		m_pRigid;

	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;
	ENGINE::CCollider*		m_pCollider;
	ENGINE::CAnimator*		m_pAnimator;
	ENGINE::CBillborad*		m_pBillborad;
	ENGINE::CCameraSubject*	m_pSubject;
	CCameraObserver*		m_pObserver;
	D3DXMATRIX              m_matView;

	ENGINE::TERRAIN_TYPE	m_eTerrainType;
	ENGINE::VTX_TEX*		m_myVtx;

	wstring					m_wstrTex;
};

#define __VFX_H__
#endif