#pragma once

#ifndef __AIM_H__

#include "UI.h"
#include "Player.h"

class CAim : public CUI
{
protected:
	enum AIM_DIR { AIM_LEFT, AIM_RIGHT, AIM_UP, AIM_DOWM, AIM_END };

protected:
	explicit CAim(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CAim();

public:
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

protected:
	virtual HRESULT Initialize() override;
	virtual HRESULT LateInit() override;
	virtual void Release() override;

protected:
	void CheckWeaponAim();
	void CheckMoveRifleAim();
	void CheckMoveShotgunAim();
	void InitRifleAim();
	void InitShotgunAim();

public:
	static CAim* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	float				m_fMaxMoveAim;
	float				m_fMoveAim;

	CUI*				m_RifleAimArr[AIM_END];
	D3DVECTOR			m_RifleAimOriPos[AIM_END];

	CUI*				m_ShotgunAimArr[AIM_END];
	D3DVECTOR			m_ShotgunAimOriPos[AIM_END];

	ENGINE::WEAPON_TAG	m_eWeaponType;
	ENGINE::WEAPON_TAG	m_eWeaponType_Old;
};

#define __AIM_H__
#endif