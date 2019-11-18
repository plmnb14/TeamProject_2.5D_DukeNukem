#pragma once

#ifndef __CRIGIDBODY_H__

#include "Component.h"

BEGIN(ENGINE)

class ENGINE_DLL CRigidBody : public CComponent
{
private:
	explicit CRigidBody();

public:
	virtual ~CRigidBody();

public:
	virtual void LateUpdate();

public:
	void Set_UseGravity(bool _UseGravity);
	void Set_IsGround(bool _Ground);
	void Set_IsAir(bool _Air);
	void Set_IsHit(bool _Hit);
	void Set_IsAttack(bool _Attack);
	void Set_IsFall(bool _Fall);
	void Set_IsJump(bool _Jump);

public:
	void Set_fMass(float _Mass);
	void Set_fPower(float _Power);

public:
	void Set_Speed(D3DXVECTOR3 _Speed = { 1.f, 1.f, 1.f });
	void Set_Accel(D3DXVECTOR3 _Accel = { 0.f, 0.f, 0.f });
	void Set_MaxAccel(D3DXVECTOR3 _MaxAccel = { 0.1f, 0.1f, 0.1f });

public:
	D3DXVECTOR3 Get_Accel() { return m_tRigid.vAccel; }
	bool Get_IsJump() { return m_tRigid.bIsJump; }
	bool Get_IsGround() { return m_tRigid.bIsGround; }
	bool Get_IsFall() { return m_tRigid.bIsFall; }
	bool Get_IsAir() { return m_tRigid.bIsAir; }

public:
	float Set_Jump(D3DXVECTOR3 _TransForm, float _Time);
	float Set_Fall(D3DXVECTOR3 _TransForm, float _Time);


public:
	static CRigidBody* Create();

private:
	RIGID m_tRigid;
};

END

#define __CRIGIDBODY_H__
#endif