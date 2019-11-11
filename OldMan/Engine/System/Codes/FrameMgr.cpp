#include "FrameMgr.h"

USING(ENGINE)

IMPLEMENT_SINGLETON(CFrameMgr)

CFrameMgr::CFrameMgr()
	: m_fTimeCount(0.f), m_fTimeCount2(0.f),
	m_iFPS(0), m_szFPS(L"")
{
	ZeroMemory(&m_OriginTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_OldTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_CurTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_CPUTick, sizeof(LARGE_INTEGER));
}


CFrameMgr::~CFrameMgr()
{
}

HRESULT CFrameMgr::InitTime()
{
	QueryPerformanceCounter(&m_OriginTime);
	QueryPerformanceCounter(&m_OldTime);
	QueryPerformanceCounter(&m_CurTime);
	QueryPerformanceFrequency(&m_CPUTick);

	return S_OK;
}

bool CFrameMgr::LockFrame(float fps)
{
	QueryPerformanceCounter(&m_CurTime);

	// 주기적으로 CPU 진동수를 새로 얻어오고 있다.
	if (m_CurTime.QuadPart - m_OriginTime.QuadPart > m_CPUTick.QuadPart)
	{
		QueryPerformanceCounter(&m_OriginTime);
		QueryPerformanceFrequency(&m_CPUTick);
	}

	m_fTimeCount += float(m_CurTime.QuadPart - m_OldTime.QuadPart)
		/ m_CPUTick.QuadPart;
	m_OldTime = m_CurTime;

	if (1.f / fps <= m_fTimeCount)
	{
		++m_iFPS;
		m_fTimeCount = 0.f;
		return true;
	}

	return false;
}
