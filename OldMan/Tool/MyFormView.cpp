// MyFormView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "MyFormView.h"


// CMyFormView

IMPLEMENT_DYNCREATE(CMyFormView, CFormView)

CMyFormView::CMyFormView()
	: CFormView(IDD_MYFORMVIEW),
	m_strObjectName(_T("")),
	m_wstrFileName(L""), m_wstrFilePath(L""),
	m_eTerrainType(TERRAIN_END)
{

}

CMyFormView::~CMyFormView()
{
}

void CMyFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FORMPICTURE, m_PictureControl);
	DDX_Text(pDX, IDC_EDIT1, m_strObjectName);
	DDX_Control(pDX, IDC_BUTTON1, m_TerrainTypeRadioBtn[0]);
	DDX_Control(pDX, IDC_BUTTON2, m_TerrainTypeRadioBtn[1]);
	DDX_Control(pDX, IDC_BUTTON3, m_TerrainTypeRadioBtn[2]);
}

BEGIN_MESSAGE_MAP(CMyFormView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON1, &CMyFormView::OnBnClickedButtonMapObj)
	ON_BN_CLICKED(IDC_BUTTON2, &CMyFormView::OnBnClickedButtonMonster)
	ON_BN_CLICKED(IDC_BUTTON3, &CMyFormView::OnBnClickedButtonTrigger)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CMyFormView �����Դϴ�.

#ifdef _DEBUG
void CMyFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMyFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMyFormView �޽��� ó�����Դϴ�.


void CMyFormView::OnBnClickedButtonMapObj()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ� �߰��մϴ�.
	m_ObjSelect_Map.ShowWindow(SW_SHOW);
}


void CMyFormView::OnBnClickedButtonMonster()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_ObjSelect_Monster.ShowWindow(SW_SHOW);
}


void CMyFormView::OnBnClickedButtonTrigger()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_ObjSelect_Trigger.ShowWindow(SW_SHOW);
}


void CMyFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	if (nullptr == m_ObjSelect_Map.GetSafeHwnd())
	{
		m_ObjSelect_Map.Create(IDD_OBJECTSELECTDLG);
		m_ObjSelect_Map.SetType(CObjectSelectDlg::OBJ_MAP);
	}

	if (nullptr == m_ObjSelect_Monster.GetSafeHwnd())
	{
		m_ObjSelect_Monster.Create(IDD_OBJECTSELECTDLG);
		m_ObjSelect_Map.SetType(CObjectSelectDlg::OBJ_MONSTER);
	}

	if (nullptr == m_ObjSelect_Trigger.GetSafeHwnd())
	{
		m_ObjSelect_Trigger.Create(IDD_OBJECTSELECTDLG);
		m_ObjSelect_Map.SetType(CObjectSelectDlg::OBJ_TRIGGER);
	}
}

void CMyFormView::UpdatePicture(wstring _wstrName, wstring _wstrPath)
{
	UpdateData(TRUE);

	//CString strFileName = L"";
	m_strObjectName = _wstrName.c_str();
	m_wstrFileName = _wstrName;
	m_wstrFilePath = _wstrPath;

	if (!lstrcmp(m_wstrFileName.c_str(), L""))
		return;

	CRect StaticPictureRect;
	m_PictureControl.GetClientRect(StaticPictureRect);

	m_Img.Destroy();
	m_Img.Load(_wstrPath.c_str());

	int iWidth = (m_Img.GetWidth() / StaticPictureRect.Width()) *  StaticPictureRect.Width();
	int iHeeght = (m_Img.GetHeight() / StaticPictureRect.Height() *  StaticPictureRect.Height());
	if (iWidth <= 0) iWidth = StaticPictureRect.Width();
	if (iHeeght <= 0) iHeeght = StaticPictureRect.Height();

	m_Img.Draw(m_PictureControl.GetWindowDC()->m_hDC,
		StaticPictureRect.TopLeft().x,
		StaticPictureRect.TopLeft().y,
		iWidth,
		iHeeght);

	UpdateData(FALSE);
}


void CMyFormView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CFormView::OnLButtonDown(nFlags, point);

	UpdateData(TRUE);

	UpdatePicture(m_wstrFileName, m_wstrFilePath);

	for (int i = 0; i < 3; i++)
		if ((m_TerrainTypeRadioBtn[i].GetCheck() >= 1)) m_eTerrainType = (TERRAIN_TYPE)i;

	UpdateData(FALSE);

}
