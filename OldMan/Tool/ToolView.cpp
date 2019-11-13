
// ToolView.cpp : CToolView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Tool.h"
#endif

#include "ToolDoc.h"
#include "ToolView.h"
#include "MainFrm.h"
#include "MyFormView.h"

#include "ToolTerrainCube.h"
#include "ToolTerrainWallCube.h"
#include "ToolTerrainRect.h"
#include "Trasform.h"

#include "PathExtract.h"
//#include "FileInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 전역변수
HWND g_hWnd;

// CToolView11

IMPLEMENT_DYNCREATE(CToolView, CView)

BEGIN_MESSAGE_MAP(CToolView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CToolView 생성/소멸

CToolView::CToolView()
	:m_pDeviceMgr(ENGINE::CGraphicDev::GetInstance()),
	m_pResourceMgr(ENGINE::CResourceMgr::GetInstance()),
	m_pSelectCube(nullptr), m_pPathExtractor(nullptr)
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CToolView::~CToolView()
{
	ENGINE::Safe_Delete(m_pPathExtractor);
}

BOOL CToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CToolView 그리기
//v f

void CToolView::OnDraw(CDC* /*pDC*/)
{
	CToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
	m_pDeviceMgr->Render_Begin();

	MAP_LAYER::iterator iter_begin = m_mapLayer.begin();
	MAP_LAYER::iterator iter_end = m_mapLayer.end();

	for (; iter_begin != iter_end; ++iter_begin)
		iter_begin->second->Render();

	m_pDeviceMgr->Render_End();
}


// CToolView 인쇄

BOOL CToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CToolView 진단

#ifdef _DEBUG
void CToolView::AssertValid() const
{
	CView::AssertValid();
}

void CToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CToolDoc* CToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CToolDoc)));
	return (CToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CToolView 메시지 처리기


void CToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// View의 크기 맞추기.
	// 먼저 MainFrame의 크기를 얻어온다.
	// AfxGetApp: WinApp의 포인터를 얻어오는 MFC의 전역함수.
	// CWinApp::GetMainWnd: MainFrame의 포인터를 얻어오는 함수.
	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	NULL_CHECK(pMainFrm);

	// GetWindowRect: 윈도우의 모든 테두리를 포함한 사각형 크기를 얻어온다.
	// 이 때 좌표는 스크린 좌표 기준이다.
	RECT rcMain = {};
	pMainFrm->GetWindowRect(&rcMain); // MainFrame의 크기를 얻어옴.

									  // 이렇게 얻어온 Main의 left, top을 0, 0기준으로 셋팅.
	::SetRect(&rcMain, 0, 0, rcMain.right - rcMain.left, rcMain.bottom - rcMain.top);

	// GetClientRect: 윈도우의 테두리를 제외한 순수 클라이언트 영역의 사각형 크기를 얻어온다.
	// 이 때 좌표는 클라이언트 좌표 기준이다. 이 때 left, top은 무조건 0, 0부터 시작.
	RECT rcView = {};
	GetClientRect(&rcView);

	// MainFrm과 View의 간격을 구한다.
	int iGapX = rcMain.right - rcView.right;
	int iGapY = rcMain.bottom - rcView.bottom;

	// MainFrame의 크기 조정.
	pMainFrm->SetWindowPos(
		nullptr, 0, 0, WINCX + iGapX, WINCY + iGapY, SWP_NOZORDER);

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	g_hWnd = m_hWnd;

	HRESULT hr = 0;

	hr = m_pDeviceMgr->InitDevice(g_hWnd, WINCX, WINCY, ENGINE::CGraphicDev::MODE_WIN);
	FAILED_CHECK_MSG(hr, L"InitDevice Failed");

	Initialize();
}


