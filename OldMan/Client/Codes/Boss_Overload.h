#pragma once

#ifndef __BOSS_OVERLOAD_H__

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
	class CPlayerSubject;
	class CRigidBody;
	class CCondition;
	class CBillborad;
	class CAnimator;
}

class CCameraObserver;
class CBoss_Overload : public ENGINE::CGameObject
{
public:
	enum OVERLOAD_STATE
	{
		OVER_MET, OVER_IDLE, OVER_WALK, OVER_RUSH, OVER_MISSILE_DIRECT, OVER_MISSILE_HOMMING, OVER_MISSILE_RAIN, OVER_DEAD
	};

private:
	explicit CBoss_Overload(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CBoss_Overload();

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
	void ChangeTex(wstring _wstrTex);

private:
	void Check_Physic();
	void State();

private:
	void Walk();
	void Direct_Missile();

private:
	void Shot_Direct();
	void Shot_Homing();
	void Shot_Rain();

public:
	static CBoss_Overload* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;

	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;
	ENGINE::CCollider*		m_pCollider;
	ENGINE::CCollider*		m_pGroundChekCollider;
	ENGINE::CRigidBody*		m_pRigid;
	ENGINE::CCondition*		m_pCondition;
	ENGINE::CBillborad*		m_pBillboard;
	ENGINE::CAnimator*		m_pAnimator;

	ENGINE::VTX_TEX*		m_myVtx;

	ENGINE::CCameraSubject*	m_pSubject;
	CCameraObserver*		m_pObserver;
	ENGINE::CPlayerSubject*	m_pPlayerSubject;

	D3DXMATRIX	m_matView;
	OVERLOAD_STATE	m_eOverState;
	wstring	m_wstrTex;

	float m_fLifeTime;

};

#define __BOSS_OVERLOAD_H__
#endif