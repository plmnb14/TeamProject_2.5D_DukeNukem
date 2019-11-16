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

protected:
	HRESULT AddComponent();
	void GetNumberArr(int _iNumber, int*& _iArr, int& _iCount);

public:
	static CNumber* Create(LPDIRECT3DDEVICE9 pGraphicDev, NUMBER_TYPE _eType);

private:
	vector<CUI*>			m_vecNumberUI;
	int*					m_iNumArr;
	int						m_iArrCount;
	NUMBER_TYPE				m_eNumberType;

	ENGINE::CPlayerSubject*	m_pPlayerSubject;
	CPlayerObserver*		m_pPlayerObserver;
};

#define __NUMBER_H__
#endif