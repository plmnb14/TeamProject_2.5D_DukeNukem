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
	Release();
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
	case CMapObjectSelectDlg::OBJ_TILE:
	{
		strRelativePath = L"..\\Client\\Texture\\Tiles\\No_Animation";

		if (!lstrcmp(strRelativePath, L""))
			return;

		m_ListBox.ResetContent();

		CFileInfo::GetMapToolFiles(strRelativePath, m_listFileInfo);

		auto& iter_begin = m_listFileInfo.begin();
		auto& iter_end = m_listFileInfo.end();
		for (; iter_begin != iter_end; )
		{
			CString strCheckDDS = (*iter_begin)->wstrFileName.c_str();
			if (strCheckDDS.Find(L".dds") > 0)
			{
				iter_begin = m_listFileInfo.erase(iter_begin);
				continue;
			}

			m_ListBox.AddString((*iter_begin)->wstrFileName.c_str());
			iter_begin++;
		}

		return;
	}
	case CMapObjectSelectDlg::OBJ_MONSTER:
	{
		CFileInfo::GetMonsterInfoFromTextFile(L"../Data/MonsterInfo.txt", m_listFileInfo);

		m_ListBox.ResetContent();

		for (auto& iter : m_listFileInfo)
		{
			m_ListBox.AddString((iter->wstrObjectKey + L"|" + iter->wstrFileName).c_str());
		}

		return;
	}
	case CMapObjectSelectDlg::OBJ_TRIGGER:
	{
		CFileInfo::GetMonsterInfoFromTextFile(L"../Data/TriggerInfo.txt", m_listFileInfo);

		m_ListBox.ResetContent();

		for (auto& iter : m_listFileInfo)
		{
			m_ListBox.AddString((iter->wstrObjectKey + L"|" + iter->wstrFileName).c_str());
		}

		return;
	}
	case CMapObjectSelectDlg::OBJ_MAPOBJ:
	{
		CFileInfo::GetMonsterInfoFromTextFile(L"../Data/MapObjInfo.txt", m_listFileInfo);

		m_ListBox.ResetContent();

		for (auto& iter : m_listFileInfo)
		{
			m_ListBox.AddString((iter->wstrObjectKey + L"|" + iter->wstrFileName).c_str());
		}

		return;
	}
	}
}

void CMapObjectSelectDlg::Release()
{
	for_each(m_listFileInfo.begin(), m_listFileInfo.end(), ENGINE::Safe_Delete<ENGINE::PATH_INFO*>);
	m_listFileInfo.clear();
}


BEGIN_MESSAGE_MAP(CMapObjectSelectDlg, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST1, &CMapObjectSelectDlg::OnLbnSelchangeListBox)
END_MESSAGE_MAP()


// CObjectSelectDlg 메시지 처리기입니다.


void CMapObjectSelectDlg::OnLbnSelchangeListBox()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	NULL_CHECK(pMainFrm);

	// GetPane(row, col): row, col 위치에 배치된 CWnd* 를 반환하는 CSplitterWnd의 멤버 함수.
	CMyFormView* pFormView = dynamic_cast<CMyFormView*>(pMainFrm->m_MainSplitter.GetPane(0, 0));
	NULL_CHECK(pFormView);

	CToolView* pView = dynamic_cast<CToolView*>(pMainFrm->m_MainSplitter.GetPane(0, 1));
	NULL_CHECK(pView);

	int iIndex = m_ListBox.GetCurSel();

	if (-1 == iIndex)
		return;

	CString strFileName = L"";
	TCHAR szObjectKey[MAX_STR] = L"";
	TCHAR szFileName[MAX_STR] = L"";

	m_ListBox.GetText(iIndex, strFileName);

	switch (m_eType)
	{
	case CMapObjectSelectDlg::OBJ_TILE:
	{
		pFormView->m_wstrObjType = L"Terrain";
		break;
	}
	case CMapObjectSelectDlg::OBJ_MONSTER:
	case CMapObjectSelectDlg::OBJ_MAPOBJ:
	case CMapObjectSelectDlg::OBJ_TRIGGER:
	{
		lstrcpy(szObjectKey, strFileName.Left(strFileName.Find('|')));
		lstrcpy(szFileName, strFileName.Mid(strFileName.Find('|') + 1, strFileName.GetLength()));

		pFormView->m_wstrObjType = szObjectKey;
		strFileName = szFileName;
		break;
	}
	}

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
		m_Img.Load(L"..\\Client\\Texture\\Tiles\\No_Animation\\64 x 64\\Tile64x64_9.png");
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

	CString strCheckDDS = (*iter_begin)->wstrImgPath.c_str();
	strCheckDDS.Replace(L".dds", L".png");

	m_Img.Load(strCheckDDS);

	int iWidth = (m_Img.GetWidth() / StaticPictureRect.Width()) *  StaticPictureRect.Width();
	int iHeeght = (m_Img.GetHeight() / StaticPictureRect.Height() *  StaticPictureRect.Height());
	if (iWidth <= 0) iWidth = StaticPictureRect.Width();
	if (iHeeght <= 0) iHeeght = StaticPictureRect.Height();

	m_Img.Draw(m_PictureControl.GetWindowDC()->m_hDC,
		StaticPictureRect.TopLeft().x,
		StaticPictureRect.TopLeft().y,
		iWidth,
		iHeeght);
	

	pFormView->UpdatePicture(m_wstrTex, (*iter_begin)->wstrImgPath);
	pView->SelectObjAfter();

	UpdateData(FALSE);

}
