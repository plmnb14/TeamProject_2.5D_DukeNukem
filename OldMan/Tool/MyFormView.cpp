// MyFormView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "MyFormView.h"

#include "MainFrm.h"
#include "ToolView.h"

#include "ToolTerrain.h"
#include "Trasform.h"

#include "ToolTerrainCube.h"
#include "ToolTerrainWallCube.h"
#include "ToolTerrainRect.h"

#include "PathExtract.h"

// CMyFormView

IMPLEMENT_DYNCREATE(CMyFormView, CFormView)

CMyFormView::CMyFormView()
	: CFormView(IDD_MYFORMVIEW),
	m_strObjectName(_T("")),
	m_wstrFileName(L"Tile64x64_9.png"), m_wstrFilePath(L""),
	m_eTerrainType(ENGINE::TERRAIN_END)
	, m_strPositionX(_T("0"))
	, m_strPositionY(_T("0"))
	, m_strPositionZ(_T("0"))
	, m_strRotationX(_T("0"))
	, m_strRotationY(_T("0"))
	, m_strRotationZ(_T("0"))
	, m_strScaleX(_T("1"))
	, m_strScaleY(_T("1"))
	, m_strScaleZ(_T("1"))
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
	DDX_Control(pDX, IDC_RADIO1, m_TerrainTypeRadioBtn[0]);
	DDX_Control(pDX, IDC_RADIO2, m_TerrainTypeRadioBtn[1]);
	DDX_Control(pDX, IDC_RADIO3, m_TerrainTypeRadioBtn[2]);
	DDX_Text(pDX, IDC_EDIT2, m_strPositionX);
	DDX_Text(pDX, IDC_EDIT3, m_strPositionY);
	DDX_Text(pDX, IDC_EDIT4, m_strPositionZ);
	DDX_Text(pDX, IDC_EDIT5, m_strRotationX);
	DDX_Text(pDX, IDC_EDIT6, m_strRotationY);
	DDX_Text(pDX, IDC_EDIT7, m_strRotationZ);
	DDX_Text(pDX, IDC_EDIT8, m_strScaleX);
	DDX_Text(pDX, IDC_EDIT9, m_strScaleY);
	DDX_Text(pDX, IDC_EDIT10, m_strScaleZ);
	DDX_Control(pDX, IDC_CHECK1, m_CheckButton_Grid);
}

BEGIN_MESSAGE_MAP(CMyFormView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON1, &CMyFormView::OnBnClickedButtonMapObj)
	ON_BN_CLICKED(IDC_BUTTON2, &CMyFormView::OnBnClickedButtonMonster)
	ON_BN_CLICKED(IDC_BUTTON3, &CMyFormView::OnBnClickedButtonTrigger)
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_EN_CHANGE(IDC_EDIT9, &CMyFormView::OnEnChangeEdit9)
	ON_EN_CHANGE(IDC_EDIT2, &CMyFormView::OnEnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT3, &CMyFormView::OnEnChangeEdit3)
	ON_EN_CHANGE(IDC_EDIT4, &CMyFormView::OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT5, &CMyFormView::OnEnChangeEdit5)
	ON_EN_CHANGE(IDC_EDIT6, &CMyFormView::OnEnChangeEdit6)
	ON_EN_CHANGE(IDC_EDIT7, &CMyFormView::OnEnChangeEdit7)
	ON_EN_CHANGE(IDC_EDIT8, &CMyFormView::OnEnChangeEdit8)
	ON_EN_CHANGE(IDC_EDIT10, &CMyFormView::OnEnChangeEdit10)
	ON_BN_CLICKED(IDC_BUTTON4, &CMyFormView::OnBnClickedButton_Save)
	ON_BN_CLICKED(IDC_BUTTON5, &CMyFormView::OnBnClickedButton_Load)
	ON_BN_CLICKED(IDC_BUTTON6, &CMyFormView::OnBnClickedButton_PathUpdate)
END_MESSAGE_MAP()


// CMyFormView 진단입니다.

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


// CMyFormView 메시지 처리기입니다.


void CMyFormView::OnBnClickedButtonMapObj()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드 추가합니다.
	m_ObjSelect_Map.ShowWindow(SW_SHOW);
}


void CMyFormView::OnBnClickedButtonMonster()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_ObjSelect_Monster.ShowWindow(SW_SHOW);
}


