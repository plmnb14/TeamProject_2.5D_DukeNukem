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
	void Set_IsLanding(bool _Land);

public:
	void Set_fMass(float _Mass);
	void Set_fPower(float _Power);
	void Set_fForce(float _Force);

public:
	void Set_Speed(D3DXVECTOR3 _Speed = { 1.f, 1.f, 1.f });
	void Set_Accel(D3DXVECTOR3 _Accel = { 0.f, 0.f, 0.f });
	void Set_MaxAccel(D3DXVECTOR3 _MaxAccel = { 0.1f, 0.1f, 0.1f });
	void Set_MaxSpeed(D3DXVECTOR3 _MaxSpeed = { 1.f, 1.f, 1.f });

public:
	D3DXVECTOR3 Get_Accel() { return m_tRigid.vAccel; }
	D3DXVECTOR3 Get_MaxAccel() { return m_tRigid.vMaxAccel; }
	D3DXVECTOR3 Get_Speed() { return m_tRigid.vSpeed; }
	D3DXVECTOR3 Get_MaxSpeed() { return m_tRigid.vMaxSpeed; }
	bool Get_IsJump() { return m_tRigid.bIsJump; }
	bool Get_IsGround() { return m_tRigid.bIsGround; }
	bool Get_IsFall() { return m_tRigid.bIsFall; }
	bool Get_IsAir() { return m_tRigid.bIsAir; }
	bool Get_IsHit() { return m_tRigid.bIsHit; }
	bool Get_IsLanding() { return m_tRigid.bIsLanding; }
public:
	float Get_Force() { return m_tRigid.fForce; }
	

public:
	float Get_Force() { return m_tRigid.fForce; }

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