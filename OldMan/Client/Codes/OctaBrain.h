#pragma once

#ifndef __OctaBrain_H__

#include "Monster.h"

class COctaBrain : public CMonster
{
private:
	explicit COctaBrain(LPDIRECT3DDEVICE9 pGraphicDev);

	
public:
	virtual ~COctaBrain();

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
	virtual void Player_Pursue(float _move)override;  //�߰��ϴ� 
	virtual void Monster_Foward()override;
	virtual void Monster_State_Set();   //����

	virtual void Monster_Range()override;                           // ����
	virtual void Monster_Idle()override;
	virtual void Monster_Shot()override;
	//	void Object_Serch();
	virtual void Monster_Fire2()override;
	virtual void Monster_Dead()override;
	virtual void Monster_Attack()override;
public:											//���� 
	virtual void Check_Physic()override;
	virtual void Object_Collison()override;



public:
	void Set_Target(CGameObject* _Target) { m_pTarget = _Target; };
	void Set_Pos(D3DXVECTOR3 _Pos);

public:
	static COctaBrain* Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _Target);

};

#define __OctaBrain_H__
#endif