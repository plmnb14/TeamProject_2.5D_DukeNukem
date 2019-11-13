#include "stdafx.h"
#include "FileInfo.h"


CFileInfo::CFileInfo()
{
}


CFileInfo::~CFileInfo()
{
}

CString CFileInfo::ConvertRelativePath(const TCHAR* pFullPath)
{
	TCHAR szCurrentDir[MAX_STR] = L"";
	::GetCurrentDirectory(MAX_STR, szCurrentDir);

	TCHAR szRelativePath[MAX_STR] = L"";

	//// From에서 To로 넘어가는 상대경로를 만들어서 Output에 저장하는 함수.
	//::PathRelativePathTo(Output, From, FILE_ATTRIBUTE_DIRECTORY,
	//	To, FILE_ATTRIBUTE_DIRECTORY);
	// 단, From과 To는 같은 드라이브 안에 있어야 한다.
	::PathRelativePathTo(szRelativePath, szCurrentDir, FILE_ATTRIBUTE_DIRECTORY,
		pFullPath, FILE_ATTRIBUTE_DIRECTORY);

	return CString(szRelativePath);
}

void CFileInfo::ExtractPathInfo(
	const TCHAR* pPath, /* D:\\김태윤\\4개월차\\98기\\Framework98\\Texture\\Stage\\Effect\\BossMultiAttack */
	list<PATH_INFO*>& rPathInfoLst_Multi, list<PATH_INFO*>& rPathInfoLst_Single)
{
	wstring wstrFindPath = wstring(pPath) + L"\\*.*";

	// 파일 검색에 관련한 MFC 클래스
	CFileFind find;
	// CFileFind::FindFile(FileName)
	// 경로에 FileName이 존재하는지 유무만 조사하는 함수.
	// 존재한다면 TRUE 반환, 아니면 FALSE 반환.
	// 만약, *.*를 인자로 넘긴다면 그 중 하나라도 찾으면 TRUE.

	// D:\\김태윤\\4개월차\\98기\\Framework98\\Texture\\Stage\\Effect\\BossMultiAttack\\*.*
	BOOL bIsFind = find.FindFile(wstrFindPath.c_str());

	// FindFile함수로 파일이나 폴더 유무를 검색했으면 
	// FindNextFile함수를 호출해야 찾은 파일에 접근할 수 있다.
	// 만약, FindFile함수에서 *.*로 검색하고
	// FindNextFile함수를 호출했을 때 가장 첫번째 파일에 접근한다.

	while (bIsFind)
	{
		// 더이상 찾을게 없거나 마지막 파일이나 폴더를 찾으면 FALSE 반환.
		bIsFind = find.FindNextFile();

		if (find.IsDots()) // 찾은 것이 마커라면 건너뛰어라.
			continue;
		else if (find.IsDirectory()) // 찾은 것이 폴더라면			
			ExtractPathInfo(find.GetFilePath(), rPathInfoLst_Multi, rPathInfoLst_Single); // 파일 찾을 때까지 재귀.
		else // 찾은 것이 파일이라면
		{
			if (find.IsSystem()) // 숨어있는 시스템 파일이라면 건너뛰어라.
				continue;

			/*
			D:\김태윤\4개월차\98기\Framework98\Texture\Stage\
			Effect\BossMultiAttack\BossMultiAttack0.png
			*/
			CString strFullPath = find.GetFilePath();

			bool bIsTiles = strFullPath.Find(L"\\Tiles\\No_Animaition") != -1;
			// 타일 이미지들의 제목이 숫자로 되어있어서 예외처리
			if (!bIsTiles)
			{
				// CString::Replace(old, new)
				// old -> new로 교체
				strFullPath.Replace(L"0.", L"%d.");
			}

			// 상대경로 변환.
			/*
			..\Texture\Stage\Effect\BossMultiAttack\BossMultiAttack%d.png
			*/
			CString strRelative = ConvertRelativePath(strFullPath);

			PATH_INFO* pPathInfo = new PATH_INFO;
			pPathInfo->wstrImgPath = strRelative;

			CString strFileName = L"";
			TCHAR szFileName[MAX_STR] = L"";
			strFileName = ::PathFindFileName(strRelative);
			lstrcpy(szFileName, strFileName);
			::PathRemoveExtension(szFileName);
			pPathInfo->wstrFileName = szFileName;
			///////////////////////////////////////////////////////////////////////////

			TCHAR szBuf[MAX_STR] = L"";
			lstrcpy(szBuf, strRelative);

			// ..\Texture\Stage\Effect\BossMultiAttack
			::PathRemoveFileSpec(szBuf);

			pPathInfo->iImgCount = CountImageFile(szBuf);

			///////////////////////////////////////////////////////////////////////

			// 파일명을 찾는다. 파일명이 없으면 말단 폴더명을 찾는다.
			pPathInfo->wstrStateKey = ::PathFindFileName(szBuf);

			if (!lstrcmp(pPathInfo->wstrStateKey.c_str(), L"Single") || bIsTiles)
			{
				::PathRemoveFileSpec(szBuf);
				pPathInfo->wstrObjectKey = ::PathFindFileName(szBuf);
				rPathInfoLst_Single.push_back(pPathInfo);
				continue;
			}

			///////////////////////////////////////////////////////////////////////////

			// ..\Texture\Stage\Effect
			::PathRemoveFileSpec(szBuf);
			pPathInfo->wstrObjectKey = ::PathFindFileName(szBuf);

			rPathInfoLst_Multi.push_back(pPathInfo);
			break;
		}

	}
}

