#include "KeyMgr.h"

USING(ENGINE)

IMPLEMENT_SINGLETON(CKeyMgr)

CKeyMgr::CKeyMgr()
	: m_dwKey(0), m_dwKeyDown(0), m_dwKeyUp(0),
	m_CurMousePos({0,0}) , m_OldMousePos({0,0})
{

}

CKeyMgr::~CKeyMgr()
{

}

void CKeyMgr::Update()
{
	m_dwKey = 0;

	if (GetAsyncKeyState(VK_UP) & 0x8000)
		m_dwKey |= KEY_UP;
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		m_dwKey |= KEY_DOWN;
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		m_dwKey |= KEY_LEFT;
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		m_dwKey |= KEY_RIGHT;
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		m_dwKey |= KEY_SPACE;
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		m_dwKey |= KEY_RETURN;
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		m_dwKey |= KEY_LBUTTON;
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
		m_dwKey |= KEY_RBUTTON;
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		m_dwKey |= KEY_LSHIFT;
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
		m_dwKey |= KEY_LCTRL;

	if (GetAsyncKeyState('W') & 0x8000)
		m_dwKey |= KEY_W;
	if (GetAsyncKeyState('S') & 0x8000)
		m_dwKey |= KEY_S;
	if (GetAsyncKeyState('A') & 0x8000)
		m_dwKey |= KEY_A;
	if (GetAsyncKeyState('D') & 0x8000)
		m_dwKey |= KEY_D;

	if (GetAsyncKeyState('Q') * 0x8000)
		m_dwKey |= KEY_Q;

	if (GetAsyncKeyState('E') * 0x8000)
		m_dwKey |= KEY_E;

	if (GetAsyncKeyState('R') * 0x8000)
		m_dwKey |= KEY_R;

	if (GetAsyncKeyState('F') * 0x8000)
		m_dwKey |= KEY_F;

	if (GetAsyncKeyState('X') * 0x8000)
		m_dwKey |= KEY_X;

	if (GetAsyncKeyState('Z') * 0x8000)
		m_dwKey |= KEY_Z;

	if (GetAsyncKeyState('1') * 0x8000)
		m_dwKey |= KEY_1;

	if (GetAsyncKeyState('2') * 0x8000)
		m_dwKey |= KEY_2;

	if (GetAsyncKeyState('3') * 0x8000)
		m_dwKey |= KEY_3;

	if (GetAsyncKeyState('4') * 0x8000)
		m_dwKey |= KEY_4;


	if (GetAsyncKeyState('5') * 0x8000)
		m_dwKey |= KEY_5;

	if (GetAsyncKeyState('6') * 0x8000)
		m_dwKey |= KEY_6;

	if (GetAsyncKeyState('7') * 0x8000)
		m_dwKey |= KEY_7;

	if (GetAsyncKeyState('8') * 0x8000)
		m_dwKey |= KEY_8;
}


bool CKeyMgr::KeyPressing(DWORD dwKey)
{
	if (m_dwKey & dwKey)
		return true;

	return false;
}

bool CKeyMgr::KeyDown(DWORD dwKey)
{
	if (!(m_dwKeyDown & dwKey) && (m_dwKey & dwKey))
	{
		m_dwKeyDown |= dwKey;
		return true;
	}
	else if ((m_dwKeyDown & dwKey) && !(m_dwKey & dwKey))
	{
		m_dwKeyDown ^= dwKey;
		return false;
	}

	return false;
}

bool CKeyMgr::KeyUp(DWORD dwKey)
{
	if ((m_dwKeyUp & dwKey) && !(m_dwKey & dwKey))
	{
		m_dwKeyUp ^= dwKey;
		return true;
	}
	else if (!(m_dwKeyUp & dwKey) && (m_dwKey & dwKey))
	{
		m_dwKeyUp |= dwKey;
		return false;
	}

	return false;
}

bool CKeyMgr::KeyCombined(DWORD dwFirst, DWORD dwSecond)
{
	if (KeyDown(dwSecond) && KeyPressing(dwFirst))
		return true;

	return false;
}

POINT CKeyMgr::Get_MouseGap(HWND _g_hWnd)
{
	GetCursorPos(&m_CurMousePos);
	ScreenToClient(_g_hWnd, &m_CurMousePos);

	m_MouseGap.x = (LONG)(m_CurMousePos.x - (float)WINCX * 0.5f);
	m_MouseGap.y = (LONG)(m_CurMousePos.y - (float)WINCY * 0.5f);

	return m_MouseGap;
}
