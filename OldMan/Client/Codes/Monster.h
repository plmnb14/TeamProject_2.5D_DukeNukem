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

}
class CCameraObserver;
class CMonster : public ENGINE::CGameObject
{
private:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	 enum STATE { MONSTER_IDLE, MONSTER_PATROLL, MONSTER_PURSUE,
		MONSTER_FIRE, MONSTER_MILL, MONSTER_DEAD, MONSTER_SHOT,
		MONSTER_SIT, MONSTER_END } ;
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
	void Monster_Foward();
	void Monster_State_Set();   //����
//	void Monster_State_Set2();   //����
	void Monster_Range();                           // ����
	void Monster_Idle();
	void Monster_Shot();
//	void Object_Serch();
	void Monster_Fire();
	void Monster_Bogan();
private:											//���� 
	void Check_Physic();
	void Object_Collison();


	
public: 
	void Set_Target(CGameObject* _Target) { m_pTarget = _Target; };
	void Set_Pos(D3DXVECTOR3 _Pos);

public:
	static CMonster* Create(LPDIRECT3DDEVICE9 pGraphicDev ,CGameObject* _Target);
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
	ENGINE::CCollider*		m_pMelleCollider;

	ENGINE::CCameraSubject*	m_pSubject;
	ENGINE::CBillborad*		m_pBillborad;
	ENGINE::CRigidBody*		m_pRigid;
	ENGINE::CCondition*		m_pCondition;
	ENGINE::CCollider*		m_pGroundChekCollider;


	float				m_fRange;                   // ����
	float				m_MoveSpeed;                // �ӵ�
	float				m_fMaxRange;                // �ִ����
	float				m_fMinRange;                // �ּҹ���
	float				m_fTime;
	D3DXVECTOR3				m_MonsterDir;			// ���� 
	D3DXVECTOR3				m_MonsterCroos;		    //���� ������
	
	STATE m_eCurState;
	STATE m_eNextState;


	STATE m_eCurState2;
	STATE m_eNextState2;

	bool m_bShot;
	wstring					m_wstrTex;
	float					m_fSizeX;
	float					m_fSizeY;
	float m_fFrame;
	D3DXVECTOR3				m_vPos;
	bool					m_bObject;

};

#define __MONSTER_H__
#endif