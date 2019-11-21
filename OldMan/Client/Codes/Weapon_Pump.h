#pragma once

#ifndef __WEAPON_PUMP__

#include "Weapon.h"

class CWeapon_Pump : public CWeapon
{
private:
	explicit CWeapon_Pump(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CWeapon_Pump();

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
	static CWeapon_Pump* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos);
};

#define __WEAPON_PUMP__
#endif