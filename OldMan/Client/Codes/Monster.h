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
	void Player_Pursue();  //�߰��ϴ� 
	void Monster_State_Set(Monster_State _state);   //����
	void Monster_Range();                           // ����
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

	float				m_fRange;                   // ����
	float				m_MoveSpeed;                // �ӵ�
	float				m_fMaxRange;                // �ִ����
	float				m_fMinRange;                // �ּҹ���

	D3DXVECTOR3				m_MonsterDir;			// ���� 
	D3DXVECTOR3				m_MonsterCroos;		    //���� ������


};

#define __MONSTER_H__
#endif