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
	void Player_Pursue(float _move);  //추격하다 
	void Monster_Foward();
	void Monster_State_Set();   //상태
								//	void Monster_State_Set2();   //상태
	void Monster_Range();                           // 범위
	void Monster_Idle();
	void Monster_Shot();
	//	void Object_Serch();
	void Monster_Fire2();
	void Monster_Dead();
	void Monster_Attack();
public:											//물리 
	void Check_Physic();
	void Object_Collison();
	void ChangeTex(wstring _wstrTex);



public:
	void Set_Target(CGameObject* _Target) { m_pTarget = _Target; };
	void Set_Pos(D3DXVECTOR3 _Pos);

public:
	static COctaBrain* Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _Target);

};

#define __OctaBrain_H__
#endif