#pragma once

#ifndef __WEAPON_H__

#include "GameObject.h"

namespace ENGINE
{
	class CResourceMgr;
	class CTimeMgr;
	class CVIBuffer;
	class CTexture;
	class CTransform;
	class CCollider;
	class CCameraSubject;
	class CRigidBody;
	class CBillborad;
}

class CCameraObserver;
class CWeapon : public ENGINE::CGameObject
{
protected:
	explicit CWeapon(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CWeapon();

public:
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

protected:
	virtual HRESULT Initialize() override;
	virtual HRESULT LateInit();
	virtual void Release() override;

public:
	static CWeapon* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;
	ENGINE::CCollider*		m_pCollider;
	ENGINE::CCollider*		m_pGroundChekCollider;
	ENGINE::CRigidBody*		m_pRigid;
	ENGINE::CTexture*		m_pTexture;

	ENGINE::W_INFO			m_pWInfo;

protected:
	CCameraObserver*		m_pObserver;
	ENGINE::CCameraSubject*	m_pSubject;
	ENGINE::CBillborad*		m_pBillborad;
	D3DXMATRIX              m_matView;
};

#define __WEAPON_H__
#endif