void CToolView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnMouseMove(nFlags, point);

	MAP_LAYER::iterator iter_begin = m_mapLayer.begin();
	MAP_LAYER::iterator iter_end = m_mapLayer.end();

	for (; iter_begin != iter_end; ++iter_begin)
		iter_begin->second->Update();

	iter_begin = m_mapLayer.begin();
	iter_end = m_mapLayer.end();

	for (; iter_begin != iter_end; ++iter_begin)
		iter_begin->second->LateUpdate();

	auto& iter_begin_Cube = m_pCubeList.begin();
	auto& iter_end_Cube = m_pCubeList.end();
	for (; iter_begin_Cube != iter_end_Cube; )
	{
		if ((*iter_begin_Cube) == nullptr ||(*iter_begin_Cube)->GetDead() == DEAD_OBJ)
		{
			iter_begin_Cube = m_pCubeList.erase(iter_begin_Cube);
		}
		else
			iter_begin_Cube++;
	}

	DragPicking(point.x, point.y);
	if(!CheckGrid())
		CubeMoveToMouse();

}


void CToolView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnLButtonDown(nFlags, point);

	if (m_pSelectCube)
	{
		m_pSelectCube->SetClicked();
		CreateCube(false);
	}

	m_vBeforeMousePos = D3DXVECTOR3(point.x, point.y, 0.f);
	m_fDragX = 0.f;
	m_fDragY = 0.f;
}


void CToolView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnRButtonDown(nFlags, point);

	if (m_pSelectCube)
	{
		m_pSelectCube->SetDead();
		m_pSelectCube = nullptr;
	}
	else
	{
		CreateCube(false);
	}
}


void CToolView::SelectObjAfter()
{
	CreateCube(true);
}

void CToolView::ChangeValueAfter()
{
	if (m_pSelectCube)
	{
		CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
		NULL_CHECK(pMainFrm);

		CMyFormView* pFormView = dynamic_cast<CMyFormView*>(pMainFrm->m_MainSplitter.GetPane(0, 0));
		NULL_CHECK(pFormView);

		ENGINE::CTransform* pTransform = dynamic_cast<ENGINE::CTransform*>(m_pSelectCube->Get_Component(L"Transform"));
		pTransform->SetPos(pFormView->GetPositionVec());
		pTransform->SetAngle(pFormView->GetRotationVec().x, ENGINE::ANGLE_X);
		pTransform->SetAngle(pFormView->GetRotationVec().y, ENGINE::ANGLE_Y);
		pTransform->SetAngle(pFormView->GetRotationVec().z, ENGINE::ANGLE_Z);
		pTransform->SetSize(pFormView->GetScaleVec());
	}
}

void CToolView::PipeLineSetup()
{
	// 조명 off
	m_pDeviceMgr->GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);

	// 후면 추려내기 off
	m_pDeviceMgr->GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// WireFrame
	//m_pDeviceMgr->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	D3DXMATRIX matView, matProj; // 뷰행렬, 투영행렬

								 // 뷰행렬(카메라의 역행렬) 생성하는 함수
	D3DXMatrixLookAtLH(
		&matView, /* 반환 값*/
		&D3DXVECTOR3(0.f, 5.f, -20.f), /* Eye (카메라 위치) */
		&D3DXVECTOR3(0.f, 0.f, 0.f), /* At (바라볼 위치) */
		&D3DXVECTOR3(0.f, 1.f, 0.f) /* Up (업벡터) */);

	// 장치에게 뷰행렬 전달.
	m_pDeviceMgr->GetDevice()->SetTransform(D3DTS_VIEW, &matView);

	// 원근투영 행렬 생성하는 함수
	D3DXMatrixPerspectiveFovLH(
		&matProj, /* 반환 값 */
		D3DXToRadian(45.f), /* FOV (Field Of View, 시야각) */
		WINCX / (float)WINCY, /* Aspect (종횡비) */
		1.f, /* Near (근평면) */
		1000.f /*Far (원평면) */);

	// 장치에게 투영 행렬 전달.
	m_pDeviceMgr->GetDevice()->SetTransform(D3DTS_PROJECTION, &matProj);
}

