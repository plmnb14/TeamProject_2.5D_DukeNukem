#pragma once

#ifndef __WEAPON_ROCKET__

#include "Weapon.h"

class CWeapon_Rocket : public CWeapon
{
private:
	explicit CWeapon_Rocket(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CWeapon_Rocket();

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

public:
	void Set_Pos(D3DXVECTOR3 _Pos);

public:
	void Physic();

public:
	static CWeapon_Rocket* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos);
};

#define __WEAPON_ROCKET__
#endif