#pragma once

#ifndef __PLAYER_H__

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
}

class CCameraObserver;
class CPlayer : public ENGINE::CGameObject
{
private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CPlayer();

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
	void KeyInput();

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:	
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;
	ENGINE::CKeyMgr*		m_pKeyMgr;

	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;
	ENGINE::CCollider*		m_pCollider;
	ENGINE::CCollider*		m_pGroundChekCollider;
	ENGINE::CRigidBody*			m_pRigid;

	ENGINE::CCameraSubject*	m_pSubject;
	CCameraObserver*		m_pObserver;
};

#define __PLAYER_H__
#endif