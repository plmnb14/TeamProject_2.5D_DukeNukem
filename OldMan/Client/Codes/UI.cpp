#include "stdafx.h"
#include "UI.h"
#include "Trasform.h"
#include "Billborad.h"
#include "Camera_Component.h"
#include "CameraObserver.h"

CUI::CUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr),
	m_pSubject(ENGINE::GetCameraSubject()),
	m_pObserver(nullptr)
{
}

CUI::~CUI()
{
	Release();
}

int CUI::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();

	return NO_EVENT;
}

void CUI::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
}

void CUI::Render()
{
	// Set Proj BeforRender ==========================================================
	D3DXMATRIX matWorld, matView, matProj, matTempView, matTempProj;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixIdentity(&matView);
	D3DXMatrixIdentity(&matProj);
	D3DXMatrixIdentity(&matTempView);
	D3DXMatrixIdentity(&matTempProj);

	// Get Temp
	matTempView = m_pObserver->GetViewMatrix();

	matProj = m_pObserver->GetProjMatrix();
	matTempProj = m_pObserver->GetProjMatrix();

	// 직교투영
	D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.f, 1.f);

	// Set Sizs
	float fScale[3];
	fScale[0] = m_pTransform->GetSize().x * m_fSizeX;
	fScale[1] = m_pTransform->GetSize().y * m_fSizeY;
	fScale[2] = 1.f;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			matView(i, j) *= fScale[i];
		}
	}

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);

	// ===============================================================================
	// Render
	m_pTexture->Render(0);
	m_pBuffer->Render();

	// Set Proj AfterRender ==========================================================
	// 되돌리기
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matTempView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matTempProj);
}

void CUI::ChangeTex(wstring _wstrTex)
{
	m_wstrTex = _wstrTex;

	// Change Texture component
	m_mapComponent.erase(L"Texture");

	ENGINE::CComponent* pComponent = nullptr;
	pComponent = ENGINE::GetResourceMgr()->CloneResource(ENGINE::RESOURCE_DYNAMIC, _wstrTex);
	NULL_CHECK(pComponent);
	m_mapComponent.insert({ L"Texture", pComponent });

	m_pTexture = dynamic_cast<ENGINE::CTexture*>(pComponent);
	NULL_CHECK(m_pTexture);
}

void CUI::SetSize(float _fSizeX, float _fSizeY)
{
	m_fSizeX = _fSizeX;
	m_fSizeY = _fSizeY;
}

HRESULT CUI::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));

	m_fSizeX = 50.f;
	m_fSizeY = 50.f;

	return S_OK;
}

HRESULT CUI::LateInit()
{
	m_pObserver = CCameraObserver::Create();
	NULL_CHECK_RETURN(m_pObserver, E_FAIL);

	m_pSubject->Subscribe(m_pObserver);

	return S_OK;
}

void CUI::Release()
{
	m_pSubject->UnSubscribe(m_pObserver);
	ENGINE::Safe_Delete(m_pObserver);
}

HRESULT CUI::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	// Texture
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_DYNAMIC, L"Tile256x256_0");
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

	// Transform
	pComponent = ENGINE::CTransform::Create(D3DXVECTOR3(0.f, 0.f, 1.f));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Transform", pComponent });

	m_pTransform = dynamic_cast<ENGINE::CTransform*>(pComponent);
	NULL_CHECK_RETURN(m_pTransform, E_FAIL);
	return S_OK;
}

CUI* CUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, wstring wstrTex)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CUI* pInstance = new CUI(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	pInstance->ChangeTex(wstrTex);

	return pInstance;
}
