#pragma once

#ifndef __TROOPER_H__

#include "Monster.h"

class CTrooper : public CMonster
{
private:
	explicit CTrooper(LPDIRECT3DDEVICE9 pGraphicDev);


public:
	virtual ~CTrooper();

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
	void Player_Pursue(float _move);  //�߰��ϴ� 
	void Monster_Foward();
	void Monster_State_Set();   //����
								//	void Monster_State_Set2();   //����
	void Monster_Range();                           // ����
	void Monster_Idle();
	void Monster_Shot();
	//	void Object_Serch();
	void Monster_Fire2();
	void Monster_Dead();
	void Monster_Attack();
public:											//���� 
	void Check_Physic();
	void Object_Collison();
	void ChangeTex(wstring _wstrTex);



public:
	void Set_Target(CGameObject* _Target) { m_pTarget = _Target; };
	void Set_Pos(D3DXVECTOR3 _Pos);

public:
	static CTrooper* Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _Target);

};

#define __TROOPER_H__
#endif