#pragma once

// CMyFormView 폼 뷰입니다.

#include "ObjectSelectDlg.h"
#include "afxwin.h"

class CMyFormView : public CFormView
{
	DECLARE_DYNCREATE(CMyFormView)

public:
	enum TERRAIN_TYPE { TERRAIN_CUBE, TERRAIN_WALL, TERRAIN_RECT, TERRAIN_END };

protected:
	CMyFormView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CMyFormView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYFORMVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonMapObj();
	afx_msg void OnBnClickedButtonMonster();
	afx_msg void OnBnClickedButtonTrigger();
	virtual void OnInitialUpdate();

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

public:
	void UpdatePicture(wstring _wstrName, wstring _wstrPath);
	void UpdateTransformStr(D3DXVECTOR3 _vPos, D3DXVECTOR3 _vRot, D3DXVECTOR3 _vSize);
	D3DXVECTOR3 GetPositionVec();
	D3DXVECTOR3 GetRotationVec();
	D3DXVECTOR3 GetScaleVec();

public:
	CStatic m_PictureControl;
	CButton m_TerrainTypeRadioBtn[3];

	CString m_strObjectName;
	CString m_strPositionX;
	CString m_strPositionY;
	CString m_strPositionZ;
	CString m_strRotationX;
	CString m_strRotationY;
	CString m_strRotationZ;
	CString m_strScaleX;
	CString m_strScaleY;
	CString m_strScaleZ;

public:
	CObjectSelectDlg m_ObjSelect_Map;
	CObjectSelectDlg m_ObjSelect_Monster;
	CObjectSelectDlg m_ObjSelect_Trigger;

	CImage m_Img;
	wstring m_wstrFileName;
	wstring m_wstrFilePath;

	TERRAIN_TYPE m_eTerrainType;
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

