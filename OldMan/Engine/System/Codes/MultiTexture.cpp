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
		MessageBox(0, wstrStateKey.c_str(), L"�̹� �����ϴ� Ű���Դϴ�", MB_OK);
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
		// D:\������\4������\99A\DefaulWindow\Texture\Stage\Terrain\Tile\Tile0.png
		swprintf_s(szFullPath, wstrFilePath.c_str(), i);

		// ���Ϸκ��� �̹��� ������ ������ �Լ�.
		hr = D3DXGetImageInfoFromFile(szFullPath, &tImgInfo);
		FAILED_CHECK_MSG_RETURN(hr, szFullPath, E_FAIL);

		// ���Ϸκ��� �̹����� �ҷ��� IDirect3DTexture9 ��ü�� �����ϴ� �Լ�.
		hr = D3DXCreateTextureFromFileEx(
			pDevice, /* ��ġ */
			szFullPath, /* �̹��� ��� */
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
