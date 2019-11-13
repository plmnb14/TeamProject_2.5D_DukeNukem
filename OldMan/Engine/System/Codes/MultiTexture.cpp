#include "MultiTexture.h"

USING(ENGINE)

CMultiTexture::CMultiTexture()
{
}


CMultiTexture::~CMultiTexture()
{
	Release();
}

const TEX_INFO* CMultiTexture::GetTexInfo(
	const wstring& wstrStateKey/* = L""*/,
	int iIndex/* = 0*/)
{
	auto iter_find = m_mapMultiTexture.find(wstrStateKey);

	if (m_mapMultiTexture.end() == iter_find)
		return nullptr;

	return iter_find->second[iIndex];
}

HRESULT CMultiTexture::LoadTexture(
	const wstring& wstrFilePath,
	const wstring& wstrStateKey/* = L""*/,
	int iImgCount/* = 0*/)
{
	auto iter_find = m_mapMultiTexture.find(wstrStateKey);

	if (m_mapMultiTexture.end() != iter_find)
	{
		MessageBox(0, wstrStateKey.c_str(), L"이미 존재하는 키값입니다", MB_OK);
		return E_FAIL;
	}

	LPDIRECT3DDEVICE9 pDevice = m_pGraphicDev->GetDevice();
	NULL_CHECK_MSG_RETURN(pDevice, L"pDevice is null", E_FAIL);

	HRESULT hr = 0;

	LPDIRECT3DTEXTURE9 pTexture = nullptr;
	TEX_INFO* pTexInfo = nullptr;
	D3DXIMAGE_INFO tImgInfo;
	ZeroMemory(&tImgInfo, sizeof(D3DXIMAGE_INFO));

	TCHAR szFullPath[MAX_STR] = L"";

	for (int i = 0; i < iImgCount; ++i)
	{
		// szFullPath
		// D:\김태윤\4개월차\99A\DefaulWindow\Texture\Stage\Terrain\Tile\Tile0.png
		swprintf_s(szFullPath, wstrFilePath.c_str(), i);

		// 파일로부터 이미지 정보를 얻어오는 함수.
		hr = D3DXGetImageInfoFromFile(szFullPath, &tImgInfo);
		FAILED_CHECK_MSG_RETURN(hr, szFullPath, E_FAIL);

		// 파일로부터 이미지를 불러와 IDirect3DTexture9 객체를 생성하는 함수.
		hr = D3DXCreateTextureFromFileEx(
			pDevice, /* 장치 */
			szFullPath, /* 이미지 경로 */
			tImgInfo.Width, /* 이미지 가로 너비 */
			tImgInfo.Height, /* 이미지 세로 너비 */
			tImgInfo.MipLevels, /* Mip levels */
			0, /* 불러온 텍스처의 사용 방식. 특수한 경우가 아닌 이상 0 */
			tImgInfo.Format, /* 이미지 픽셀 포맷 */
			D3DPOOL_MANAGED, /* 메모리 사용 방식 */
			D3DX_DEFAULT, /* 텍스처 필터링 방식 */
			D3DX_DEFAULT, /* 밉맵 필터링 방식 */
			0, /* 제거할 색상 */
			nullptr, /* 불러온 이미지 정보 */
			nullptr,
			&pTexture);
		FAILED_CHECK_MSG_RETURN(hr, szFullPath, E_FAIL);

		pTexInfo = new TEX_INFO;
		pTexInfo->pTexture = pTexture;
		pTexInfo->tImgInfo = tImgInfo;

		m_mapMultiTexture[wstrStateKey].push_back(pTexInfo);
	}

	return S_OK;
}

void CMultiTexture::Release()
{
	for (auto& MyPair : m_mapMultiTexture)
	{
		for (auto& pTexInfo : MyPair.second)
		{
			pTexInfo->pTexture->Release();
			Safe_Delete(pTexInfo);
		}

		MyPair.second.clear();
		MyPair.second.shrink_to_fit();
	}

	m_mapMultiTexture.clear();
}

CMultiTexture* CMultiTexture::Create(
	const wstring& wstrFilePath,
	const wstring& wstrStateKey,
	int iImgCount)
{
	CMultiTexture* pInstance = new CMultiTexture;

	if (FAILED(pInstance->LoadTexture(wstrFilePath, wstrStateKey, iImgCount)))
	{
		Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}
