#include "stdafx.h"
#include "Logo.h"
#include "SceneSelector.h"

#include "LogoBack.h"

CLogo::CLogo(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CScene(pGraphicDev),
	m_pManagement(ENGINE::GetManagement()),
	m_pResourceMgr(ENGINE::GetResourceMgr())
{
}

CLogo::~CLogo()
{
	Release();
}

void CLogo::Update()
{
	system("cls");
	cout << ENGINE::GetResourceMgr()->Get_TextureCount() * 100 / ENGINE::GetTextureMgr()->Get_MaxTextureCount();
	cout << " / ";
	cout << ENGINE::GetTextureMgr()->Get_MaxTextureCount() / ENGINE::GetTextureMgr()->Get_MaxTextureCount() * 100 << endl;

	ENGINE::CScene::Update();

	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{		
		WaitForSingleObject(m_hLoadingThread, INFINITE);

		HRESULT hr = ENGINE::GetManagement()->SceneChange(CSceneSelector(CSceneSelector::STAGE));
		FAILED_CHECK_MSG(hr, L"STAGE Scene Change Failed");
	}
}

void CLogo::LateUpdate()
{
	ENGINE::CScene::LateUpdate();
}

void CLogo::Render()
{
	ENGINE::CScene::Render();
}

HRESULT CLogo::Add_Environment_Layer()
{
	return S_OK;
}

HRESULT CLogo::Add_Object_Layer()
{
	// Object Layer
	ENGINE::CLayer* pObject_Layer = ENGINE::CLayer::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pObject_Layer, E_FAIL);
	m_mapLayer.insert({ ENGINE::CLayer::OBJECT, pObject_Layer });

	
	return S_OK;
}

HRESULT CLogo::Add_UI_Layer()
{
	return S_OK;
}

HRESULT CLogo::Initialize()
{
	ENGINE::GetTextureMgr()->InitTextureMgr(ENGINE::GetGraphicDev()->GetDevice());

	m_hLoadingThread = (HANDLE)_beginthreadex(nullptr, 0,
		LoadingThreadFunc, this, 0, nullptr);
	NULL_CHECK_MSG_RETURN(m_hLoadingThread, L"LoadingThread Create Failed", E_FAIL);

	InitializeCriticalSection(&m_CriticalSection);

	PipeLineSetUp();

	// Environment Layer
	HRESULT hr = Add_Environment_Layer();
	FAILED_CHECK_RETURN(hr, E_FAIL);

	// Object Layer
	hr = Add_Object_Layer();
	FAILED_CHECK_RETURN(hr, E_FAIL);

	// UI Layer
	hr = Add_UI_Layer();
	FAILED_CHECK_RETURN(hr, E_FAIL);

	return S_OK;
}

void CLogo::Release()
{
	CloseHandle(m_hLoadingThread);
	DeleteCriticalSection(&m_CriticalSection);
}

map<WORD, ENGINE::CLayer*> CLogo::Get_MapLayer()
{
	return m_mapLayer;
}

void CLogo::PipeLineSetUp()
{
	// 조명 off
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

CLogo* CLogo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CLogo* pInstance = new CLogo(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}

unsigned CLogo::LoadingThreadFunc(void * pParam)
{
	CLogo* pLogo = reinterpret_cast<CLogo*>(pParam);

	EnterCriticalSection(&pLogo->m_CriticalSection);

	HRESULT hr = ENGINE::GetTextureMgr()->LoadTextureFromImgPath(L"../../Data/TexturePath_Client.txt");
	FAILED_CHECK_MSG(hr, L"LoadTextureFromImgPath Failed");

	for (auto& iter : ENGINE::GetTextureMgr()->GetMapTexture_Multi())
	{
		hr = ENGINE::CResourceMgr::GetInstance()->AddTexture(
			ENGINE::GetGraphicDev()->GetDevice(),
			ENGINE::RESOURCE_STATIC,
			ENGINE::TEX_NORMAL,
			iter->wstrStateKey,
			iter->wstrImgPath, iter->iImgCount);
		FAILED_CHECK_MSG(hr, iter->wstrFileName.c_str());
	}

	// Single은 FileName
	for (auto& iter : ENGINE::GetTextureMgr()->GetMapTexture_Single())
	{
		string strCheckDDS;
		strCheckDDS.assign(iter->wstrFileName.begin(), iter->wstrFileName.end());

		// .dds 를 찾았다면 TEX_CUBE
		if (strCheckDDS.find(".dds") != string::npos)
		{
			hr = ENGINE::CResourceMgr::GetInstance()->AddTexture(
				ENGINE::GetGraphicDev()->GetDevice(),
				ENGINE::RESOURCE_STATIC,
				ENGINE::TEX_CUBE,
				iter->wstrFileName,
				iter->wstrImgPath, 1);
			FAILED_CHECK_MSG(hr, iter->wstrFileName.c_str());
		}

		else
		{
			hr = ENGINE::CResourceMgr::GetInstance()->AddTexture(
				ENGINE::GetGraphicDev()->GetDevice(),
				ENGINE::RESOURCE_STATIC,
				ENGINE::TEX_NORMAL,
				iter->wstrFileName,
				iter->wstrImgPath, 1);
			FAILED_CHECK_MSG(hr, iter->wstrFileName.c_str());
		}
	}

	LeaveCriticalSection(&pLogo->m_CriticalSection);
}
