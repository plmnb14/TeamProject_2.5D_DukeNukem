#include "stdafx.h"
#include "Number.h"
#include "PlayerObserver.h"
#include "Trasform.h"

USING(ENGINE)

CNumber::CNumber(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev),
	m_pSubject(ENGINE::GetPlayerSubject()),
	m_pObserver(nullptr)
{
}

CNumber::~CNumber()
{
}

int CNumber::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	return NO_EVENT;
}

void CNumber::LateUpdate()
{
}

void CNumber::Render()
{
	// 여러자리 수 일때
	// 재귀함수로 나를 다시 부르기?
}

HRESULT CNumber::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));

	m_fSizeX = 50.f;
	m_fSizeY = 50.f;

	return S_OK;
}

HRESULT CNumber::LateInit()
{
	return S_OK;
}

void CNumber::Release()
{
}

void CNumber::ChangeTex(wstring _wstrTex)
{
}

HRESULT CNumber::AddComponent()
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

CNumber* CNumber::Create(LPDIRECT3DDEVICE9 pGraphicDev, NUMBER_TYPE _eType)
{
	CNumber* pInstance = new CNumber(pGraphicDev);
	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	pInstance->m_eNumberType = _eType;

	return pInstance;
}
