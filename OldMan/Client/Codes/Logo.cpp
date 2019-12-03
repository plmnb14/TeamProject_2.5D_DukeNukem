#include "stdafx.h"
#include "Logo.h"
#include "SceneSelector.h"

#include "LogoBack.h"
#include "UI.h"
#include "Number.h"
#include "SoundMgr.h"

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
	ENGINE::CScene::Update();

	if (Loading())
	{
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		{
			WaitForSingleObject(m_hLoadingThread, INFINITE);

			HRESULT hr = ENGINE::GetManagement()->SceneChange(CSceneSelector(CSceneSelector::STAGE));
			FAILED_CHECK_MSG(hr, L"STAGE Scene Change Failed");
		}
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
	// Object Layer
	ENGINE::CLayer* pUILayer = ENGINE::CLayer::Create(m_pGraphicDev);
	NULL_CHECK_MSG_RETURN(pUILayer, L"UI Layer Create Failed", E_FAIL);
	m_mapLayer.insert({ ENGINE::CLayer::UI, pUILayer });

	// Title Background
	ENGINE::CGameObject* pObject = CUI::Create(m_pGraphicDev, L"Title.png");
	NULL_CHECK_MSG_RETURN(pObject, L"BG Create Failed", E_FAIL);
	pUILayer->AddObject(ENGINE::OBJECT_TYPE::UI, pObject);
	static_cast<CUI*>(pObject)->SetSize(640,360);
	pObject->Set_MapLayer(m_mapLayer);

	// Loading Number
	pObject = CNumber::Create(m_pGraphicDev, CNumber::NUMBER_LOADING);
	NULL_CHECK_MSG_RETURN(pObject, L"Number Create Failed", E_FAIL);
	pUILayer->AddObject(ENGINE::OBJECT_TYPE::UI, pObject);
	static_cast<CUI*>(pObject)->SetSize(6.f, 10.f);
	static_cast<CUI*>(pObject)->SetPos(D3DXVECTOR3(555.f, -330.f, 0.f));
	pObject->Set_MapLayer(m_mapLayer);
	m_pLoadingPercent = pObject;

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

	HRESULT hr = m_pResourceMgr->AddBuffer(
		m_pGraphicDev,
		ENGINE::RESOURCE_DYNAMIC,
		ENGINE::CVIBuffer::BUFFER_RCTEX,
		L"Buffer_Player");
	FAILED_CHECK_MSG_RETURN(hr, L"Buffer_Player Add Failed", E_FAIL);

	// Terrain Buffer


	// 
	hr = m_pResourceMgr->AddBuffer(
		m_pGraphicDev,
		ENGINE::RESOURCE_DYNAMIC,
		ENGINE::CVIBuffer::BUFFER_CUBECOL,
		L"Buffer_CubeCol");
	FAILED_CHECK_MSG_RETURN(hr, L"Buffer_CubeCol Add Failed", E_FAIL);

	//
	hr = m_pResourceMgr->AddBuffer(
		m_pGraphicDev,
		ENGINE::RESOURCE_DYNAMIC,
		ENGINE::CVIBuffer::BUFFER_WALLCUBECOL,
		L"Buffer_WallCubeCol");
	FAILED_CHECK_MSG_RETURN(hr, L"Buffer_WallCubeCol Add Failed", E_FAIL);

	//
	hr = m_pResourceMgr->AddBuffer(
		m_pGraphicDev,
		ENGINE::RESOURCE_DYNAMIC,
		ENGINE::CVIBuffer::BUFFER_CUBETEX,
		L"Buffer_CubeTex");
	FAILED_CHECK_MSG_RETURN(hr, L"Buffer_CubeTex Add Failed", E_FAIL);



	// Add Texture
	hr = ENGINE::CResourceMgr::GetInstance()->AddTexture(
		ENGINE::GetGraphicDev()->GetDevice(),
		ENGINE::RESOURCE_DYNAMIC,
		ENGINE::TEX_NORMAL,
		L"Title.png",
		L"../Texture/UI/No_Animation/Title/Title.png", 1);
	FAILED_CHECK_MSG(hr, L"Title.png");

	hr = ENGINE::CResourceMgr::GetInstance()->AddTexture(
		ENGINE::GetGraphicDev()->GetDevice(),
		ENGINE::RESOURCE_STATIC,
		ENGINE::TEX_NORMAL,
		L"Number",
		L"../Texture/UI/Animation/Unusable/Number/Number_%d.png", 12);
	FAILED_CHECK_MSG(hr, L"Number.png");
	
	// Add Buffer
	hr = m_pResourceMgr->AddBuffer(
		m_pGraphicDev,
		ENGINE::RESOURCE_DYNAMIC,
		ENGINE::CVIBuffer::BUFFER_RCTEX,
		L"Buffer_RcTex");
	FAILED_CHECK_MSG_RETURN(hr, L"Buffer_RcTex Add Failed", E_FAIL);

	// Environment Layer
	hr = Add_Environment_Layer();
	FAILED_CHECK_RETURN(hr, E_FAIL);

	// Object Layer
	hr = Add_Object_Layer();
	FAILED_CHECK_RETURN(hr, E_FAIL);

	// UI Layer
	hr = Add_UI_Layer();
	FAILED_CHECK_RETURN(hr, E_FAIL);

	// PlaySound
	CSoundMgr::GetInstance()->SetVolume(CSoundMgr::BGM, 1.0f);
	CSoundMgr::GetInstance()->StopAll();
	CSoundMgr::GetInstance()->MyPlayBGM(L"Title Theme.mp3");

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

bool CLogo::Loading()
{
	system("cls");

	if ((ENGINE::GetResourceMgr()->Get_TextureCount() * 100 / ENGINE::GetTextureMgr()->Get_MaxTextureCount()) != 0)
	{
		if ((ENGINE::GetResourceMgr()->Get_TextureCount() * 100 / ENGINE::GetTextureMgr()->Get_MaxTextureCount()) == 100)
		{
			cout << "Complete !!" << endl;
			static_cast<CNumber*>(m_pLoadingPercent)->UpdateNumber(100);
			return true;
		}
		else
			static_cast<CNumber*>(m_pLoadingPercent)->UpdateNumber(ENGINE::GetResourceMgr()->Get_TextureCount() * 100 / ENGINE::GetTextureMgr()->Get_MaxTextureCount());

		if ((ENGINE::GetResourceMgr()->Get_TextureCount() * 100 / ENGINE::GetTextureMgr()->Get_MaxTextureCount()) < 100)
		{
			cout << "Loading" << endl;
		}

		if ((ENGINE::GetResourceMgr()->Get_TextureCount() * 100 / ENGINE::GetTextureMgr()->Get_MaxTextureCount()) % 1 == 0)
		{
			cout << " .";
		}

		if ((ENGINE::GetResourceMgr()->Get_TextureCount() * 100 / ENGINE::GetTextureMgr()->Get_MaxTextureCount()) % 2 == 0)
		{
			cout << " .";
		}

		if ((ENGINE::GetResourceMgr()->Get_TextureCount() * 100 / ENGINE::GetTextureMgr()->Get_MaxTextureCount()) % 3 == 0)
		{
			cout << " .";
		}
	}

	cout << "" << endl;

	cout << ENGINE::GetResourceMgr()->Get_TextureCount() * 100 / ENGINE::GetTextureMgr()->Get_MaxTextureCount();
	cout << " / ";
	cout << ENGINE::GetTextureMgr()->Get_MaxTextureCount() / ENGINE::GetTextureMgr()->Get_MaxTextureCount() * 100 << endl;

	return false;
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
