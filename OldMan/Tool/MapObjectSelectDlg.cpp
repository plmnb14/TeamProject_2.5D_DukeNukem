// ObjectSelectDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "MapObjectSelectDlg.h"
#include "afxdialogex.h"

#include "FileInfo.h"

#include "MainFrm.h"
#include "ToolView.h"
#include "MyFormView.h"


// CObjectSelectDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMapObjectSelectDlg, CDialog)

CMapObjectSelectDlg::CMapObjectSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_OBJECTSELECTDLG, pParent)
{

}

CMapObjectSelectDlg::~CMapObjectSelectDlg()
{
}

void CMapObjectSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_PICTURE, m_PictureControl);
}

void CMapObjectSelectDlg::SetType(OBJECT_TYPE _eType)
{
	m_eType = _eType;
	SetData();
}

void CMapObjectSelectDlg::SetData()
{
	CString strRelativePath = L"";

	switch (m_eType)
	{
	case CMapObjectSelectDlg::OBJ_MAP:
	{
		strRelativePath = L"..\\Client\\Texture\\Tiles\\No_Animaition\\64 x 64";
		break;
	}
	case CMapObjectSelectDlg::OBJ_MONSTER:
	{
		strRelativePath = L"..\\Client\\Texture\\Monster";
		break;
	}
	case CMapObjectSelectDlg::OBJ_TRIGGER:
		break;
	case CMapObjectSelectDlg::OBJ_END:
		break;
	default:
		break;
	}

	if (!lstrcmp(strRelativePath, L""))
		return;

	m_ListBox.ResetContent();

	CFileInfo::GetMapToolFiles(strRelativePath, m_listFileInfo);

	for (auto& iter : m_listFileInfo)
	{
		m_ListBox.AddString(iter->wstrStateKey.c_str());
	}
}


BEGIN_MESSAGE_MAP(CMapObjectSelectDlg, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST1, &CMapObjectSelectDlg::OnLbnSelchangeListBox)
END_MESSAGE_MAP()


// CObjectSelectDlg 메시지 처리기입니다.


void CMapObjectSelectDlg::OnLbnSelchangeListBox()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	int iIndex = m_ListBox.GetCurSel();

	if (-1 == iIndex)
		return;

	CString strFileName = L"";
	m_ListBox.GetText(iIndex, strFileName);
	m_wstrTex = strFileName;
	auto& iter_begin = m_listFileInfo.begin();

	for (int i = 0; i < iIndex; i++)
		iter_begin++;

	CRect StaticPictureRect;
	m_PictureControl.GetClientRect(StaticPictureRect);

	if (!m_Img.IsNull())
	{
		m_Img.Destroy();
		m_PictureControl.SetBitmap(NULL);

		// CImage 초기화가 안되서 임시 방편.
		m_Img.Load(L"..\\Client\\Texture\\Tiles\\No_Animaition\\64 x 64\\Tile64_09.png");
		int iWidth = (m_Img.GetWidth() / StaticPictureRect.Width()) *  StaticPictureRect.Width();
		int iHeight = (m_Img.GetHeight() / StaticPictureRect.Height() *  StaticPictureRect.Height());
		if (iWidth <= 0) iWidth = StaticPictureRect.Width();
		if (iHeight <= 0) iHeight = StaticPictureRect.Height();

		m_Img.Draw(m_PictureControl.GetWindowDC()->m_hDC,
			StaticPictureRect.TopLeft().x,
			StaticPictureRect.TopLeft().y,
			iWidth,
			iHeight);
		m_Img.Destroy();
	}

	m_Img.Load((*iter_begin)->wstrImgPath.c_str());

	int iWidth = (m_Img.GetWidth() / StaticPictureRect.Width()) *  StaticPictureRect.Width();
	int iHeeght = (m_Img.GetHeight() / StaticPictureRect.Height() *  StaticPictureRect.Height());
	if (iWidth <= 0) iWidth = StaticPictureRect.Width();
	if (iHeeght <= 0) iHeeght = StaticPictureRect.Height();

	m_Img.Draw(m_PictureControl.GetWindowDC()->m_hDC,
		StaticPictureRect.TopLeft().x,
		StaticPictureRect.TopLeft().y,
		iWidth,
		iHeeght);


	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	NULL_CHECK(pMainFrm);

	// GetPane(row, col): row, col 위치에 배치된 CWnd* 를 반환하는 CSplitterWnd의 멤버 함수.
	CMyFormView* pFormView = dynamic_cast<CMyFormView*>(pMainFrm->m_MainSplitter.GetPane(0, 0));
	NULL_CHECK(pFormView);
	pFormView->UpdatePicture(m_wstrTex, (*iter_begin)->wstrImgPath);

	CToolView* pView = dynamic_cast<CToolView*>(pMainFrm->m_MainSplitter.GetPane(0, 1));
	NULL_CHECK(pView);
	pView->SelectObjAfter();

	UpdateData(FALSE);

}
