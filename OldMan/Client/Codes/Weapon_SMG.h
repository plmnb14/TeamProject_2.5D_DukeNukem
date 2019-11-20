#pragma once

#ifndef __WEAPON_SMG__

#include "Weapon.h"

class CWeapon_SMG : public CWeapon
{
private:
	explicit CWeapon_SMG(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CWeapon_SMG();

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
	static CWeapon_SMG* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos);
};

#define __WEAPON_SMG__
#endif