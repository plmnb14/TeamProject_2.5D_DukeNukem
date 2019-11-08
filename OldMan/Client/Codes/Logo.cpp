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
	ENGINE::CScene::Update();

	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{		
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

	// Object Create
	HRESULT hr = 0;
	ENGINE::CGameObject* pObject = nullptr;

	// LogoBack
	pObject = CLogoBack::Create(m_pGraphicDev);
	NULL_CHECK_MSG_RETURN(pObject, L"LogoBack Create Failed", E_FAIL);

	// Layer insert
	hr = m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::PROPS, pObject);
	FAILED_CHECK_MSG_RETURN(hr, L"LogoBack Add Failed", E_FAIL);
	
	return S_OK;
}

HRESULT CLogo::Add_UI_Layer()
{
	return S_OK;
}

HRESULT CLogo::Initialize()
{
	PipeLineSetUp();	

	// Texture
	HRESULT hr = m_pResourceMgr->AddTexture(
		m_pGraphicDev,
		ENGINE::RESOURCE_DYNAMIC,
		ENGINE::TEX_NORMAL,
		L"Texture_LogoBack",
		L"../Texture/LogoBack/LogoBack_%d.png", 38);
	FAILED_CHECK_MSG_RETURN(hr, L"Texture_LogoBack Add Failed", E_FAIL);

	// Buffer
	hr = m_pResourceMgr->AddBuffer(
		m_pGraphicDev,
		ENGINE::RESOURCE_DYNAMIC,
		ENGINE::CVIBuffer::BUFFER_RCTEX,
		L"Buffer_RcTex");
	FAILED_CHECK_MSG_RETURN(hr, L"Buffer_RcTex Add Failed", E_FAIL);

	// Environment Layer
	hr = Add_Environment_Layer();
	FAILED_CHECK_RETURN(hr,E_FAIL);

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
	m_pResourceMgr->ResetDynamicResource();
}

void CLogo::PipeLineSetUp()
{
	// Α¶Έν off
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
