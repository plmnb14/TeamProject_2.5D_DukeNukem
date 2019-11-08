#include "Renderer.h"
#include "Scene.h"

USING(ENGINE)

CRenderer::CRenderer(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev), m_pScene(nullptr)
{
	m_pGraphicDev->AddRef();
}

CRenderer::~CRenderer()
{
	Release();
}

void CRenderer::SetScene(CScene* pScene)
{
	m_pScene = pScene;
	NULL_CHECK_MSG(m_pScene, L"Renderer`s m_pScene is nullptr");
}

void CRenderer::Render()
{
	m_pGraphicDev->Clear(0, nullptr,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
		D3DCOLOR_ARGB(255, 0, 0, 255), 1.f, 0);
	m_pGraphicDev->BeginScene();

	NULL_CHECK_MSG(m_pScene, L"Renderer`s m_pScene is nullptr");
	m_pScene->Render();

	m_pGraphicDev->EndScene();
	m_pGraphicDev->Present(nullptr, nullptr, nullptr, nullptr);
}

HRESULT CRenderer::Initialize()
{
	return S_OK;
}

void CRenderer::Release()
{
	Safe_Release(m_pGraphicDev);
}

CRenderer* CRenderer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CRenderer* pInstance = new CRenderer(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}
