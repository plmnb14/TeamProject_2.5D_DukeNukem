#pragma once

#ifndef __WEAPON_REVOLVER__

#include "Weapon.h"

class CWeapon_Revolver : public CWeapon
{
private:
	explicit CWeapon_Revolver(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CWeapon_Revolver();

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
	static CWeapon_Revolver* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos);
};

#define __WEAPON_REVOLVER__
#endif