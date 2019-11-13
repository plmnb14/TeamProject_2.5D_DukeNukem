#include "GraphicDev.h"

USING(ENGINE)

IMPLEMENT_SINGLETON(CGraphicDev)

CGraphicDev::CGraphicDev()
{
}

CGraphicDev::~CGraphicDev()
{
	Release();
}

LPDIRECT3DDEVICE9 CGraphicDev::GetDevice()
{
	return m_pGraphicDev;
}

LPD3DXSPRITE CGraphicDev::GetSprite()
{
	return m_pSprite;
}

HRESULT CGraphicDev::InitDevice(
	HWND hWnd, 
	const DWORD& dwWinCX, 
	const DWORD& dwWinCY, 
	DISPLAY_MODE eMode)
{
	// IDirect3D9 ��ü ����.
	m_pSDK = Direct3DCreate9(D3D_SDK_VERSION);
	NULL_CHECK_MSG_RETURN(m_pSDK, L"Direct3DCreate9 Failed", E_FAIL);

	// ��ġ ���� �� ���� ����.
	D3DCAPS9	d3dcaps;
	ZeroMemory(&d3dcaps, sizeof(D3DCAPS9));

	// HAL (Hardware Abstraction Layer, �ϵ���� �߻� ����)
	HRESULT hr = m_pSDK->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dcaps);
	FAILED_CHECK_MSG_RETURN(hr, L"GetDeviceCaps Failed", E_FAIL);

	// �ϵ���� ���ؽ� ���μ��� ���� ������ ����.
	// ���ؽ� ���μ���: ���� ó�� + ���� ó��
	DWORD vp = 0;

	if (d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// ������ ȯ���� ����.
	D3DPRESENT_PARAMETERS	d3dpp;
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	d3dpp.BackBufferWidth = dwWinCX;	// �ĸ���� ���γʺ�
	d3dpp.BackBufferHeight = dwWinCY;	// �ĸ���� ���γʺ�
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;	// �ĸ���� �ȼ�����
	d3dpp.BackBufferCount = 1;	// �ĸ���� ����

	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;

	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	// ����ü�� ������� �������� �ؼ�.
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.Windowed = eMode;

	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// IDirect3DDevice9 ��ü ����
	hr = m_pSDK->CreateDevice(
		D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, vp, &d3dpp, &m_pGraphicDev);
	FAILED_CHECK_MSG_RETURN(hr, L"CreateDevice Failed", E_FAIL);

	// m_pSprite ����
	hr = D3DXCreateSprite(m_pGraphicDev, &m_pSprite);
	FAILED_CHECK_MSG_RETURN(hr, L"D3DXCreateSprite Failed", E_FAIL);

	return S_OK;
}

void CGraphicDev::Render_Begin()
{
	// 1.�ĸ���۸� ����.
	m_pGraphicDev->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
		D3DCOLOR_ARGB(255, 0, 0, 255), 1.f, 0);

	// 2.�ĸ���ۿ� �׸���.
	m_pGraphicDev->BeginScene();
}

void CGraphicDev::Render_End(HWND hWnd)
{
	m_pGraphicDev->EndScene();

	// 4.�ĸ���۸� ������۷� ��ü�Ͽ� ȭ�鿡 �������Ѵ�.
	m_pGraphicDev->Present(nullptr, nullptr, hWnd, nullptr);
}

void CGraphicDev::Release()
{
	if(Safe_Release(m_pSprite))
		ERR_MSG(L"m_pSprite Release Failed");

	if (Safe_Release(m_pGraphicDev))
		ERR_MSG(L"m_pGraphicDev Release Failed");

	if (Safe_Release(m_pSDK))
		ERR_MSG(L"m_pSDK Release Failed");
}
