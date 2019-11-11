#pragma once

#ifndef __FRAMEMGR_H__

#include "Engine_Include.h"

BEGIN(ENGINE)

class ENGINE_DLL CFrameMgr
{
	DECLARE_SINGLETON(CFrameMgr)

private:
	explicit CFrameMgr();
	~CFrameMgr();

public:
	HRESULT InitTime();
	bool LockFrame(float fps);

private:
	LARGE_INTEGER	m_OriginTime;
	LARGE_INTEGER	m_OldTime;
	LARGE_INTEGER	m_CurTime;
	LARGE_INTEGER	m_CPUTick;

	float			m_fTimeCount;
	float			m_fTimeCount2;

	int				m_iFPS;
	TCHAR			m_szFPS[MIN_STR];
};

END

#define __FRAMEMGR_H__
#endif

