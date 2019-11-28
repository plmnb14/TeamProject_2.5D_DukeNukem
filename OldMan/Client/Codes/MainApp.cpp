#include "stdafx.h"
#include "MainApp.h"
#include "SceneSelector.h"
#include "SoundMgr.h"

CMainApp::CMainApp()
	: m_pDeviceMgr(ENGINE::GetGraphicDev()),
	m_pManagement(ENGINE::GetManagement()),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pKeyMgr(ENGINE::GetKeyMgr()),
	m_fTimeCount(0) , m_iFps(0)
	
{
}

CMainApp::~CMainApp()
{	
	Release();
}

void CMainApp::Update()
{
	m_pTimeMgr->UpdateTime();
	m_pKeyMgr->Update();
	m_pManagement->Update();
}

void CMainApp::LateUpdate()
{
	m_pManagement->LateUpdate();
}

void CMainApp::Render()
{
	m_pManagement->Render();
}

void CMainApp::FrameRender()
{
	//m_fTimeCount += m_pTimeMgr->GetDelta();
	//
	//// 1초가 지났다.
	//if (1.f <= m_fTimeCount)
	//{
	//	swprintf_s(m_szFPS, L"FPS: %d", m_iFps);
	//	m_iFps = 0;
	//	m_fTimeCount = 0.f;
	//}
	//
	//D3DXMATRIX matTrans;
	//D3DXMatrixTranslation(&matTrans, 600.f, 100.f, 0.f);
	//
	//CDeviceMgr::GetInstance()->GetSprite()->SetTransform(&matTrans);
	//
	//// 다이렉트 폰트 출력
	//CDeviceMgr::GetInstance()->GetFont()->DrawText(
	//	CDeviceMgr::GetInstance()->GetSprite(), /* 다이렉트 폰트는 텍스처 기반이다. */
	//	m_szFPS, lstrlen(m_szFPS), nullptr, 0,
	//	D3DCOLOR_ARGB(255, 0, 255, 0));
}

HRESULT CMainApp::Initialize()
{
	HRESULT hr = m_pDeviceMgr->InitDevice(
		g_hWnd, WINCX, WINCY, ENGINE::CGraphicDev::MODE_WIN);
	FAILED_CHECK_MSG_RETURN(hr, L"InitDevice Failed", E_FAIL);

	hr = m_pManagement->InitManagement(
		m_pDeviceMgr->GetDevice());
	FAILED_CHECK_MSG_RETURN(hr, L"InitManagement Failed", E_FAIL);

	hr = m_pManagement->SceneChange(CSceneSelector(CSceneSelector::LOGO));
	FAILED_CHECK_MSG_RETURN(hr, L"Logo Scene Change Failed", E_FAIL);	

	CSoundMgr::GetInstance()->Initialize();


	return S_OK;
}

void CMainApp::Release()
{		
	m_pTimeMgr->DestroyInstance();
	m_pManagement->DestroyInstance();
	m_pResourceMgr->DestroyInstance();
	m_pDeviceMgr->DestroyInstance();
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp;

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}


