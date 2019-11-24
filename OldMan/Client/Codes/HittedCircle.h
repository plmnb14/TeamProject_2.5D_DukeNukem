#pragma once

#ifndef __HITTEDCIRCLE_H__

#include "UI.h"
#include "Player.h"

class CHittedCircle : public CUI
{
public:
	enum HITTED_SIZE { SIZE_XS, SIZE_S, SIZE_M, SIZE_L, SIZE_XL, SIZE_END };

protected:
	explicit CHittedCircle(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CHittedCircle();

public:
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

protected:
	virtual HRESULT Initialize() override;
	virtual HRESULT LateInit() override;
	virtual void Release() override;

public:
	static CHittedCircle* Create(LPDIRECT3DDEVICE9 pGraphicDev, HITTED_SIZE _eSize);

protected:
	void SetCircle();
	void CheckLifeTime();

protected:
	float m_fLifeTime;
	HITTED_SIZE m_eSize;
};

#define __HITTEDCIRCLE_H__
#endif