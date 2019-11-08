#include "TimeMgr.h"

USING(ENGINE)

IMPLEMENT_SINGLETON(CTimeMgr)

CTimeMgr::CTimeMgr()
	: m_fDeltaTime(0.f)
{
	// �ϵ��� �ִ� ���ػ� Ÿ�̸� ���� ���� ������ �Լ�.
	// �� �� Ÿ�̸Ӵ� CPU�� Clock ���� ������� �Ѵ�.
	QueryPerformanceCounter(&m_CurTime);
	QueryPerformanceCounter(&m_OldTime);

	// CPU�� �ʴ� ������(Hz)�� ������ �Լ�.
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
