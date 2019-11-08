#include "TimeMgr.h"

USING(ENGINE)

IMPLEMENT_SINGLETON(CTimeMgr)

CTimeMgr::CTimeMgr()
	: m_fDeltaTime(0.f)
{
	// 하드웨어에 있는 고해상도 타이머 누적 값을 얻어오는 함수.
	// 이 때 타이머는 CPU의 Clock 수를 기반으로 한다.
	QueryPerformanceCounter(&m_CurTime);
	QueryPerformanceCounter(&m_OldTime);

	// CPU의 초당 진동수(Hz)를 얻어오는 함수.
	QueryPerformanceFrequency(&m_CpuTick);
}


CTimeMgr::~CTimeMgr()
{
}

float CTimeMgr::GetDelta()
{
	return m_fDeltaTime;
}

void CTimeMgr::UpdateTime()
{
	QueryPerformanceCounter(&m_CurTime);

	if (m_CurTime.QuadPart - m_OldTime.QuadPart > m_CpuTick.QuadPart)
	{
		QueryPerformanceCounter(&m_CurTime);
		QueryPerformanceCounter(&m_OldTime);
		QueryPerformanceFrequency(&m_CpuTick);
	}

	m_fDeltaTime =
		float(m_CurTime.QuadPart - m_OldTime.QuadPart) / m_CpuTick.QuadPart;

	m_OldTime.QuadPart = m_CurTime.QuadPart;
}
