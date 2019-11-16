#include "stdafx.h"
#include "Number.h"
#include "PlayerObserver.h"
#include "Trasform.h"

USING(ENGINE)

CNumber::CNumber(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev),
	m_pPlayerSubject(ENGINE::GetPlayerSubject()),
	m_pPlayerObserver(nullptr)
{
}

CNumber::~CNumber()
{
	Release();
}

int CNumber::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();

	switch (m_eNumberType)
	{
	case CNumber::NUMBER_HP:
	{
		// 임시.
		GetNumberArr(100, m_iNumArr, m_iArrCount);
		//GetNumberArr(m_pObserver->GetPlayerInfo().fHitPoint, m_iNumArr, m_iArrCount);
		break;
	}
	case CNumber::NUMBER_BULLET:
		// 임시.
		GetNumberArr(9999, m_iNumArr, m_iArrCount);
		break;
	case CNumber::NUMBER_END:
		break;
	default:
		break;
	}

	for (auto& iter : m_vecNumberUI)
	{
		iter->Update();
	}

	return NO_EVENT;
}

void CNumber::LateUpdate()
{
}

void CNumber::Render()
{
	for (int i = 0; i < m_iArrCount; i++)
	{
		float fPadding = m_fSizeX * 0.75f;
		float fIdxPos = 0.f;
		fIdxPos -= (i * m_fSizeX) + (fPadding * i);
		m_vecNumberUI[i]->SetPos(D3DXVECTOR3(m_vPos.x + fIdxPos, m_vPos.y, m_vPos.z));
		m_vecNumberUI[i]->SetSize(m_fSizeX, m_fSizeY);
	}

	for (auto& iter : m_vecNumberUI)
	{
		iter->Render();
	}
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
	m_pPlayerObserver = CPlayerObserver::Create();
	NULL_CHECK_RETURN(m_pPlayerObserver, E_FAIL);

	m_pPlayerSubject->Subscribe(m_pPlayerObserver);

	return S_OK;
}

void CNumber::Release()
{
	m_pPlayerSubject->UnSubscribe(m_pPlayerObserver);
	ENGINE::Safe_Delete(m_pPlayerObserver);

	for (auto& iter : m_vecNumberUI)
	{
		ENGINE::Safe_Delete(iter);
	}
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

void CNumber::GetNumberArr(int _iNumber, int*& _iArr, int& _iCount)
{
	int iNum = _iNumber;
	for (int i = 0; i < MAXINT; i++)
	{
		iNum /= 10;

		if (iNum <= 0)
		{
			_iCount = i + 1;
			break;
		}
	}

	_iArr = new int[_iCount];
	for (auto& iter : m_vecNumberUI)
		ENGINE::Safe_Delete(iter);
	m_vecNumberUI.clear();

	for (int i = 0; i < _iCount; i++)
	{
		_iArr[i] = _iNumber % 10;
		_iNumber /= 10;

		TCHAR _szNum[MIN_STR];
		swprintf_s(_szNum, L"Number_%d", _iArr[i]);
		m_vecNumberUI.push_back(CUI::Create(m_pGraphicDev, _szNum));
	}
}
