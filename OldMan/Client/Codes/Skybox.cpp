#include "stdafx.h"
#include "Skybox.h"
#include "Trasform.h"

CSkybox::CSkybox(LPDIRECT3DDEVICE9 pGraphicDev)
	:ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pManagement(ENGINE::GetManagement()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr),
	m_pPlayer(nullptr)
{
}

CSkybox::~CSkybox()
{
}

int CSkybox::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::Update();

	ENGINE::CTransform* pTrans = dynamic_cast<ENGINE::CTransform*>(m_pPlayer->Get_Component(L"Transform"));
	m_pTransform->SetPos(pTrans->GetPos());

	return NO_EVENT;
}

void CSkybox::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
}

void CSkybox::Render()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &(m_pTransform->GetWorldMatrix()));

	if (m_pTexture) m_pTexture->Render(0);
	else m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pBuffer->Render();

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CSkybox::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(200.f, 200.f, 100.f));

	return S_OK;
}

void CSkybox::Release()
{
}

HRESULT CSkybox::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	//Texture
	m_wstrTex = L"Tile256x256_0.dds"; // default (begin loading)
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, m_wstrTex);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Texture", pComponent });

	m_pTexture = dynamic_cast<ENGINE::CTexture*>(pComponent);
	NULL_CHECK_RETURN(m_pTexture, E_FAIL);

	// Buffer
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_DYNAMIC, L"Buffer_CubeTex");
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

void CSkybox::ChangeTex(wstring _wstrTex)
{
	m_wstrTex = _wstrTex;

	// Change Texture component
	m_mapComponent.erase(L"Texture");
	m_pTexture = nullptr;

	ENGINE::CComponent* pComponent = nullptr;
	pComponent = ENGINE::GetResourceMgr()->CloneResource(ENGINE::RESOURCE_STATIC, _wstrTex);
	if (!pComponent)
		return;
	m_mapComponent.insert({ L"Texture", pComponent });

	m_pTexture = dynamic_cast<ENGINE::CTexture*>(pComponent);
	NULL_CHECK(m_pTexture);
}

CSkybox* CSkybox::Create(LPDIRECT3DDEVICE9 pGraphicDev, wstring _wstrTexture, ENGINE::CGameObject* pPlayer)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CSkybox* pInstance = new CSkybox(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	pInstance->ChangeTex(_wstrTexture);
	pInstance->m_pPlayer = pPlayer;

	return pInstance;
}
