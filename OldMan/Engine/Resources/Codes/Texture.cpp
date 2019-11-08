#include "Texture.h"

USING(ENGINE)

CTexture::CTexture(LPDIRECT3DDEVICE9 pGraphicDev)
	: CResources(pGraphicDev)
{
}

CTexture::~CTexture()
{
	Release();
}

void CTexture::Render()
{
}

void CTexture::Render(const DWORD& dwIndex)
{
	if (dwIndex >= m_dwContainerSize)
		return;

	// 장치에 텍스처 전달.
	// stage: 0 ~ 7 범위를 줄 수 있음. 이 값은 D3DFVF_TEX1 ~ D3DFVF_TEX8에 매칭된다.
	m_pGraphicDev->SetTexture(0, m_vecTexture[dwIndex]);
}

CResources* CTexture::CloneResource()
{
	++(*m_pRefCnt);

	return new CTexture(*this);
}

HRESULT CTexture::LoadTexture(
	TEXTRUE_TYPE eTexType, 
	const wstring& wstrFilePath, 
	const DWORD& dwCnt)
{
	m_vecTexture.reserve(dwCnt);

	IDirect3DBaseTexture9* pTexture = nullptr;
	TCHAR szFullPath[MAX_STR] = L"";
	HRESULT hr = 0;

	for (DWORD i = 0; i < dwCnt; ++i)
	{
		swprintf_s(szFullPath, wstrFilePath.c_str(), i);

		switch (eTexType)
		{
		case TEX_NORMAL:
			// 아래 함수로 텍스처 로드할 때는 텍스처의 가로 세로 사이즈가 2의 제곱이어야 한다.
			hr = D3DXCreateTextureFromFile(
				m_pGraphicDev,
				szFullPath,
				(LPDIRECT3DTEXTURE9*)&pTexture);
			break;
		case TEX_CUBE:
			// 이 때 텍스처의 확장자는 .dds
			hr = D3DXCreateCubeTextureFromFile(
				m_pGraphicDev,
				szFullPath,
				(LPDIRECT3DCUBETEXTURE9*)&pTexture);
			break;
		}

		FAILED_CHECK_RETURN(hr, E_FAIL);
		m_vecTexture.push_back(pTexture);
	}

	m_dwContainerSize = m_vecTexture.size();

	return S_OK;
}

void CTexture::Release()
{
	if (0 == (*m_pRefCnt))
	{
		for_each(m_vecTexture.begin(), m_vecTexture.end(), Safe_Release<IDirect3DBaseTexture9*>);
		m_vecTexture.clear();
		m_vecTexture.shrink_to_fit();
	}	
}

CTexture* CTexture::Create(
	LPDIRECT3DDEVICE9 pGraphicDev, 
	TEXTRUE_TYPE eTexType, 
	const wstring& wstrFilePath, 
	const DWORD& dwCnt)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CTexture* pInstance = new CTexture(pGraphicDev);

	if (FAILED(pInstance->LoadTexture(eTexType, wstrFilePath, dwCnt)))
	{
		Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}
