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

	//// From���� To�� �Ѿ�� ����θ� ���� Output�� �����ϴ� �Լ�.
	//::PathRelativePathTo(Output, From, FILE_ATTRIBUTE_DIRECTORY,
	//	To, FILE_ATTRIBUTE_DIRECTORY);
	// ��, From�� To�� ���� ����̺� �ȿ� �־�� �Ѵ�.
	::PathRelativePathTo(szRelativePath, szCurrentDir, FILE_ATTRIBUTE_DIRECTORY,
		pFullPath, FILE_ATTRIBUTE_DIRECTORY);

	return CString(szRelativePath);
}

void CFileInfo::ExtractPathInfo(
	const TCHAR* pPath, /* D:\\������\\4������\\98��\\Framework98\\Texture\\Stage\\Effect\\BossMultiAttack */
	list<PATH_INFO*>& rPathInfoLst_Multi, list<PATH_INFO*>& rPathInfoLst_Single)
{
	wstring wstrFindPath = wstring(pPath) + L"\\*.*";

	// ���� �˻��� ������ MFC Ŭ����
	CFileFind find;
	// CFileFind::FindFile(FileName)
	// ��ο� FileName�� �����ϴ��� ������ �����ϴ� �Լ�.
	// �����Ѵٸ� TRUE ��ȯ, �ƴϸ� FALSE ��ȯ.
	// ����, *.*�� ���ڷ� �ѱ�ٸ� �� �� �ϳ��� ã���� TRUE.

	// D:\\������\\4������\\98��\\Framework98\\Texture\\Stage\\Effect\\BossMultiAttack\\*.*
	BOOL bIsFind = find.FindFile(wstrFindPath.c_str());

	// FindFile�Լ��� �����̳� ���� ������ �˻������� 
	// FindNextFile�Լ��� ȣ���ؾ� ã�� ���Ͽ� ������ �� �ִ�.
	// ����, FindFile�Լ����� *.*�� �˻��ϰ�
	// FindNextFile�Լ��� ȣ������ �� ���� ù��° ���Ͽ� �����Ѵ�.

	while (bIsFind)
	{
		// ���̻� ã���� ���ų� ������ �����̳� ������ ã���� FALSE ��ȯ.
		bIsFind = find.FindNextFile();

		if (find.IsDots()) // ã�� ���� ��Ŀ��� �ǳʶپ��.
			continue;
		else if (find.IsDirectory()) // ã�� ���� �������			
			ExtractPathInfo(find.GetFilePath(), rPathInfoLst_Multi, rPathInfoLst_Single); // ���� ã�� ������ ���.
		else // ã�� ���� �����̶��
		{
			if (find.IsSystem()) // �����ִ� �ý��� �����̶�� �ǳʶپ��.
				continue;

			/*
			D:\������\4������\98��\Framework98\Texture\Stage\
			Effect\BossMultiAttack\BossMultiAttack0.png
			*/
			CString strFullPath = find.GetFilePath();

			bool bIsTiles = strFullPath.Find(L"\\Tiles\\No_Animaition") != -1;
			// Ÿ�� �̹������� ������ ���ڷ� �Ǿ��־ ����ó��
			if (!bIsTiles)
			{
				// CString::Replace(old, new)
				// old -> new�� ��ü
				strFullPath.Replace(L"0.", L"%d.");
			}

			// ����� ��ȯ.
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

			// ���ϸ��� ã�´�. ���ϸ��� ������ ���� �������� ã�´�.
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
		// ���̻� ã���� ���ų� ������ �����̳� ������ ã���� FALSE ��ȯ.
		bIsFind = find.FindNextFile();

		if (find.IsDots()) // ã�� ���� ��Ŀ��� �ǳʶپ��.
			continue;
		else if (find.IsDirectory()) // ã�� ���� �������			
			ExtractPathInfo(find.GetFilePath(), rPathInfoLst, rPathInfoLst); // ���� ã�� ������ ���.
		else // ã�� ���� �����̶��
		{
			if (find.IsSystem()) // �����ִ� �ý��� �����̶�� �ǳʶپ��.
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