void CMyFormView::OnBnClickedButtonTrigger()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_ObjSelect_Trigger.ShowWindow(SW_SHOW);
}


void CMyFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (nullptr == m_ObjSelect_Map.GetSafeHwnd())
	{
		m_ObjSelect_Map.Create(IDD_OBJECTSELECTDLG);
		m_ObjSelect_Map.SetType(CMapObjectSelectDlg::OBJ_MAP);
	}

	if (nullptr == m_ObjSelect_Monster.GetSafeHwnd())
	{
		m_ObjSelect_Monster.Create(IDD_OBJECTSELECTDLG);
		m_ObjSelect_Monster.SetType(CMapObjectSelectDlg::OBJ_MONSTER);
	}

	if (nullptr == m_ObjSelect_Trigger.GetSafeHwnd())
	{
		m_ObjSelect_Trigger.Create(IDD_OBJECTSELECTDLG);
		m_ObjSelect_Trigger.SetType(CMapObjectSelectDlg::OBJ_TRIGGER);
	}

	m_TerrainTypeRadioBtn[0].SetCheck(true);
	m_CheckButton_Grid.SetCheck(true);
}

void CMyFormView::UpdatePicture(wstring _wstrName, wstring _wstrPath)
{
	UpdateData(TRUE);

	//CString strFileName = L"";
	m_strObjectName = _wstrName.c_str();
	m_wstrFileName = _wstrName;
	m_wstrFilePath = _wstrPath;

	if (!lstrcmp(m_wstrFilePath.c_str(), L""))
		return;

	CRect StaticPictureRect;
	m_PictureControl.GetClientRect(StaticPictureRect);

	if (!m_Img.IsNull())
	{
		m_Img.Destroy();
		//m_PictureControl.SetBitmap(NULL);

		//// CImage 초기화가 안되서 임시 방편.
		//m_Img.Load(L"..\\Client\\Texture\\Tiles\\No_Animation\\64 x 64\\Tile64x64_9.png");
		//int iWidth = (m_Img.GetWidth() / StaticPictureRect.Width()) *  StaticPictureRect.Width();
		//int iHeight = (m_Img.GetHeight() / StaticPictureRect.Height() *  StaticPictureRect.Height());
		//if (iWidth <= 0) iWidth = StaticPictureRect.Width();
		//if (iHeight <= 0) iHeight = StaticPictureRect.Height();

		//m_Img.Draw(m_PictureControl.GetWindowDC()->m_hDC,
		//	StaticPictureRect.TopLeft().x,
		//	StaticPictureRect.TopLeft().y,
		//	iWidth,
		//	iHeight);
		//m_Img.Destroy();
	}

	CString strCheckDDS = m_wstrFilePath.c_str();
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

	UpdateData(FALSE);
}

void CMyFormView::UpdateTransformStr(D3DXVECTOR3 _vPos, D3DXVECTOR3 _vRot, D3DXVECTOR3 _vSize)
{
	UpdateData(TRUE);

	m_strPositionX = to_string(_vPos.x).substr(0, (_vPos.x < 0) ? 5 : 4).c_str();
	m_strPositionY = to_string(_vPos.y).substr(0, (_vPos.y < 0) ? 5 : 4).c_str();
	m_strPositionZ = to_string(_vPos.z).substr(0, (_vPos.z < 0) ? 5 : 4).c_str();

	m_strRotationX = to_string(_vRot.x).substr(0, (_vRot.x < 0) ? 5 : 4).c_str();
	m_strRotationY = to_string(_vRot.y).substr(0, (_vRot.y < 0) ? 5 : 4).c_str();
	m_strRotationZ = to_string(_vRot.z).substr(0, (_vRot.z < 0) ? 5 : 4).c_str();

	m_strScaleX = to_string(_vSize.x).substr(0, (_vSize.x < 0) ? 5 : 4).c_str();
	m_strScaleY = to_string(_vSize.y).substr(0, (_vSize.y < 0) ? 5 : 4).c_str();
	m_strScaleZ = to_string(_vSize.z).substr(0, (_vSize.z < 0) ? 5 : 4).c_str();

	UpdateData(FALSE);
}

D3DXVECTOR3 CMyFormView::GetPositionVec()
{
	return D3DXVECTOR3(stof((wstring)m_strPositionX), stof((wstring)m_strPositionY), stof((wstring)m_strPositionZ));
}

