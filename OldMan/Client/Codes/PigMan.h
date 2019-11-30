#pragma once

#ifndef __PIGMAN_H__

#include "Monster.h"
//총알 샷건 처럼 5발 
class CPigMan : public CMonster
{
private:
	explicit CPigMan(LPDIRECT3DDEVICE9 pGraphicDev);


public:
	virtual ~CPigMan();

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
	virtual void Player_Pursue(float _move)override;  //추격하다 
	virtual void Monster_Foward()override;
	virtual void Monster_State_Set();   //상태
								//	void Monster_State_Set2();   //상태
	virtual void Monster_Range()override;                           // 범위
	virtual void Monster_Idle()override;
	virtual void Monster_Shot()override;
	//	void Object_Serch();
	virtual void Monster_Fire2()override;
	virtual void Monster_Dead()override;
	virtual void Monster_Attack()override;
public:											//물리 
	virtual void Check_Physic()override;
	virtual void Object_Collison()override;



public:
	void Set_Target(CGameObject* _Target) { m_pTarget = _Target; };
	void Set_Pos(D3DXVECTOR3 _Pos);

public:
	static CPigMan* Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _Target);

};

#define __PIGMAN_H__
#endif