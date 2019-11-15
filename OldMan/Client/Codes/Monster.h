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
private:
	typedef enum { MONSTER_IDLE, MONSTER_PATROLL, MONSTER_PURSUE,
		MONSTER_FIRE, MONSTER_ATTACK, MONSTER_DEAD, MONSTER_SHOT,
		MONSTER_SIT, MONSTER_END } Monster_State;
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
	void Monster_State_Set(Monster_State _state);   //상태
	void Monster_Range();                           // 범위
	void Monster_Idle();
public: 
	void Set_Target(CGameObject* _Target) { m_pTarget = _Target; };

public:
	static CMonster* Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _Target);
private:
	CGameObject*			m_pTarget;
	CCameraObserver*		m_pObserver;
	D3DXMATRIX              m_matView;
private:
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;
	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;
	ENGINE::CCollider*		m_pCollider;
	ENGINE::CCameraSubject*	m_pSubject;
	ENGINE::CBillborad*		m_pBillborad;

	float				m_fRange;                   // 범위
	float				m_MoveSpeed;                // 속도
	float				m_fMaxRange;                // 최대범위
	float				m_fMinRange;                // 최소범위

	D3DXVECTOR3				m_MonsterDir;			// 방향 
	D3DXVECTOR3				m_MonsterCroos;		    //외적 받을값


};

#define __MONSTER_H__
#endif