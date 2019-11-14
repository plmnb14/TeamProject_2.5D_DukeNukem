#include "stdafx.h"
#include "PathExtract.h"
#include "FileInfo.h"
#include "afxdialogex.h"

CPathExtract::CPathExtract()
{

}

CPathExtract::~CPathExtract()
{
	for_each(m_PathInfoLst_Multi.begin(), m_PathInfoLst_Multi.end(), ENGINE::Safe_Delete<ENGINE::PATH_INFO*>);
	m_PathInfoLst_Multi.clear();

	for_each(m_PathInfoLst_Single.begin(), m_PathInfoLst_Single.end(), ENGINE::Safe_Delete<ENGINE::PATH_INFO*>);
	m_PathInfoLst_Single.clear();
}

void CPathExtract::MakePathFile()
{
	OnDropFiles(L"..\\Client\\Texture\\Tiles", true);
	OnDropFiles(L"..\\Client\\Texture\\Monster", true);
	OnBnClickedSave(true);

	m_PathInfoLst_Multi.clear();
	m_PathInfoLst_Single.clear();

	OnDropFiles(L"..\\Client\\Texture\\Tiles");
	OnDropFiles(L"..\\Client\\Texture\\Monster");

	OnBnClickedSave();
}

void CPathExtract::OnDropFiles(wstring _wstrPath, bool _bForClient)
{
	TCHAR szFullPath[MAX_STR] = L"";
	lstrcpy(szFullPath, _wstrPath.c_str());
	CFileInfo::ExtractPathInfo(szFullPath, m_PathInfoLst_Multi, m_PathInfoLst_Single, _bForClient);
}

void CPathExtract::OnBnClickedSave(bool _bForClient)
{
	wofstream fout;

	if (_bForClient)
	{
		fout.open(L"../Data/TexturePath_Client.txt");
	}
	else
	{
		fout.open(L"../Data/TexturePath.txt");
	}

	if (fout.fail())
		return;

	DWORD dwByte = 0;
	wstring wstrCombined = L"";
	TCHAR szCombined[MAX_STR] = L"";
	TCHAR szImgCount[MIN_STR] = L"";

	for (auto& pPathInfo : m_PathInfoLst_Multi)
	{
		if (!lstrcmp(pPathInfo->wstrStateKey.c_str(), L"Unusable")
			|| !lstrcmp(pPathInfo->wstrObjectKey.c_str(), L"Unusable"))
			continue;

		// 정수 -> 문자
		_itow_s(pPathInfo->iImgCount, szImgCount, 10);

		wstrCombined = pPathInfo->wstrObjectKey + L"|" + pPathInfo->wstrStateKey
			+ L"|" + pPathInfo->wstrFileName + L"|" + szImgCount + L"|" + pPathInfo->wstrImgPath;

		fout << wstrCombined << endl;
	}

	for (auto& pPathInfo : m_PathInfoLst_Single)
	{
		// 정수 -> 문자
		_itow_s(pPathInfo->iImgCount, szImgCount, 10);

		wstrCombined = pPathInfo->wstrObjectKey + L"|" + pPathInfo->wstrStateKey
			+ L"|" + pPathInfo->wstrFileName + L"|" + szImgCount + L"|" + pPathInfo->wstrImgPath;

		fout << wstrCombined << endl;
	}

	fout.close();
}


void CPathExtract::OnBnClickedLoad()
{

}
