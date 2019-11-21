#include "stdafx.h"
#include "GaugeBar.h"
#include "PlayerObserver.h"
#include "Trasform.h"

CGaugeBar::CGaugeBar(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev)
{
}

CGaugeBar::~CGaugeBar()
{
	Release();
}

int CGaugeBar::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();

	switch (m_eBarType)
	{
	case CGaugeBar::BAR_HP:
		m_fHP = m_pPlayerObserver->GetPlayerInfo().fHitPoint;
		break;
	case CGaugeBar::BAR_SHIELD:
		m_fShield = m_pPlayerObserver->GetPlayerInfo().fShieldPoint;
		break;
	case CGaugeBar::BAR_END:
		break;
	}

	UpdateBar();

	for (auto& iter : m_vecBarUI)
	{
		iter->Update();
	}

	return NO_EVENT;
}

void CGaugeBar::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
}

void CGaugeBar::Render()
{
	for (int i = 0; i < m_iBarMaxCount; i++)
	{
		float fPadding = m_fSizeX * 0.3f;
		float fIdxPos = 0.f;
		fIdxPos += (i * m_fSizeX) - (fPadding * i);
		m_vecBarUI[i]->SetPos(D3DXVECTOR3(m_vPos.x + fIdxPos, m_vPos.y, m_vPos.z));
		m_vecBarUI[i]->SetSize(m_fSizeX, m_fSizeY);
	}

	for (auto& iter : m_vecBarUI)
	{
		iter->Render();
	}
}

HRESULT CGaugeBar::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));

	m_fSizeX = 50.f;
	m_fSizeY = 50.f;

	m_iBarMaxCount = 10;
	m_iBarCurCount = m_iBarMaxCount;

	// 임시
	m_iTestMaxHP = 100;
	m_iTestHP = m_iTestMaxHP;
	m_iTestMaxShield = 100;
	m_iTestShield = m_iTestMaxShield;

	return S_OK;
}

HRESULT CGaugeBar::LateInit()
{
	CUI::LateInit();
	InitBar();

	return S_OK;
}

void CGaugeBar::Release()
{
	for (auto& iter : m_vecBarUI)
	{
		ENGINE::Safe_Delete(iter);
	}
}

HRESULT CGaugeBar::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	// Texture
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_DYNAMIC, L"Tile256x256_0.png");
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

void CGaugeBar::InitBar()
{
	switch (m_eBarType)
	{
	case CGaugeBar::BAR_HP:
	{
		for (int i = 0; i < m_iBarMaxCount; i++)
			m_vecBarUI.push_back(CUI::Create(m_pGraphicDev, L"HpGauge.png"));
		break;
	}
	case CGaugeBar::BAR_SHIELD:
	{
		for (int i = 0; i < m_iBarMaxCount; i++)
			m_vecBarUI.push_back(CUI::Create(m_pGraphicDev, L"ShieldGauge.png"));
		break;
	}
	case CGaugeBar::BAR_END:
		break;
	}
}

void CGaugeBar::UpdateBar()
{
	// 임시 코드
	if (ENGINE::CKeyMgr::GetInstance()->KeyPressing(ENGINE::KEY_Q))
		m_iTestHP -= 1;
	if (ENGINE::CKeyMgr::GetInstance()->KeyPressing(ENGINE::KEY_E))
		m_iTestShield -= 1;

	// Init Bar Visible
	for (int i = 0; i < m_iBarMaxCount; i++)
		m_vecBarUI[i]->SetVisible(true);

	switch (m_eBarType)
	{
	case CGaugeBar::BAR_HP:
	{
		int iIdx = m_iTestHP / m_iBarMaxCount;
		if (m_iTestHP < 0) iIdx = 0;
		if (m_iTestHP > 0 && m_iTestHP < 10) iIdx = 1;
		for (int i = 0; i < m_iBarMaxCount - iIdx; i++)
			m_vecBarUI[m_iBarMaxCount - 1- i]->SetVisible(false);
		break;
	}
	case CGaugeBar::BAR_SHIELD:
	{
		int iIdx = m_iTestShield / m_iBarMaxCount;
		if (m_iTestShield < 0) iIdx = 0;
		if (m_iTestShield > 0 && m_iTestShield < 10) iIdx = 1;
		for (int i = 0; i < m_iBarMaxCount - iIdx; i++)
			m_vecBarUI[m_iBarMaxCount - 1 - i]->SetVisible(false);
		break;
	}
	case CGaugeBar::BAR_END:
		break;
	}
}

CGaugeBar* CGaugeBar::Create(LPDIRECT3DDEVICE9 pGraphicDev, GAUGEBAR_TYPE _eType)
{
	CGaugeBar* pInstance = new CGaugeBar(pGraphicDev);
	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	pInstance->m_eBarType = _eType;

	return pInstance;
}