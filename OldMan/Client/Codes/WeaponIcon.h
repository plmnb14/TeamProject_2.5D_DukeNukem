#pragma once

#ifndef __WEAPONICON_H__

#include "UI.h"
#include "Player.h"

class CWeaponIcon : public CUI
{
protected:

protected:
	explicit CWeaponIcon(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CWeaponIcon();

public:
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

protected:
	virtual HRESULT Initialize() override;
	virtual HRESULT LateInit() override;
	virtual void Release() override;

protected:
	void CheckWeapon();

public:
	static CWeaponIcon* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	ENGINE::WEAPON_TAG	m_eWeaponType;
	ENGINE::WEAPON_TAG	m_eWeaponType_Old;
};

#define __AIM_H__
#endif