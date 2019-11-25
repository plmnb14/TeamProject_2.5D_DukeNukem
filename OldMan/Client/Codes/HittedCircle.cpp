#include "stdafx.h"
#include "HittedCircle.h"
#include "Trasform.h"
#include "CameraObserver.h"
#include "PlayerObserver.h"

CHittedCircle::CHittedCircle(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev)
{
}

CHittedCircle::~CHittedCircle()
{
	Release();
}

int CHittedCircle::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();
	CheckLifeTime();

	return NO_EVENT;
}

void CHittedCircle::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
}

void CHittedCircle::Render()
{
	CUI::Render();
}

HRESULT CHittedCircle::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));

	m_fSizeX = 250.f;
	m_fSizeY = 250.f;

	m_fLifeTime = 0.5f;

	return S_OK;
}

HRESULT CHittedCircle::LateInit()
{
	CUI::LateInit();
	SetCircle();

	return S_OK;
}

void CHittedCircle::Release()
{
}

CHittedCircle* CHittedCircle::Create(LPDIRECT3DDEVICE9 pGraphicDev, HITTED_SIZE _eSize)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CHittedCircle* pInstance = new CHittedCircle(pGraphicDev);
	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	pInstance->m_eSize = _eSize;

	return pInstance;
}

void CHittedCircle::SetCircle()
{
	switch (m_eSize)
	{
	case CHittedCircle::SIZE_XS:
	{
		ChangeTex(L"Hitted_UI_XS.png");
		break;
	}
	case CHittedCircle::SIZE_S:
	{
		ChangeTex(L"Hitted_UI_S.png");
		break;
	}
	case CHittedCircle::SIZE_M:
	{
		ChangeTex(L"Hitted_UI_M.png");
		break;
	}
	case CHittedCircle::SIZE_L:
	{
		ChangeTex(L"Hitted_UI_L.png");
		break;
	}
	case CHittedCircle::SIZE_XL:
	{
		ChangeTex(L"Hitted_UI_XL.png");
		break;
	}
	case CHittedCircle::SIZE_END:
		break;
	}

}

void CHittedCircle::CheckLifeTime()
{
	if (m_fLifeTime <= 0)
		m_bIsDead = true;
	else
		m_fLifeTime -= m_pTimeMgr->GetDelta();
}
