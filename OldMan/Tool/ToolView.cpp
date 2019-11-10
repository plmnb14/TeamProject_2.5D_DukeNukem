
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

#include "TerrainCube.h"
#include "TerrainWallCube.h"
#include "TerrainRect.h"
#include "Trasform.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 전역변수
HWND g_hWnd;

// CToolView

IMPLEMENT_DYNCREATE(CToolView, CView)

BEGIN_MESSAGE_MAP(CToolView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

// CToolView 생성/소멸

CToolView::CToolView()
	:m_pDeviceMgr(ENGINE::CGraphicDev::GetInstance()),
	m_pResourceMgr(ENGINE::CResourceMgr::GetInstance()),
	m_pSelectCube(nullptr)
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CToolView::~CToolView()
{
}

BOOL CToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CToolView 그리기

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

	CView::Invalidate(FALSE); // 화면 갱신
}


void CToolView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnLButtonDown(nFlags, point);

	// 임시
	if (m_pSelectCube)
	{
		CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
		NULL_CHECK(pMainFrm);

		CMyFormView* pFormView = dynamic_cast<CMyFormView*>(pMainFrm->m_MainSplitter.GetPane(0, 0));
		NULL_CHECK(pFormView);

		switch (pFormView->m_eTerrainType)
		{
		case CMyFormView::TERRAIN_CUBE:
		{
			dynamic_cast<CTerrainCube*>(m_pSelectCube)->SetClicked();
			break;
		}
		case CMyFormView::TERRAIN_WALL:
		{
			dynamic_cast<CTerrainWallCube*>(m_pSelectCube)->SetClicked();
			break;
		}
		case CMyFormView::TERRAIN_RECT:
		{
			dynamic_cast<CTerrainRect*>(m_pSelectCube)->SetClicked();
			break;
		}
		}

		m_pCubeList.push_back(m_pSelectCube);

		m_pSelectCube = CTerrainCube::Create(m_pDeviceMgr->GetDevice());
		m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::PROPS, m_pSelectCube);
	}
}


void CToolView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnRButtonDown(nFlags, point);

	if (m_pSelectCube)
	{
		m_pSelectCube = nullptr;

		//delete Object in Layer
		//m_mapLayer[ENGINE::CLayer::OBJECT]
	}
	else
	{
		m_pSelectCube = CTerrainCube::Create(m_pDeviceMgr->GetDevice());
		m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::PROPS, m_pSelectCube);
	}
}


void CToolView::SelectObjAfter()
{
	if (!m_pSelectCube)
	{
		CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
		NULL_CHECK(pMainFrm);

		CMyFormView* pFormView = dynamic_cast<CMyFormView*>(pMainFrm->m_MainSplitter.GetPane(0, 0));
		NULL_CHECK(pFormView);

		switch (pFormView->m_eTerrainType)
		{
		case CMyFormView::TERRAIN_CUBE:
		{
			m_pSelectCube = CTerrainCube::Create(m_pDeviceMgr->GetDevice());
			m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::PROPS, m_pSelectCube);
			break;
		}
		case CMyFormView::TERRAIN_WALL:
		{
			m_pSelectCube = CTerrainWallCube::Create(m_pDeviceMgr->GetDevice());
			m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::PROPS, m_pSelectCube);
			break;
		}
		case CMyFormView::TERRAIN_RECT:
		{
			m_pSelectCube = CTerrainRect::Create(m_pDeviceMgr->GetDevice());
			m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::PROPS, m_pSelectCube);
			break;
		}
		}
	}
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

	// Player Texture
	hr = m_pResourceMgr->AddTexture(
		m_pDeviceMgr->GetDevice(),
		ENGINE::RESOURCE_STATIC,
		ENGINE::TEX_NORMAL,
		L"Texture_Player",
		L"../Client/Texture/Terrain/Terrain%d.png", 1);
	FAILED_CHECK_MSG_RETURN(hr, L"Texture_Player Add Failed", E_FAIL);

	//// Terrain Texture
	//hr = m_pResourceMgr->AddTexture(
	//	m_pDeviceMgr->GetDevice(),
	//	ENGINE::RESOURCE_DYNAMIC,
	//	ENGINE::TEX_NORMAL,
	//	L"Texture_Terrain",
	//	L"../Texture/Terrain/Terrain%d.png", 1);
	//FAILED_CHECK_MSG_RETURN(hr, L"Texture_Terrain Add Failed", E_FAIL);

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

