#include "SingleTexture.h"

USING(ENGINE)

CSingleTexture::CSingleTexture()
	:  m_pTexInfo(nullptr)
{
}


CSingleTexture::~CSingleTexture()
{
	Release();
}

const TEX_INFO* CSingleTexture::GetTexInfo(
	const wstring& wstrStateKey, int iIndex)
{
	return m_pTexInfo;
}

HRESULT CSingleTexture::LoadTexture(
	LPDIRECT3DDEVICE9 pGraphicDev,
	const wstring& wstrFilePath,
	const wstring& wstrStateKey/* = L""*/,
	int iImgCount/* = 0*/)
{
	HRESULT hr = 0;

	D3DXIMAGE_INFO tImgInfo;
	ZeroMemory(&tImgInfo, sizeof(D3DXIMAGE_INFO));

	// 파일로부터 이미지 정보를 얻어오는 함수.
	hr = D3DXGetImageInfoFromFile(wstrFilePath.c_str(), &tImgInfo);
	FAILED_CHECK_MSG_RETURN(hr, wstrFilePath.c_str(), E_FAIL);

	LPDIRECT3DTEXTURE9 pTexture = nullptr;

	// 파일로부터 이미지를 불러와 IDirect3DTexture9 객체를 생성하는 함수.
	hr = D3DXCreateTextureFromFileEx(
		pGraphicDev, /* 장치 */
		wstrFilePath.c_str(), /* 이미지 경로 */
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
	FAILED_CHECK_MSG_RETURN(hr, wstrFilePath.c_str(), E_FAIL);

	m_pTexInfo = new TEX_INFO;
	m_pTexInfo->pTexture = pTexture;
	m_pTexInfo->tImgInfo = tImgInfo;

	return S_OK;
}

void CSingleTexture::Release()
{
	m_pTexInfo->pTexture->Release();
	Safe_Delete(m_pTexInfo);
}

CSingleTexture* CSingleTexture::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring & wstrFilePath)
{
	CSingleTexture* pInstance = new CSingleTexture();

	if (FAILED(pInstance->LoadTexture(pGraphicDev, wstrFilePath)))
	{
		Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}
