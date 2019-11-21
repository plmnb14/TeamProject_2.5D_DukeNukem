#pragma once

#ifndef __GAUGEBAR_H__

#include "UI.h"

namespace ENGINE
{
	class CResourceMgr;
	class CTimeMgr;
	class CVIBuffer;
	class CTexture;
	class CTransform;
	class CCollider;
	class CPlayerSubject;
}

class CPlayerObserver;
class CGaugeBar : public CUI
{
public:
	enum GAUGEBAR_TYPE { BAR_HP, BAR_SHIELD, BAR_END };

protected:
	explicit CGaugeBar(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CGaugeBar();

public:
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

protected:
	virtual HRESULT Initialize() override;
	virtual HRESULT LateInit() override;
	virtual void Release() override;

protected:
	HRESULT AddComponent();
	void InitBar();
	void UpdateBar();

public:
	static CGaugeBar* Create(LPDIRECT3DDEVICE9 pGraphicDev, GAUGEBAR_TYPE _eType);

private:
	vector<CUI*>			m_vecBarUI;
	int						m_iBarMaxCount;
	int						m_iBarCurCount;

	float					m_fHP;
	float					m_fShield;

	GAUGEBAR_TYPE			m_eBarType;

	int m_iTestMaxHP;
	int m_iTestHP;
	int m_iTestMaxShield;
	int m_iTestShield;
};

#define __GAUGEBAR_H__
#endif