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
	enum NUMBER_TYPE { NUMBER_HP, NUMBER_SHIELD, NUMBER_BULLET, NUMBER_GRENADE , NUMBER_END };

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
	void GetNumberArr(int _iNumber, int*& _iArr, int& _iCount, int _iNumber2 = -1);

public:
	static CNumber* Create(LPDIRECT3DDEVICE9 pGraphicDev, NUMBER_TYPE _eType);

private:
	CUI*					m_NumberUI[10];
	int*					m_iNumArr;
	int						m_iArrCount;

	NUMBER_TYPE				m_eNumberType;

	int m_iCurBullet;
	int m_iMaxBullet;
};

#define __NUMBER_H__
#endif