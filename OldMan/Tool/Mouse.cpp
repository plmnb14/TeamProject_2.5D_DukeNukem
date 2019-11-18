#include "stdafx.h"
#include "Mouse.h"

CMouse::CMouse()
{
}


CMouse::~CMouse()
{
}

D3DXVECTOR3 CMouse::GetMouse()
{
	POINT pt = {};

	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	return D3DXVECTOR3((float)pt.x, (float)pt.y, 0.f);
}