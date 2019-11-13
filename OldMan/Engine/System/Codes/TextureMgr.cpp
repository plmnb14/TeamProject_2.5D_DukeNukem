#include "TextureMgr.h"
#include "SingleTexture.h"
#include "MultiTexture.h"
#include <fstream>
#include <tchar.h>

USING(ENGINE)

IMPLEMENT_SINGLETON(CTextureMgr)

CTextureMgr::CTextureMgr()
{
}


CTextureMgr::~CTextureMgr()
{
	Release();
}

const TEX_INFO* CTextureMgr::GetTexInfo(
	const wstring& wstrObjectKey,
	const wstring& wstrStateKey,
	int iIndex)
{
	auto iter_find = m_mapTexture.find(wstrObjectKey);

	if (m_mapTexture.end() == iter_find)
		return nullptr;

	return iter_find->second->GetTexInfo(wstrStateKey, iIndex);
}

map<wstring, CAPITexture*> CTextureMgr::GetMapTexture()
{
	return m_mapTexture;
}

HRESULT CTextureMgr::LoadTextureFromImgPath(const wstring& wstrImgPath)
{
	wifstream fin;

	fin.open(wstrImgPath);

	if (fin.fail())
		return E_FAIL;

	TCHAR szObjectKey[MAX_STR] = L"";
	TCHAR szStateKey[MAX_STR] = L"";
	TCHAR szFileName[MAX_STR] = L"";
	TCHAR szImgCount[MAX_STR] = L"";
	TCHAR szImgPath[MAX_STR] = L"";

	while (true)
	{
		fin.getline(szObjectKey, MAX_STR, '|');
		fin.getline(szStateKey, MAX_STR, '|');
		fin.getline(szFileName, MAX_STR, '|');
		fin.getline(szImgCount, MAX_STR, '|');
		fin.getline(szImgPath, MAX_STR);

		if (fin.eof())
			break;

		if (!lstrcmp(szObjectKey, L"No_Animaition"))
		{
			HRESULT hr = LoadTexture(SINGLE_TEXTURE, szImgPath,
				szFileName);
			FAILED_CHECK_MSG_RETURN(hr, szImgPath, E_FAIL);
		}
		else
		{
			HRESULT hr = LoadTexture(MULTI_TEXTURE, szImgPath,
				szObjectKey, szStateKey, _ttoi(szImgCount));
			FAILED_CHECK_MSG_RETURN(hr, szImgPath, E_FAIL);
		}
	}

	fin.close();

	return S_OK;
}

HRESULT CTextureMgr::LoadTexture(
	TEXTURE_TYPE eTexType,
	const wstring& wstrFilePath,
	const wstring & wstrObjectKey,
	const wstring & wstrStateKey,
	int iImgCount)
{
	auto iter_find = m_mapTexture.find(wstrObjectKey);

	// 중복된 오브젝트 키가 없다면 싱글이건 멀티건 최초 할당해주고 map에 삽입.
	if (m_mapTexture.end() == iter_find)
	{
		CAPITexture* pTexture = nullptr;

		switch (eTexType)
		{
		case SINGLE_TEXTURE:
			pTexture = CSingleTexture::Create(wstrFilePath);
			break;
		case MULTI_TEXTURE:
			pTexture = CMultiTexture::Create(wstrFilePath, wstrStateKey, iImgCount);
			break;
		}

		NULL_CHECK_RETURN(pTexture, E_FAIL);
		m_mapTexture.insert({ wstrObjectKey, pTexture });
	}
	// 오브젝트 키가 중복되는데 멀티 텍스처인 경우 이미 할당되어 있는 멀티 텍스처에 상태 키만 추가해준다.
	else if (MULTI_TEXTURE == eTexType)
	{
		HRESULT hr = m_mapTexture[wstrObjectKey]->LoadTexture(
			wstrFilePath, wstrStateKey, iImgCount);
		FAILED_CHECK_RETURN(hr, E_FAIL);
	}


	return S_OK;
}

void CTextureMgr::Release()
{
	for (auto& Mypair : m_mapTexture)
		Safe_Delete(Mypair.second);

	m_mapTexture.clear();
}