D3DXVECTOR3 CMyFormView::GetRotationVec()
{
	return D3DXVECTOR3(stof((wstring)m_strRotationX), stof((wstring)m_strRotationY), stof((wstring)m_strRotationZ));
}

D3DXVECTOR3 CMyFormView::GetScaleVec()
{
	return D3DXVECTOR3(stof((wstring)m_strScaleX), stof((wstring)m_strScaleY), stof((wstring)m_strScaleZ));
}

void CMyFormView::EditDataExchange()
{
	UpdateData(TRUE);

	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	NULL_CHECK(pMainFrm);

	CToolView* pView = dynamic_cast<CToolView*>(pMainFrm->m_MainSplitter.GetPane(0, 1));
	NULL_CHECK(pView);
	pView->ChangeValueAfter();

	m_fPositionValue[X] = GetPositionVec().x;
	m_fPositionValue[Y] = GetPositionVec().y;
	m_fPositionValue[Z] = GetPositionVec().z;

	m_fRotaionValue[X] = GetRotationVec().x;
	m_fRotaionValue[Y] = GetRotationVec().y;
	m_fRotaionValue[Z] = GetRotationVec().z;

	m_fScaleValue[X] = GetScaleVec().x;
	m_fScaleValue[Y] = GetScaleVec().y;
	m_fScaleValue[Z] = GetScaleVec().z;

	UpdateData(FALSE);
}

void CMyFormView::InitData()
{
	UpdateData(TRUE);

	m_strPositionX = to_string(m_fPositionValue[X]).substr(0, (m_fPositionValue[X] < 0) ? 5 : 4).c_str();
	m_strPositionY = to_string(m_fPositionValue[Y]).substr(0, (m_fPositionValue[Y] < 0) ? 5 : 4).c_str();
	m_strPositionZ = to_string(m_fPositionValue[Z]).substr(0, (m_fPositionValue[Z] < 0) ? 5 : 4).c_str();

	m_strRotationX = to_string(m_fRotaionValue[X]).substr(0, (m_fRotaionValue[X] < 0) ? 5 : 4).c_str();
	m_strRotationY = to_string(m_fRotaionValue[Y]).substr(0, (m_fRotaionValue[Y] < 0) ? 5 : 4).c_str();
	m_strRotationZ = to_string(m_fRotaionValue[Z]).substr(0, (m_fRotaionValue[Z] < 0) ? 5 : 4).c_str();

	m_strScaleX = to_string(m_fScaleValue[X]).substr(0, (m_fScaleValue[X] < 0) ? 5 : 4).c_str();
	m_strScaleY = to_string(m_fScaleValue[Y]).substr(0, (m_fScaleValue[Y] < 0) ? 5 : 4).c_str();
	m_strScaleZ = to_string(m_fScaleValue[Z]).substr(0, (m_fScaleValue[Z] < 0) ? 5 : 4).c_str();
	
	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	NULL_CHECK(pMainFrm);

	CToolView* pView = dynamic_cast<CToolView*>(pMainFrm->m_MainSplitter.GetPane(0, 1));
	NULL_CHECK(pView);
	pView->ChangeValueAfter();

	UpdateData(FALSE);
}

void CMyFormView::Update()
{
	UpdateData(TRUE);

	m_bIsOnDlg = m_ObjSelect_Map.IsWindowVisible();

	for (int i = 0; i < 3; i++)
	{
		if ((m_TerrainTypeRadioBtn[i].GetCheck() >= 1))
		{
			if (m_eTerrainType != (ENGINE::TERRAIN_TYPE)i)
			{
				if (m_eTerrainType != ENGINE::TERRAIN_END)
				{
					CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
					NULL_CHECK(pMainFrm);

					m_eTerrainType = (ENGINE::TERRAIN_TYPE)i;

					CToolView* pView = dynamic_cast<CToolView*>(pMainFrm->m_MainSplitter.GetPane(0, 1));
					NULL_CHECK(pView);
					pView->ChangeTerrainType();
				}

				m_eTerrainType = (ENGINE::TERRAIN_TYPE)i;
			}
		}
	}

	UpdateData(FALSE);
}


