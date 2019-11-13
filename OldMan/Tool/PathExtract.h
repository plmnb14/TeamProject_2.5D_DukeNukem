#pragma once
#ifndef __PATHEXTRACT_H__
class CPathExtract
{
public:
	CPathExtract();
	virtual ~CPathExtract();

public:
	void MakePathFile();

private:
	void OnDropFiles(wstring _wstrPath);
	void OnBnClickedSave();
	void OnBnClickedLoad();

public:
	list<PATH_INFO*>	m_PathInfoLst_Multi;
	list<PATH_INFO*>	m_PathInfoLst_Single;
};
#define __PATHEXTRACT_H__
#endif
