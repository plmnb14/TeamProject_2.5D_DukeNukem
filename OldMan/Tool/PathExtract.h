#pragma once
#ifndef __PATHEXTRACT_H__
class CPathExtract
{
public:
	CPathExtract();
	virtual ~CPathExtract();

public:
	void MakePathFile();
	void ExportFile();

	void Release();

private:
	void OnDropFiles(wstring _wstrPath, bool _bForClient = false);
	void OnBnClickedSave(bool _bForClient = false);
	void OnBnClickedLoad();

public:
	list<ENGINE::PATH_INFO*>	m_PathInfoLst_Multi;
	list<ENGINE::PATH_INFO*>	m_PathInfoLst_Single;
};
#define __PATHEXTRACT_H__
#endif