void CToolView::LoadTexture()
{
	m_pPathExtractor = new CPathExtract;
	m_pPathExtractor->MakePathFile();

	HRESULT hr;
	//// 임시.
	//// 항상 MakePathFile하기 때문에 로딩느림.
	//// TextureMgr가 txt 파일에서 읽어온 Map을 사용하면 좋겠지만 구현방법 생각중.
	//HRESULT hr = ENGINE::GetTextureMgr()->LoadTextureFromImgPath(L"../Data/TexturePath.txt");
	//FAILED_CHECK_MSG(hr, L"LoadTextureFromImgPath Failed");

	for (auto& iter : m_pPathExtractor->m_PathInfoLst_Multi)
	{
		hr = m_pResourceMgr->AddTexture(
			m_pDeviceMgr->GetDevice(),
			ENGINE::RESOURCE_DYNAMIC,
			ENGINE::TEX_NORMAL,
			iter->wstrStateKey,
			iter->wstrImgPath, iter->iImgCount);
		FAILED_CHECK_MSG(hr, iter->wstrFileName.c_str());
	}

	// Single은 FileName
	for (auto& iter : m_pPathExtractor->m_PathInfoLst_Single)
	{
		hr = m_pResourceMgr->AddTexture(
			m_pDeviceMgr->GetDevice(),
			ENGINE::RESOURCE_DYNAMIC,
			ENGINE::TEX_NORMAL,
			iter->wstrFileName,
			iter->wstrImgPath, 1);
		FAILED_CHECK_MSG(hr, iter->wstrFileName.c_str());
	}
}

HRESULT CToolView::Initialize()
{
	PipeLineSetup();

	// Player Buffer

	// Terrain Buffer
	HRESULT hr = m_pResourceMgr->AddBuffer(
		m_pDeviceMgr->GetDevice(),
		ENGINE::RESOURCE_STATIC,
		ENGINE::CVIBuffer::BUFFER_RCTEX,
		L"Buffer_RcTex");
	FAILED_CHECK_MSG_RETURN(hr, L"Buffer_RcTex Add Failed", E_FAIL);

	int a = 0;

	hr = m_pResourceMgr->AddBuffer(
		m_pDeviceMgr->GetDevice(),
		ENGINE::RESOURCE_STATIC,
		ENGINE::CVIBuffer::BUFFER_CUBECOL,
		L"Buffer_CubeCol");
	FAILED_CHECK_MSG_RETURN(hr, L"Buffer_CubeCol Add Failed", E_FAIL);

	hr = m_pResourceMgr->AddBuffer(
		m_pDeviceMgr->GetDevice(),
		ENGINE::RESOURCE_STATIC,
		ENGINE::CVIBuffer::BUFFER_WALLCUBECOL,
		L"Buffer_WallCubeCol");
	FAILED_CHECK_MSG_RETURN(hr, L"Buffer_WallCubeCol Add Failed", E_FAIL);

	LoadTexture();

	// Environment Layer
	hr = Add_Environment_Layer();
	FAILED_CHECK_RETURN(hr, E_FAIL);

	// Object Layer
	hr = Add_Object_Layer();
	FAILED_CHECK_RETURN(hr, E_FAIL);

	//// UI Layer
	//hr = Add_UI_Layer();
	//FAILED_CHECK_RETURN(hr, E_FAIL);

	return S_OK;
}

HRESULT CToolView::Add_Environment_Layer()
{
	return S_OK;
}

HRESULT CToolView::Add_Object_Layer()
{
	// Object Layer
	ENGINE::CLayer* pObject_Layer = ENGINE::CLayer::Create(m_pDeviceMgr->GetDevice());
	NULL_CHECK_MSG_RETURN(pObject_Layer, L"Object Layer Create Failed", E_FAIL);
	m_mapLayer.insert({ ENGINE::CLayer::OBJECT, pObject_Layer });

	//// Terrain
	//pObject = CTerrain::Create(m_pGraphicDev);
	//NULL_CHECK_MSG_RETURN(pObject, L"Terrain Create Failed", E_FAIL);
	//pObject_Layer->AddObject(ENGINE::OBJECT_TYPE::PROPS, pObject);

	//// Player
	//ENGINE::CGameObject* pObject = CPlayer::Create(m_pDeviceMgr->GetDevice());
	//NULL_CHECK_MSG_RETURN(pObject, L"Player Create Failed", E_FAIL);
	//pObject_Layer->AddObject(ENGINE::OBJECT_TYPE::PLAYER, pObject);

	// Terrain
	//ENGINE::CGameObject* pObject = CTerrainCube::Create(m_pDeviceMgr->GetDevice());
	//NULL_CHECK_MSG_RETURN(pObject, L"CTerrainCube Create Failed", E_FAIL);
	//pObject_Layer->AddObject(ENGINE::OBJECT_TYPE::PROPS, pObject);

	//// Camera
	//pObject = CCamera::Create(m_pDeviceMgr->GetDevice(), pObject_Layer->Get_Player());
	//NULL_CHECK_MSG_RETURN(pObject, L"Terrain Create Failed", E_FAIL);
	//pObject_Layer->AddObject(ENGINE::OBJECT_TYPE::PROPS, pObject);

	return S_OK;
}

