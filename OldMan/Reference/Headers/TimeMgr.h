#pragma once

#ifndef __TIMEMGR_H__

#include "Engine_Include.h"

BEGIN(ENGINE)

class ENGINE_DLL CTimeMgr
{
	DECLARE_SINGLETON(CTimeMgr)

private:
	explicit CTimeMgr();
	~CTimeMgr();

public:
	float GetDelta();

public:
	void UpdateTime();

private:
	LARGE_INTEGER	m_CurTime;
	LARGE_INTEGER	m_OldTime;
	LARGE_INTEGER	m_CpuTick;
	float			m_fDeltaTime;
};

END

#define __TIMEMGR_H__
#endif