#pragma once
class CFileInfo
{
private:
	CFileInfo();
	~CFileInfo();

public:
	static CString ConvertRelativePath(const TCHAR* pFullPath);
	static void ExtractPathInfo(const TCHAR* pPath, list<ENGINE::PATH_INFO*>& rPathInfoLst, list<ENGINE::PATH_INFO*>& rPathInfoLst_Single, bool bForClientPath = false);
	static int CountImageFile(const TCHAR* pPath);
	static void GetMapToolFiles(const TCHAR * pFullPath, list<ENGINE::PATH_INFO*>& rPathInfoLst);
};