void CMyFormView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CFormView::OnLButtonDown(nFlags, point);

	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	NULL_CHECK(pMainFrm);

	CToolView* pView = dynamic_cast<CToolView*>(pMainFrm->m_MainSplitter.GetPane(0, 1));
	NULL_CHECK(pView);
	pView->ChangeValueAfter();
}



void CMyFormView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CFormView::OnMouseMove(nFlags, point);
	UpdatePicture(m_wstrFileName, m_wstrFilePath);
}

void CMyFormView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CFormView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMyFormView::OnEnChangeEdit2()
{
	EditDataExchange();
}

void CMyFormView::OnEnChangeEdit3()
{
	EditDataExchange();
}

void CMyFormView::OnEnChangeEdit4()
{
	EditDataExchange();
}

void CMyFormView::OnEnChangeEdit5()
{
	EditDataExchange();
}

void CMyFormView::OnEnChangeEdit6()
{
	EditDataExchange();
}

void CMyFormView::OnEnChangeEdit7()
{
	EditDataExchange();
}

void CMyFormView::OnEnChangeEdit8()
{
	EditDataExchange();
}

void CMyFormView::OnEnChangeEdit9()
{
	EditDataExchange();
}

void CMyFormView::OnEnChangeEdit10()
{
	EditDataExchange();
}

#pragma endregion


