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

	// ���Ϸκ��� �̹��� ������ ������ �Լ�.
	hr = D3DXGetImageInfoFromFile(wstrFilePath.c_str(), &tImgInfo);
	FAILED_CHECK_MSG_RETURN(hr, wstrFilePath.c_str(), E_FAIL);

	LPDIRECT3DTEXTURE9 pTexture = nullptr;

	// ���Ϸκ��� �̹����� �ҷ��� IDirect3DTexture9 ��ü�� �����ϴ� �Լ�.
	hr = D3DXCreateTextureFromFileEx(
		pGraphicDev, /* ��ġ */
		wstrFilePath.c_str(), /* �̹��� ��� */
		tImgInfo.Width, /* �̹��� ���� �ʺ� */
		tImgInfo.Height, /* �̹��� ���� �ʺ� */
		tImgInfo.MipLevels, /* Mip levels */
		0, /* �ҷ��� �ؽ�ó�� ��� ���. Ư���� ��찡 �ƴ� �̻� 0 */
		tImgInfo.Format, /* �̹��� �ȼ� ���� */
		D3DPOOL_MANAGED, /* �޸� ��� ��� */
		D3DX_DEFAULT, /* �ؽ�ó ���͸� ��� */
		D3DX_DEFAULT, /* �Ӹ� ���͸� ��� */
		0, /* ������ ���� */
		nullptr, /* �ҷ��� �̹��� ���� */
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
