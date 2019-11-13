
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

#include "ToolTerrainCube.h"
#include "ToolTerrainWallCube.h"
#include "ToolTerrainRect.h"
#include "Trasform.h"

#include "PathExtract.h"
//#include "FileInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ��������
HWND g_hWnd;

// CToolView11

IMPLEMENT_DYNCREATE(CToolView, CView)

BEGIN_MESSAGE_MAP(CToolView, CView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CToolView ����/�Ҹ�

CToolView::CToolView()
	:m_pDeviceMgr(ENGINE::CGraphicDev::GetInstance()),
	m_pResourceMgr(ENGINE::CResourceMgr::GetInstance()),
	m_pSelectCube(nullptr), m_pPathExtractor(nullptr)
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CToolView::~CToolView()
{
	ENGINE::Safe_Delete(m_pPathExtractor);
}

BOOL CToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CToolView �׸���
//v f

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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

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

void CToolView::LoadTexture()
{
	m_pPathExtractor = new CPathExtract;
	m_pPathExtractor->MakePathFile();

	HRESULT hr;
	//// �ӽ�.
	//// �׻� MakePathFile�ϱ� ������ �ε�����.
	//// TextureMgr�� txt ���Ͽ��� �о�� Map�� ����ϸ� �������� ������� ������.
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

	// Single�� FileName
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
	// �ӽ� (Ray�� ���� ������� üũ�ϴ� ������ �����ϱ�)
	if (::GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		D3DXVECTOR3 vMouse = { float(_fPointX), float(_fPointY), 0.f };

		m_fDragX += vMouse.x - m_vBeforeMousePos.x;
		m_fDragY += vMouse.y - m_vBeforeMousePos.y;
		m_vBeforeMousePos = vMouse;

		// Ÿ�� �����ŭ ���������� üũ
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

	CView::Invalidate(FALSE); // ȭ�� ����
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

	// �ӽ� (3D ��ŷ ��� �� ����� �����ϱ�)
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

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