void CMyFormView::OnBnClickedButton_Save()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	NULL_CHECK(pMainFrm);

	// GetPane(row, col): row, col 위치에 배치된 CWnd* 를 반환하는 CSplitterWnd의 멤버 함수.
	CToolView* pView = dynamic_cast<CToolView*>(pMainFrm->m_MainSplitter.GetPane(0, 1));
	NULL_CHECK(pView);

	list<CToolTerrain*> pTerrainList = pView->m_pCubeList;

	UpdateData(TRUE);

	CFileDialog Dlg(FALSE, L".dat", L"제목없음.dat",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Data Files(*.dat)|*.dat||", this);

	TCHAR szCurrentPath[MAX_STR] = L"";

	// 현재 작업경로를 얻어오는 함수
	::GetCurrentDirectory(MAX_STR, szCurrentPath);
	// 현재 경로에서 파일명을 제거하는 함수. 제거할 파일명 없다면 말단 폴더명을 제거 (\Debug ,\Tool)
	::PathRemoveFileSpec(szCurrentPath);

	lstrcat(szCurrentPath, L"\\Data");

	// 절대 경로만 가능
	Dlg.m_ofn.lpstrInitialDir = szCurrentPath;

	if (IDOK == Dlg.DoModal()) // IDOK : 확인, 저장, 열기 등 버튼 눌렀을 때 반환
	{
		HANDLE hFile = ::CreateFile(Dlg.GetPathName(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			FAILED_CHECK_MSG(-1, L"Save Failed. [INVALID_HANDLE_VALUE]");

		DWORD dwByte = 0;
		D3DXVECTOR3 vPos, vSize, vAngle;
		ENGINE::TERRAIN_TYPE eTerrainType;

		for (auto iter : pTerrainList)
		{
			TCHAR szName[MAX_STR] = L"";
			lstrcpy(szName, iter->GetTexName().c_str());

			ENGINE::CTransform* pTransform = dynamic_cast<ENGINE::CTransform*>(iter->Get_Component(L"Transform"));
			vPos = pTransform->GetPos();
			vSize = pTransform->GetSize();
			vAngle = D3DXVECTOR3(pTransform->GetAngle(ENGINE::ANGLE_X), pTransform->GetAngle(ENGINE::ANGLE_Y), pTransform->GetAngle(ENGINE::ANGLE_Z));
			eTerrainType = iter->GetTerrainType();

			::WriteFile(hFile, &szName, sizeof(TCHAR) * MAX_STR, &dwByte, nullptr);
			::WriteFile(hFile, &vPos, sizeof(D3DXVECTOR3), &dwByte, nullptr);
			::WriteFile(hFile, &vSize, sizeof(D3DXVECTOR3), &dwByte, nullptr);
			::WriteFile(hFile, &vAngle, sizeof(D3DXVECTOR3), &dwByte, nullptr);
			::WriteFile(hFile, &eTerrainType, sizeof(ENGINE::TERRAIN_TYPE), &dwByte, nullptr);
		}

		CloseHandle(hFile);
		MessageBox(_T("Save Success."), _T("Save"), MB_OK);
	}

	UpdateData(FALSE);
}


void CMyFormView::OnBnClickedButton_Load()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	NULL_CHECK(pMainFrm);

	// GetPane(row, col): row, col 위치에 배치된 CWnd* 를 반환하는 CSplitterWnd의 멤버 함수.
	CToolView* pView = dynamic_cast<CToolView*>(pMainFrm->m_MainSplitter.GetPane(0, 1));
	NULL_CHECK(pView);

	list<CToolTerrain*> pTerrainList = pView->m_pCubeList;

	CFileDialog Dlg(TRUE, L".dat", L"제목없음.dat",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Data Files(*.dat)|*.dat||", this);

	TCHAR szCurrentPath[MAX_STR] = L"";

	// 현재 작업경로를 얻어오는 함수
	::GetCurrentDirectory(MAX_STR, szCurrentPath);
	// 현재 경로에서 파일명을 제거하는 함수. 제거할 파일명 없다면 말단 폴더명을 제거 (\Debug ,\Tool)
	::PathRemoveFileSpec(szCurrentPath);

	lstrcat(szCurrentPath, L"\\Data");

	// 절대 경로만 가능
	Dlg.m_ofn.lpstrInitialDir = szCurrentPath;

	if (IDOK == Dlg.DoModal()) // IDOK : 확인, 저장, 열기 등 버튼 눌렀을 때 반환
	{
		HANDLE hFile = CreateFile(Dlg.GetPathName(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			FAILED_CHECK_MSG(-1, L"Load Failed. [INVALID_HANDLE_VALUE]");

		for (auto& _rObj : pTerrainList)
		{
			//_rObj->SetDead();
			Safe_Delete(_rObj);
		}
		pView->m_pCubeList.clear();

		DWORD dwByte = 0;
		TCHAR szName[MAX_STR] = L"";
		D3DXVECTOR3 vPos, vSize, vAngle;
		ENGINE::TERRAIN_TYPE eTerrainType;

		while (true)
		{
			::ReadFile(hFile, &szName, sizeof(TCHAR) * MAX_STR, &dwByte, nullptr);
			::ReadFile(hFile, &vPos, sizeof(D3DXVECTOR3), &dwByte, nullptr);
			::ReadFile(hFile, &vSize, sizeof(D3DXVECTOR3), &dwByte, nullptr);
			::ReadFile(hFile, &vAngle, sizeof(D3DXVECTOR3), &dwByte, nullptr);
			::ReadFile(hFile, &eTerrainType, sizeof(ENGINE::TERRAIN_TYPE), &dwByte, nullptr);

			if (0 == dwByte)
				break;

			CToolTerrain* pTerrain = nullptr;

			switch (eTerrainType)
			{
			case ENGINE::TERRAIN_CUBE:
			{
				pTerrain = CToolTerrainCube::Create(ENGINE::GetGraphicDev()->GetDevice());
				pTerrain->SetTexName(szName);
				pTerrain->ChangeTex();
				break;
			}
			case ENGINE::TERRAIN_WALL:
			{
				pTerrain = CToolTerrainWallCube::Create(ENGINE::GetGraphicDev()->GetDevice());
				break;
			}
			case ENGINE::TERRAIN_RECT:
			{
				pTerrain = CToolTerrainRect::Create(ENGINE::GetGraphicDev()->GetDevice());
				pTerrain->SetTexName(szName);
				pTerrain->ChangeTex();
				break;
			}
			}

			ENGINE::CTransform* pTransform = dynamic_cast<ENGINE::CTransform*>(pTerrain->Get_Component(L"Transform"));
			pTransform->SetPos(vPos);
			pTransform->SetSize(vSize);
			pTransform->SetAngle(vAngle.x, ENGINE::ANGLE_X);
			pTransform->SetAngle(vAngle.y, ENGINE::ANGLE_Y);
			pTransform->SetAngle(vAngle.z, ENGINE::ANGLE_Z);

			pTerrain->SetTexName(szName);

			pView->AddCubeForLoad(pTerrain);
		}

		CloseHandle(hFile);
		MessageBox(_T("Load Success."), _T("Load"), MB_OK);
	}

	UpdateData(FALSE);
}


void CMyFormView::OnBnClickedButton_PathUpdate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CPathExtract* pPath = new CPathExtract;
	pPath->ExportFile();
	pPath->Release();
	ENGINE::Safe_Delete(pPath);
}
