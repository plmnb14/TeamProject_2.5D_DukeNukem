#include "stdafx.h"
#include "Number.h"
#include "PlayerObserver.h"
#include "Trasform.h"

CNumber::CNumber(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev)
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

	// 임시 코드
	if (ENGINE::CKeyMgr::GetInstance()->KeyPressing(ENGINE::KEY_Q))
		if(m_iTestHP > 0) m_iTestHP -= 1;
	if (ENGINE::CKeyMgr::GetInstance()->KeyPressing(ENGINE::KEY_E))
		if (m_iTestShield > 0) m_iTestShield -= 1;

	switch (m_eNumberType)
	{
	case CNumber::NUMBER_HP:
	{
		//GetNumberArr((int)m_pPlayerObserver->GetPlayerInfo().fHitPoint, m_iNumArr, m_iArrCount);
		GetNumberArr(m_iTestHP, m_iNumArr, m_iArrCount);
		break;
	}
	case CNumber::NUMBER_SHIELD:
	{
		//GetNumberArr((int)m_pPlayerObserver->GetPlayerInfo().fShieldPoint, m_iNumArr, m_iArrCount);
		GetNumberArr(m_iTestShield, m_iNumArr, m_iArrCount);
		break;
	}
	case CNumber::NUMBER_BULLET:
	{
		if (m_iCurBullet != m_pPlayerObserver->GetWeaponInfo().wCurBullet
			|| m_iMaxBullet != m_pPlayerObserver->GetWeaponInfo().wMagazineBullet)
		{
			m_iCurBullet = m_pPlayerObserver->GetWeaponInfo().wCurBullet;
			m_iMaxBullet = m_pPlayerObserver->GetWeaponInfo().wMagazineBullet;
			GetNumberArr(m_iCurBullet, m_iNumArr, m_iArrCount, m_iMaxBullet);
		}

		//GetNumberArr(m_pPlayerObserver->GetWeaponInfo().wMagazineBullet, m_iNumArr, m_iArrCount);
		// Release 했을 때 이 부분에서 자꾸터져서, 임시방편으로 저렇게 해둠
		break;
	}
	case CNumber::NUMBER_END:
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
	ENGINE::CGameObject::LateUpdate();
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

	// 임시
	m_iTestMaxHP = 100;
	m_iTestHP = m_iTestMaxHP;
	m_iTestMaxShield = 100;
	m_iTestShield = m_iTestMaxShield;

	return S_OK;
}

HRESULT CNumber::LateInit()
{
	CUI::LateInit();
	return S_OK;
}

void CNumber::Release()
{
	for (auto& iter : m_vecNumberUI)
	{
		ENGINE::Safe_Delete(iter);
	}
}

HRESULT CNumber::AddComponent()
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

void CNumber::GetNumberArr(int _iNumber, int*& _iArr, int& _iCount, int _iNumber2)
{
	for (auto& iter : m_vecNumberUI)
	{
		iter->SetDead();
		//ENGINE::Safe_Delete(iter);
	}
	m_vecNumberUI.clear();

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

	int iTemp = 0;
	if (_iNumber2 >= 0)
	{
		iNum = _iNumber2;
		for (int i = 0; i < MAXINT; i++)
		{
			iNum /= 10;

			if (iNum <= 0)
			{
				iTemp = i + 1;
				break;
			}
		}
		_iArr = new int[_iCount + iTemp];
	}
	else
		_iArr = new int[_iCount];

	for (int i = 0; i < _iCount; i++)
	{
		_iArr[i] = _iNumber % 10;
		_iNumber /= 10;

		TCHAR _szNum[MIN_STR];
		swprintf_s(_szNum, L"Number_%d.png", _iArr[i]);
		m_vecNumberUI.push_back(CUI::Create(m_pGraphicDev, _szNum));
	}

	if (_iNumber2 >= 0)
	{
		m_vecNumberUI.push_back(CUI::Create(m_pGraphicDev, L"Slash.png"));

		for (int i = _iCount + 1; i < _iCount + iTemp + 1; i++)
		{
			_iArr[i] = _iNumber2 % 10;
			_iNumber2 /= 10;
		
			TCHAR _szNum[MIN_STR];
			swprintf_s(_szNum, L"Number_%d.png", _iArr[i]);
			m_vecNumberUI.push_back(CUI::Create(m_pGraphicDev, _szNum));
		}

		_iCount += iTemp + 1;
	}
}