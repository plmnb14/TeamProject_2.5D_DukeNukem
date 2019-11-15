#pragma once

#ifndef __NUMBER_H__

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
class CNumber : public CUI
{
public:
	enum NUMBER_TYPE { NUMBER_HP, NUMBER_BULLET, NUMBER_END };

protected:
	explicit CNumber(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CNumber();

public:
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

protected:
	virtual HRESULT Initialize() override;
	virtual HRESULT LateInit() override;
	virtual void Release() override;

public:
	virtual void ChangeTex(wstring _wstrTex);

protected:
	HRESULT AddComponent();

public:
	static CNumber* Create(LPDIRECT3DDEVICE9 pGraphicDev, NUMBER_TYPE _eType);

private:
	int*					m_iNumArr;
	int						m_iArrCount;

	ENGINE::CPlayerSubject*	m_pSubject;
	CPlayerObserver*		m_pObserver;
	NUMBER_TYPE				m_eNumberType;
};

#define __NUMBER_H__
#endif