void CToolView::ChangeTerrainType()
{
	CreateCube(true);
}

void CToolView::AddCubeForLoad(CToolTerrain* _pTerrain)
{
	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	NULL_CHECK(pMainFrm);

	CMyFormView* pFormView = dynamic_cast<CMyFormView*>(pMainFrm->m_MainSplitter.GetPane(0, 0));
	NULL_CHECK(pFormView);

	_pTerrain->SetClicked();
	m_pCubeList.push_back(_pTerrain);
	m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::PROPS, _pTerrain);
}

void CToolView::CubeMoveToMouse()
{
	if (m_pSelectCube)
	{
		CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
		NULL_CHECK(pMainFrm);

		CMyFormView* pFormView = dynamic_cast<CMyFormView*>(pMainFrm->m_MainSplitter.GetPane(0, 0));
		NULL_CHECK(pFormView);

		ENGINE::CTransform* pTransform = dynamic_cast<ENGINE::CTransform*>(m_pSelectCube->Get_Component(L"Transform"));
		pFormView->UpdateTransformStr(
			pTransform->GetPos(),
			D3DXVECTOR3(pTransform->GetAngle(ENGINE::ANGLE_X), pTransform->GetAngle(ENGINE::ANGLE_Y), pTransform->GetAngle(ENGINE::ANGLE_Z)),
			pTransform->GetSize()
		);
	}
}

void CToolView::CreateCube(bool _bIsChange)
{
	if (_bIsChange && m_pSelectCube)
	{
		m_pSelectCube->SetDead();
		m_pSelectCube = nullptr;
	}

	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	NULL_CHECK(pMainFrm);

	CMyFormView* pFormView = dynamic_cast<CMyFormView*>(pMainFrm->m_MainSplitter.GetPane(0, 0));
	NULL_CHECK(pFormView);

	switch (pFormView->m_eTerrainType)
	{
	case ENGINE::TERRAIN_CUBE:
	{
		m_pCubeList.push_back(m_pSelectCube);
		m_pSelectCube = CToolTerrainCube::Create(m_pDeviceMgr->GetDevice());
		break;
	}
	case ENGINE::TERRAIN_WALL:
	{
		m_pCubeList.push_back(m_pSelectCube);
		m_pSelectCube = CToolTerrainWallCube::Create(m_pDeviceMgr->GetDevice());
		break;
	}
	case ENGINE::TERRAIN_RECT:
	{
		m_pCubeList.push_back(m_pSelectCube);
		m_pSelectCube = CToolTerrainRect::Create(m_pDeviceMgr->GetDevice());
		break;
	}
	}

	m_pSelectCube->SetTexName(pFormView->m_wstrFileName);
	m_pSelectCube->ChangeTex();
	m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::PROPS, m_pSelectCube);

	pFormView->EditDataExchange();
	pFormView->InitData();
}

