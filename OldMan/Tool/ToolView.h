
// ToolView.h : CToolView Ŭ������ �������̽�
//

#pragma once

#include "Layer.h"

class CToolTerrain;
class CToolDoc;
class CToolView : public CView
{
protected: // serialization������ ��������ϴ�.
	CToolView();
	DECLARE_DYNCREATE(CToolView)

// Ư���Դϴ�.
public:
	CToolDoc* GetDocument() const;

// �۾��Դϴ�.
// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �����Դϴ�.
public:
	virtual ~CToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

public:
	void SelectObjAfter();
	void ChangeValueAfter();
	void ChangeTerrainType();
	void AddCubeForLoad(CToolTerrain* _pTerrain);

	void Update();

private:
	void PipeLineSetup();
	void LoadTexture();
	HRESULT Initialize();
	HRESULT Add_Environment_Layer();
	HRESULT Add_Object_Layer();
	//HRESULT Add_UI_Layer();
	void CubeMoveToMouse();
	void CreateCube(bool _bIsChange);
	void DragPicking(float _fPointX, float _fPointY);
	void CheckDeleteCube();
	bool CheckGrid();

public:
	ENGINE::CGraphicDev*			m_pDeviceMgr;
	ENGINE::CResourceMgr*			m_pResourceMgr;
	
	list<CToolTerrain*>				m_pCubeList;
	CToolTerrain*					m_pSelectCube;
	CToolTerrain*					m_pDeleteCube;

	typedef map<WORD, ENGINE::CLayer*>	MAP_LAYER;
	MAP_LAYER	m_mapLayer;

	D3DXVECTOR3 m_vLastPickedCubePos;
	bool m_bIsMousePressing;
	POINT m_ptFitCursorPos;

	D3DXMATRIX m_ViewMatrix;
	D3DXMATRIX m_ProjMatrix;

	bool m_bIsEraseMode;

};

#ifndef _DEBUG  // ToolView.cpp�� ����� ����
inline CToolDoc* CToolView::GetDocument() const
   { return reinterpret_cast<CToolDoc*>(m_pDocument); }
#endif

