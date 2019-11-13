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
	// IDirect3D9 객체 생성.
	m_pSDK = Direct3DCreate9(D3D_SDK_VERSION);
	NULL_CHECK_MSG_RETURN(m_pSDK, L"Direct3DCreate9 Failed", E_FAIL);

	// 장치 검증 및 조사 진행.
	D3DCAPS9	d3dcaps;
	ZeroMemory(&d3dcaps, sizeof(D3DCAPS9));

	// HAL (Hardware Abstraction Layer, 하드웨어 추상 계층)
	HRESULT hr = m_pSDK->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dcaps);
	FAILED_CHECK_MSG_RETURN(hr, L"GetDeviceCaps Failed", E_FAIL);

	// 하드웨어 버텍스 프로세싱 지원 유무를 조사.
	// 버텍스 프로세싱: 정점 처리 + 조명 처리
	DWORD vp = 0;

	if (d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// 렌더링 환경을 구성.
	D3DPRESENT_PARAMETERS	d3dpp;
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	d3dpp.BackBufferWidth = dwWinCX;	// 후면버퍼 가로너비
	d3dpp.BackBufferHeight = dwWinCY;	// 후면버퍼 세로너비
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;	// 후면버퍼 픽셀포맷
	d3dpp.BackBufferCount = 1;	// 후면버퍼 개수

	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;

	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	// 스왑체인 방식으로 깜빡임을 해소.
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.Windowed = eMode;

	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// IDirect3DDevice9 객체 생성
	hr = m_pSDK->CreateDevice(
		D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, vp, &d3dpp, &m_pGraphicDev);
	FAILED_CHECK_MSG_RETURN(hr, L"CreateDevice Failed", E_FAIL);

	// m_pSprite 생성
	hr = D3DXCreateSprite(m_pGraphicDev, &m_pSprite);
	FAILED_CHECK_MSG_RETURN(hr, L"D3DXCreateSprite Failed", E_FAIL);

	return S_OK;
}

void CGraphicDev::Render_Begin()
{
	// 1.후면버퍼를 비운다.
	m_pGraphicDev->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
		D3DCOLOR_ARGB(255, 0, 0, 255), 1.f, 0);

	// 2.후면버퍼에 그린다.
	m_pGraphicDev->BeginScene();
}

void CGraphicDev::Render_End(HWND hWnd)
{
	m_pGraphicDev->EndScene();

	// 4.후면버퍼를 전면버퍼로 교체하여 화면에 렌더링한다.
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
