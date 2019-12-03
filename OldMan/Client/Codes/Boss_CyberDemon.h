#pragma once

#ifndef __BOSS_CYBERDEMON_H__

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
class CBoss_CyberDemon : public ENGINE::CGameObject
{
public:
	enum CYBERDEMON_STATE
	{
		CYBER_MET, CYBER_WALK, CYBER_SLASH ,CYBER_DASH, CYBER_FIRE, CYBER_JUMP, CYBER_DEAD
	};

private:
	explicit CBoss_CyberDemon(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CBoss_CyberDemon();

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

	void Monster_Foward();
	void Monster_Callcul();

private:
	void Check_Physic();
	void State();
	void HittedSound();

private:
	void Walk();
	void Dash();
	void Slash();
	void Fire();
	void Jump();

private:

public:
	static CBoss_CyberDemon* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;

	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;
	ENGINE::CCollider*		m_pCollider;
	ENGINE::CCollider*		m_pGroundChekCollider;
	ENGINE::CCollider*		m_pMelleCollider;
	ENGINE::CRigidBody*		m_pRigid;
	ENGINE::CCondition*		m_pCondition;
	ENGINE::CONDITION		m_tCondition;
	ENGINE::CBillborad*		m_pBillboard;
	ENGINE::CAnimator*		m_pAnimator;

	ENGINE::VTX_TEX*		m_myVtx;

	ENGINE::CCameraSubject*	m_pSubject;
	CCameraObserver*		m_pObserver;
	ENGINE::CPlayerSubject*	m_pPlayerSubject;
	ENGINE::CBossSubject*	m_pBossSubject;

	D3DXMATRIX				m_matView;
	CYBERDEMON_STATE		m_eState;
	wstring					m_wstrTex;

	CGameObject*			m_pHpBar;

	float					m_fLifeTime;
	float					m_fFowardDealy;

	CGameObject*			m_pTarget;
	D3DXVECTOR3				m_vPos;
	D3DXVECTOR3				m_vMonster_Player_Dir;
	D3DXVECTOR3				m_vPlayer_Pos;
	D3DXVECTOR3				m_vMonster_Pos;
	D3DXVECTOR3				m_vPlayer_Pos_Top;
	D3DXVECTOR3				m_vMonsterDir_Fowrd_Get;

	bool					m_bIsFindPlayer;
	float					m_fRandomWalkDelay;
	D3DXVECTOR3				m_vRandomPos;

	float					m_fPatternChangeDelay;
	int						m_iPatternIndex;

	D3DXVECTOR3				m_vDashDir;
	float					m_fChargeDelay;
	bool					m_bIsCharging;
	bool					m_bIsChargeHitted;

	float					m_fSlashDelay;
	float					m_fFireDelay;

	bool					m_bIsJump;
	float					m_fJumpDelay;
	float					m_fAccel;

	float					m_fHittedSoundDelay;
	float					m_fOldHp;

	float					m_fWalkSoundDelay;
};

#define __BOSS_CYBERDEMON_H__
#endif