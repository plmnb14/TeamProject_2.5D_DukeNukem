#pragma once

#ifndef __GRAPHICDEV_H__

#include "Engine_Include.h"

// dllimport
// dllexport

BEGIN(ENGINE)

class ENGINE_DLL CGraphicDev
{
	DECLARE_SINGLETON(CGraphicDev)

public:
	enum DISPLAY_MODE { MODE_FULL, MODE_WIN };

private:
	CGraphicDev();
	~CGraphicDev();

public:
	LPDIRECT3DDEVICE9 GetDevice();

public:
	HRESULT InitDevice(
		HWND hWnd,
		const DWORD& dwWinCX,
		const DWORD& dwWinCY,
		DISPLAY_MODE eMode);
	void Render_Begin();
	void Render_End(HWND hWnd = nullptr);

private:
	void Release();

private:
	// IDirect3D9: ��ġ ������ ��ġ�� ������ ��ü�� ������ ���� Com��ü.
	LPDIRECT3D9				m_pSDK;

	// IDirect3DDevice9: �׷��� ��ġ�� �����ϴ� Com��ü.
	LPDIRECT3DDEVICE9		m_pGraphicDev;
};

END

#define __GRAPHICDEV_H__
#endif