#include "stdafx.h"
#include "LogoBack.h"

CLogoBack::CLogoBack(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr)
{
}

CLogoBack::~CLogoBack()
{
	Release();
}

void CLogoBack::Update()
{
	ENGINE::CGameObject::Update();

	m_fStartFrame += m_fMaxFrame * m_pTimeMgr->GetDelta();

	if (m_fStartFrame >= m_fMaxFrame)
		m_fStartFrame = 0.f;
}

void CLogoBack::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
}

void CLogoBack::Render()
{
	/*auto iter_find = m_mapComponent.find(L"Texture");
	dynamic_cast<ENGINE::CTexture*>(iter_find->second)->Render(0);

	iter_find = m_mapComponent.find(L"Buffer");
	dynamic_cast<ENGINE::CVIBuffer*>(iter_find->second)->Render();*/

	m_pTexture->Render(DWORD(m_fStartFrame));
	m_pBuffer->Render();
}

HRESULT CLogoBack::Initialize()
{
	HRESULT hr = AddComponent();
	FAILED_CHECK_RETURN(hr, E_FAIL);

	m_fStartFrame = 0.f;
	m_fMaxFrame = 38.f;

	return S_OK;
}

void CLogoBack::Release()
{
}

HRESULT CLogoBack::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	// Texture
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_DYNAMIC, L"Texture_LogoBack");
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Texture", pComponent });

	m_pTexture = dynamic_cast<ENGINE::CTexture*>(pComponent);
	NULL_CHECK_RETURN(m_pTexture, E_FAIL);

	// Buffer
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_DYNAMIC, L"Buffer_RcTex");
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Buffer", pComponent });

	m_pBuffer = dynamic_cast<ENGINE::CVIBuffer*>(pComponent);
	NULL_CHECK_RETURN(m_pBuffer, E_FAIL);

	return S_OK;
}

CLogoBack* CLogoBack::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CLogoBack* pInstance = new CLogoBack(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}
