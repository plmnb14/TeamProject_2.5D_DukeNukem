
// ToolView.cpp : CToolView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
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

// ��������
HWND g_hWnd;

// CToolView

IMPLEMENT_DYNCREATE(CToolView, CView)

BEGIN_MESSAGE_MAP(CToolView, CView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

// CToolView ����/�Ҹ�

CToolView::CToolView()
	:m_pDeviceMgr(ENGINE::CGraphicDev::GetInstance()),
	m_pResourceMgr(ENGINE::CResourceMgr::GetInstance()),
	m_pSelectCube(nullptr)
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CToolView::~CToolView()
{
}

BOOL CToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CToolView �׸���

void CToolView::OnDraw(CDC* /*pDC*/)
{
	CToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
	m_pDeviceMgr->Render_Begin();

	MAP_LAYER::iterator iter_begin = m_mapLayer.begin();
	MAP_LAYER::iterator iter_end = m_mapLayer.end();

	for (; iter_begin != iter_end; ++iter_begin)
		iter_begin->second->Render();

	m_pDeviceMgr->Render_End();
}


// CToolView �μ�

BOOL CToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CToolView ����

#ifdef _DEBUG
void CToolView::AssertValid() const
{
	CView::AssertValid();
}

void CToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CToolDoc* CToolView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CToolDoc)));
	return (CToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CToolView �޽��� ó����


void CToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// View�� ũ�� ���߱�.
	// ���� MainFrame�� ũ�⸦ ���´�.
	// AfxGetApp: WinApp�� �����͸� ������ MFC�� �����Լ�.
	// CWinApp::GetMainWnd: MainFrame�� �����͸� ������ �Լ�.
	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	NULL_CHECK(pMainFrm);

	// GetWindowRect: �������� ��� �׵θ��� ������ �簢�� ũ�⸦ ���´�.
	// �� �� ��ǥ�� ��ũ�� ��ǥ �����̴�.
	RECT rcMain = {};
	pMainFrm->GetWindowRect(&rcMain); // MainFrame�� ũ�⸦ ����.

									  // �̷��� ���� Main�� left, top�� 0, 0�������� ����.
	::SetRect(&rcMain, 0, 0, rcMain.right - rcMain.left, rcMain.bottom - rcMain.top);

	// GetClientRect: �������� �׵θ��� ������ ���� Ŭ���̾�Ʈ ������ �簢�� ũ�⸦ ���´�.
	// �� �� ��ǥ�� Ŭ���̾�Ʈ ��ǥ �����̴�. �� �� left, top�� ������ 0, 0���� ����.
	RECT rcView = {};
	GetClientRect(&rcView);

	// MainFrm�� View�� ������ ���Ѵ�.
	int iGapX = rcMain.right - rcView.right;
	int iGapY = rcMain.bottom - rcView.bottom;

	// MainFrame�� ũ�� ����.
	pMainFrm->SetWindowPos(
		nullptr, 0, 0, WINCX + iGapX, WINCY + iGapY, SWP_NOZORDER);

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	g_hWnd = m_hWnd;

	HRESULT hr = 0;

	hr = m_pDeviceMgr->InitDevice(g_hWnd, WINCX, WINCY, ENGINE::CGraphicDev::MODE_WIN);
	FAILED_CHECK_MSG(hr, L"InitDevice Failed");

	Initialize();
}


void CToolView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

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

	CView::Invalidate(FALSE); // ȭ�� ����
}


void CToolView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CView::OnLButtonDown(nFlags, point);

	// �ӽ�
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

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
	// ���� off
	m_pDeviceMgr->GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);

	// �ĸ� �߷����� off
	m_pDeviceMgr->GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// WireFrame
	//m_pDeviceMgr->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	D3DXMATRIX matView, matProj; // �����, �������

								 // �����(ī�޶��� �����) �����ϴ� �Լ�
	D3DXMatrixLookAtLH(
		&matView, /* ��ȯ ��*/
		&D3DXVECTOR3(0.f, 5.f, -20.f), /* Eye (ī�޶� ��ġ) */
		&D3DXVECTOR3(0.f, 0.f, 0.f), /* At (�ٶ� ��ġ) */
		&D3DXVECTOR3(0.f, 1.f, 0.f) /* Up (������) */);

	// ��ġ���� ����� ����.
	m_pDeviceMgr->GetDevice()->SetTransform(D3DTS_VIEW, &matView);

	// �������� ��� �����ϴ� �Լ�
	D3DXMatrixPerspectiveFovLH(
		&matProj, /* ��ȯ �� */
		D3DXToRadian(45.f), /* FOV (Field Of View, �þ߰�) */
		WINCX / (float)WINCY, /* Aspect (��Ⱦ��) */
		1.f, /* Near (�����) */
		1000.f /*Far (�����) */);

	// ��ġ���� ���� ��� ����.
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