void CToolView::DragPicking(float _fPointX, float _fPointY)
{
	// Drag Picking
	// 임시 (Ray로 옆면 벗어났는지 체크하는 것으로 수정하기)
	if (::GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		D3DXVECTOR3 vMouse = { float(_fPointX), float(_fPointY), 0.f };

		m_fDragX += vMouse.x - m_vBeforeMousePos.x;
		m_fDragY += vMouse.y - m_vBeforeMousePos.y;
		m_vBeforeMousePos = vMouse;

		// 타일 사이즈만큼 움직였는지 체크
		ENGINE::CTransform* pTransform = dynamic_cast<ENGINE::CTransform*>(m_pSelectCube->Get_Component(L"Transform"));
		float fTestMul = 100.f;
		float fSizeX = pTransform->GetSize().x * 2.f * fTestMul;
		float fSizeY = pTransform->GetSize().y * 2.f * fTestMul;

		if (abs(m_fDragX) >= fSizeX)
		{
			m_fDragX = 0.f;
			m_vBeforeMousePos = vMouse;

			m_pSelectCube->SetClicked();
			CreateCube(false);
		}
		else if(abs(m_fDragY) >= fSizeY)
		{
			m_fDragY = 0.f;
			m_vBeforeMousePos = vMouse;

			m_pSelectCube->SetClicked();
			CreateCube(false);
		}
	}

	CView::Invalidate(FALSE); // 화면 갱신
}

bool CToolView::CheckGrid()
{
	if (!m_pSelectCube)
		return false;

	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	NULL_CHECK(pMainFrm);

	CMyFormView* pFormView = dynamic_cast<CMyFormView*>(pMainFrm->m_MainSplitter.GetPane(0, 0));
	NULL_CHECK(pFormView);

	if (!(pFormView->m_CheckButton_Grid.GetCheck()))
	{
		m_pSelectCube->SetFitGrid(false);
		return false;
	}

	// 임시 (3D 픽킹 배운 뒤 제대로 수정하기)
	for (auto& iter : m_pCubeList)
	{
		D3DXVECTOR3 vVtxPos;
		if (iter->CheckGrid(vVtxPos))
		{
			ENGINE::CTransform* pTransform = dynamic_cast<ENGINE::CTransform*>(iter->Get_Component(L"Transform"));
			pFormView->UpdateTransformStr(
				vVtxPos,
				D3DXVECTOR3(pTransform->GetAngle(ENGINE::ANGLE_X), pTransform->GetAngle(ENGINE::ANGLE_Y), pTransform->GetAngle(ENGINE::ANGLE_Z)),
				pTransform->GetSize()
			);

			m_pSelectCube->SetFitGrid(true);
			pFormView->EditDataExchange();
			return true;
		}
	}

	m_pSelectCube->SetFitGrid(false);
	return false;
}



void CToolView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnKeyDown(nChar, nRepCnt, nFlags);


	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	NULL_CHECK(pMainFrm);

	CMyFormView* pFormView = dynamic_cast<CMyFormView*>(pMainFrm->m_MainSplitter.GetPane(0, 0));
	NULL_CHECK(pFormView);

	if (GetAsyncKeyState('W') & 0x8000)
	{
		ENGINE::CTransform* pTransform = dynamic_cast<ENGINE::CTransform*>(m_pSelectCube->Get_Component(L"Transform"));
		pTransform->SetPos(
			D3DXVECTOR3(
			pTransform->GetPos().x,
			pTransform->GetPos().y + (2.f * pTransform->GetSize().y),
			pTransform->GetPos().z));

		pFormView->UpdateTransformStr(
			pTransform->GetPos(),
			D3DXVECTOR3(pTransform->GetAngle(ENGINE::ANGLE_X), pTransform->GetAngle(ENGINE::ANGLE_Y), pTransform->GetAngle(ENGINE::ANGLE_Z)),
			pTransform->GetSize()
		);
		pFormView->EditDataExchange();
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		ENGINE::CTransform* pTransform = dynamic_cast<ENGINE::CTransform*>(m_pSelectCube->Get_Component(L"Transform"));
		pTransform->SetPos(
			D3DXVECTOR3(
				pTransform->GetPos().x,
				pTransform->GetPos().y - (2.f * pTransform->GetSize().y),
				pTransform->GetPos().z));

		pFormView->UpdateTransformStr(
			pTransform->GetPos(),
			D3DXVECTOR3(pTransform->GetAngle(ENGINE::ANGLE_X), pTransform->GetAngle(ENGINE::ANGLE_Y), pTransform->GetAngle(ENGINE::ANGLE_Z)),
			pTransform->GetSize()
		);
		pFormView->EditDataExchange();
	}
}
