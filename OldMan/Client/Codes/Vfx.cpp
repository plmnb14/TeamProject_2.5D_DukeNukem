#include "stdafx.h"
#include "Vfx.h"
#include "Trasform.h"
#include "CameraObserver.h"

CVfx::CVfx(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pManagement(ENGINE::GetManagement()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr), m_pAnimator(nullptr),
	m_pSubject(ENGINE::GetCameraSubject()), m_pBillborad(nullptr),
	m_eTerrainType(ENGINE::TERRAIN_END)
{
}

CVfx::~CVfx()
{
	Release();
}

int CVfx::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::Update();

	return NO_EVENT;
}

void CVfx::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
}

void CVfx::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &(m_pTransform->GetWorldMatrix()));
	m_pTexture->Render(0);
	m_pBuffer->Render();
}

void CVfx::ChangeTex(wstring _wstrTex)
{
}

HRESULT CVfx::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));

	return S_OK;
}

HRESULT CVfx::LateInit()
{
	return S_OK;
}

void CVfx::Release()
{
}

HRESULT CVfx::AddComponent()
{
	return S_OK;
}

CVfx* CVfx::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CVfx* pInstance = new CVfx(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}
