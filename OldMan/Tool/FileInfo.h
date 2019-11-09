#pragma once
class CFileInfo
{
private:
	CFileInfo();
	~CFileInfo();

public:
	static CString ConvertRelativePath(const TCHAR* pFullPath);
	static void ExtractPathInfo(const TCHAR* pPath, list<PATH_INFO*>& rPathInfoLst, list<PATH_INFO*>& rPathInfoLst_Single);
	static int CountImageFile(const TCHAR* pPath);
	static void GetMapToolFiles(const TCHAR * pFullPath, list<PATH_INFO*>& rPathInfoLst);
};

