#pragma once

#ifndef __WEAPON_REVOLVER__

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
class CWeapon_Revolver : public ENGINE::CGameObject
{
private:
	explicit CWeapon_Revolver(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CWeapon_Revolver();

public:
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

private:
	virtual HRESULT Initialize() override;
	virtual HRESULT LateInit();
	virtual void Release() override;

private:
	HRESULT AddComponent();

public:
	void Set_Pos(D3DXVECTOR3 _Pos);

public:
	void Physic();

public:
	static CWeapon_Revolver* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos);

private:
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;
	ENGINE::CCollider*		m_pCollider;
	ENGINE::CCollider*		m_pGroundChekCollider;
	ENGINE::CRigidBody*		m_pRigid;
	ENGINE::CTexture*		m_pTexture;

	ENGINE::W_INFO			m_pWInfo;

private:
	CCameraObserver*		m_pObserver;
	ENGINE::CCameraSubject*	m_pSubject;
	ENGINE::CBillborad*		m_pBillborad;
	D3DXMATRIX              m_matView;
};

#define __WEAPON_REVOLVER__
#endif