// ../Texture/Stage/Effect/BossMultiAttack
int CFileInfo::CountImageFile(const TCHAR* pPath)
{
	wstring wstrFindPath = wstring(pPath) + L"\\*.*";
	int iCount = 0;

	CFileFind find;

	// ../Texture/Stage/Effect/BossMultiAttack/*.*
	BOOL bIsFind = find.FindFile(wstrFindPath.c_str());

	while (bIsFind)
	{
		bIsFind = find.FindNextFile();

		if (find.IsDots() || find.IsDirectory() || find.IsSystem())
			continue;

		++iCount;
	}

	return iCount;
}

// Get StateKey (FileName) Only
void CFileInfo::GetMapToolFiles(const TCHAR* pFullPath, list<PATH_INFO*>& rPathInfoLst)
{
	wstring wstrFindPath = wstring(pFullPath) + L"\\*.*";

	CFileFind find;

	BOOL bIsFind = find.FindFile(wstrFindPath.c_str());

	while (bIsFind)
	{
		// 더이상 찾을게 없거나 마지막 파일이나 폴더를 찾으면 FALSE 반환.
		bIsFind = find.FindNextFile();

		if (find.IsDots()) // 찾은 것이 마커라면 건너뛰어라.
			continue;
		else if (find.IsDirectory()) // 찾은 것이 폴더라면			
			ExtractPathInfo(find.GetFilePath(), rPathInfoLst, rPathInfoLst); // 파일 찾을 때까지 재귀.
		else // 찾은 것이 파일이라면
		{
			if (find.IsSystem()) // 숨어있는 시스템 파일이라면 건너뛰어라.
				continue;

			CString strFullPath = find.GetFilePath();

			CString strRelative = ConvertRelativePath(strFullPath);

			PATH_INFO* pPathInfo = new PATH_INFO;
			pPathInfo->wstrImgPath = strRelative;

			///////////////////////////////////////////////////////////////////////////

			TCHAR szBuf[MAX_STR] = L"";
			lstrcpy(szBuf, strRelative);

			::PathRemoveExtension(szBuf);
			pPathInfo->wstrStateKey = ::PathFindFileName(szBuf);

			rPathInfoLst.push_back(pPathInfo);
		}
	}
}
