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
	class CRigidBody;
	class CCondition;
	class CAnimator;

}
class CCameraObserver;
class CMonster : public ENGINE::CGameObject
{
protected:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
protected:
	enum STATE {
		MONSTER_IDLE, MONSTER_PATROLL, MONSTER_PURSUE,
		MONSTER_FIRE, MONSTER_MILL, MONSTER_DEAD, MONSTER_SHOT,
		MONSTER_SIT, MONSTER_END
	};
public:
	virtual ~CMonster();

public:
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

protected:
	virtual HRESULT Initialize() override;
	virtual HRESULT LateInit();
	virtual void Release() override;

protected:
	virtual HRESULT AddComponent();
	virtual	void Player_Pursue(float _move);  //�߰��ϴ�    //������
	virtual	void Monster_Foward();
	virtual void Monster_State_Set();   //����
								
	virtual	void Monster_Range();                           // ������
	virtual void Monster_Idle();
	virtual	void Monster_Shot();
	//void Object_Serch();
	virtual void Monster_Fire2();                           // ��껩�� ���� �������̵� ����
	virtual void Monster_Dead();
	virtual void Monster_Attack();							// �������� �������̵�� ���Ǹ� �ٽ��ϴ°� �´�. 
	virtual void Monster_Callcul();                            // �����Լ��� Įŧ�� �ɰ� 
protected:											//���� 
	virtual void Check_Physic();
	virtual void Object_Collison();
	virtual void ChangeTex(wstring _wstrTex);



public:
	void Set_Target(CGameObject* _Target) { m_pTarget = _Target; };
	void Set_Pos(D3DXVECTOR3 _Pos);

public:
	static CMonster* Create(LPDIRECT3DDEVICE9 pGraphicDev ,CGameObject* _Target);
protected:
	CGameObject*			m_pTarget;
	CCameraObserver*		m_pObserver;
	D3DXMATRIX              m_matView;
	D3DXVECTOR3				m_vPos;
	D3DXVECTOR3				m_vMonster_Player_Dir;
	D3DXVECTOR3				m_vPlayer_Pos;
	D3DXVECTOR3				m_vMonster_Pos;
	D3DXVECTOR3				m_vPlayer_Pos_Top;
	D3DXVECTOR3				m_vMonsterDir_Fowrd_Get;



protected:
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;
	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;
	ENGINE::CCollider*		m_pCollider;
	ENGINE::CCollider*		m_pMelleCollider;

	ENGINE::CCameraSubject*	m_pSubject;
	ENGINE::CBillborad*		m_pBillborad;
	ENGINE::CRigidBody*		m_pRigid;
	ENGINE::CCondition*		m_pCondition;
	ENGINE::CCollider*		m_pGroundChekCollider;
	ENGINE::CAnimator*		m_pAnimator;

protected:
	float					m_fRange;                   // ����
	float					m_MoveSpeed;                // �ӵ�
	float					m_fMaxRange;                // �ִ����
	float					m_fMinRange;                // �ּҹ���
	float					m_fTime;					//�ð� ��ư�ð���
	float					m_fHitTime;					//shot�� �ð�
	float					m_fRangeTiem;				//
	float					m_fDelayTime;
	float					m_fSizeX;
	float       			m_fSizeY;
	float					m_fFrame;
	float					m_fFowardDealy;
	float					m_fAttack;

	STATE					m_eNextState;

	wstring					m_wstrTex;      //�ؽ�ó
	wstring					m_OldwstrTex;

	bool					m_bObject;
	bool					m_bShot;
	bool					m_bAttack;


};

#define __MONSTER_H__
#endif