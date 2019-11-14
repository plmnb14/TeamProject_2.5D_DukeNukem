#pragma once

#ifndef __MONSTER_H__

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
	class CBillborad;
}
class CCameraObserver;
class CMonster : public ENGINE::CGameObject
{
private:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CMonster();

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
	void Player_Pursue();  //추격하다 
	void Billborad_Front();
	void Billborad_YAngle();
public: 
	void Set_Target(CGameObject* _Target) { m_pTarget = _Target; };

public:
	static CMonster* Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _Target);
private:
	CGameObject*			m_pTarget;
	CCameraObserver*		m_pObserver;
	D3DXMATRIX              matView;
private:
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;
	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;
	ENGINE::CCollider*		m_pCollider;
	ENGINE::CCameraSubject*	m_pSubject;
	ENGINE::CBillborad*		m_pBillborad;




};

#define __MONSTER_H__
#endif