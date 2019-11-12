#pragma once
#include "afxwin.h"


// CObjectSelectDlg ��ȭ �����Դϴ�.

class CObjectSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CObjectSelectDlg)

public: 
	enum OBJECT_TYPE { OBJ_MAP, OBJ_MONSTER, OBJ_TRIGGER, OBJ_END };

public:
	CObjectSelectDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CObjectSelectDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OBJECTSELECTDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	afx_msg void OnLbnSelchangeListBox();

	DECLARE_MESSAGE_MAP()

public:
	CListBox m_ListBox;
	CStatic m_PictureControl;

public:
	void SetType(OBJECT_TYPE _eType);

private:
	void SetData();

public:
	OBJECT_TYPE				m_eType;
	list<PATH_INFO*>		m_listFileInfo;
	wstring					m_wstrTex;
	CImage					m_Img;
};