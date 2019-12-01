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
	m_pTextureMgr(ENGINE::GetTextureMgr()),
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


HRESULT CMainApp::Initialize()
{
	HRESULT hr = m_pDeviceMgr->InitDevice(
		g_hWnd, WINCX, WINCY, ENGINE::CGraphicDev::MODE_WIN);
	FAILED_CHECK_MSG_RETURN(hr, L"InitDevice Failed", E_FAIL);

	hr = m_pManagement->InitManagement(
		m_pDeviceMgr->GetDevice());
	FAILED_CHECK_MSG_RETURN(hr, L"InitManagement Failed", E_FAIL);

	CSoundMgr::GetInstance()->Initialize();
	
	hr = m_pManagement->SceneChange(CSceneSelector(CSceneSelector::LOGO));
	FAILED_CHECK_MSG_RETURN(hr, L"Logo Scene Change Failed", E_FAIL);	

	return S_OK;
}

void CMainApp::Release()
{		
	CSoundMgr::GetInstance()->DestroyInstance();
	m_pTextureMgr->DestroyInstance();
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


