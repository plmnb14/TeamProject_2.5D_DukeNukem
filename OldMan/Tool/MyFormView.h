#pragma once

// CMyFormView 폼 뷰입니다.

#include "MapObjectSelectDlg.h"
#include "afxwin.h"

class CMyFormView : public CFormView
{
	DECLARE_DYNCREATE(CMyFormView)

public:
	enum VALUE_IDX { X, Y, Z, VALUE_END };

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
	afx_msg void OnBnClickedButtonTile();
	afx_msg void OnBnClickedButtonMonster();
	afx_msg void OnBnClickedButtonTrigger();
	afx_msg void OnBnClickedButtonMapObj();
	virtual void OnInitialUpdate();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnEnChangeEdit4();
	afx_msg void OnEnChangeEdit5();
	afx_msg void OnEnChangeEdit6();
	afx_msg void OnEnChangeEdit7();
	afx_msg void OnEnChangeEdit8();
	afx_msg void OnEnChangeEdit9();
	afx_msg void OnEnChangeEdit10();
	afx_msg void OnBnClickedButton_Save();
	afx_msg void OnBnClickedButton_Load();
	afx_msg void OnBnClickedButton_PathUpdate();

public:
	void UpdatePicture(wstring _wstrName, wstring _wstrPath);
	void UpdateTransformStr(D3DXVECTOR3 _vPos, D3DXVECTOR3 _vRot, D3DXVECTOR3 _vSize);
	void UpdateIndex();
	void EditDataExchange();
	void InitData();
	void Update();
	D3DXVECTOR3 GetPositionVec();
	D3DXVECTOR3 GetRotationVec();
	D3DXVECTOR3 GetScaleVec();

private:
	bool CheckNumber(CString _str, float& _fValue);

public:
	CStatic m_PictureControl;
	CButton m_TerrainTypeRadioBtn[3];
	CButton m_CheckButton_Grid;

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

	CString m_strIndex;

public:
	CMapObjectSelectDlg m_ObjSelect_Tile;
	CMapObjectSelectDlg m_ObjSelect_Monster;
	CMapObjectSelectDlg m_ObjSelect_Trigger;
	CMapObjectSelectDlg m_ObjSelect_MapObj;

	CImage m_Img;
	wstring	m_wstrObjType;
	wstring m_wstrFileName;
	wstring m_wstrFilePath;

	float m_fPositionValue[VALUE_END];
	float m_fRotaionValue[VALUE_END];
	float m_fScaleValue[VALUE_END];

	ENGINE::TERRAIN_TYPE m_eTerrainType;

	bool m_bIsOnDlg;
	int m_iIndex;
	afx_msg void OnEnChangeEdit11();
};

