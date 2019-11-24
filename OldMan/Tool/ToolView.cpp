
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

#include "ToolTerrain.h"
#include "ToolTerrainCube.h"
#include "ToolTerrainWallCube.h"
#include "ToolTerrainRect.h"
#include "Trasform.h"
#include "Camera.h"


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
	ON_WM_LBUTTONUP()
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
	for_each(m_mapLayer.begin(), m_mapLayer.end(),
		[](auto& MyPair)
	{
		Safe_Delete(MyPair.second);
	});

	m_mapLayer.clear();

	ENGINE::GetTextureMgr()->DestroyInstance();
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

	DragPicking((float)point.x, (float)point.y);

}


void CToolView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CView::OnLButtonDown(nFlags, point);

	GetCursorPos(&m_ptFitCursorPos);

	if (m_pSelectCube)
	{
		m_pSelectCube->SetClicked();
		CreateCube(false);
	}

	if (!m_bIsMousePressing && (GetAsyncKeyState(VK_MENU) & 0x8000)) //ALT
	{
		// Delete Data
		auto& iter_DataBegin = m_pCubeList.begin();
		auto& iter_DataEnd = m_pCubeList.end();
		for (; iter_DataBegin != iter_DataEnd;)
		{
			if ((*iter_DataBegin)->GetPicked())
			{
				iter_DataBegin = m_pCubeList.erase(iter_DataBegin);
			}
			else
				iter_DataBegin++;
		}

		// Delete Obj in MapLayer
		list<ENGINE::CGameObject*> pList = m_mapLayer[ENGINE::CLayer::OBJECT]->Get_List(ENGINE::OBJECT_TYPE::PROPS);
		list<ENGINE::CGameObject*>::iterator iter_Begin = pList.begin();
		list<ENGINE::CGameObject*>::iterator iter_End = pList.end();
		for (;iter_Begin != iter_End;)
		{
			CToolTerrain* pTerrain = dynamic_cast<CToolTerrain*>((*iter_Begin));
			if (pTerrain->GetPicked())
			{
				pTerrain->SetDead();
				iter_Begin = pList.erase(iter_Begin);
			}
			else
				iter_Begin++;
		}
	}

	m_vLastPickedCubePos = D3DXVECTOR3((float)point.x, (float)point.y, 0.f);
	m_bIsMousePressing = true;
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
	cout << "- Make PathInfo from text File" << endl;
	HRESULT hr = ENGINE::GetTextureMgr()->LoadTextureFromImgPath(L"../Data/TexturePath.txt");
	FAILED_CHECK_MSG(hr, L"LoadTextureFromImgPath Failed");

	cout << "Add Texture" << endl;
	cout << "- Add Multi Texture" << endl;
	for (auto& iter : ENGINE::GetTextureMgr()->GetMapTexture_Multi())
	{
		hr = m_pResourceMgr->AddTexture(
			m_pDeviceMgr->GetDevice(),
			ENGINE::RESOURCE_DYNAMIC,
			ENGINE::TEX_NORMAL,
			iter->wstrStateKey,
			iter->wstrImgPath, iter->iImgCount);
		FAILED_CHECK_MSG(hr, iter->wstrFileName.c_str());
	}

	cout << "- Add Single Texture" << endl;
	// Single�� FileName
	for (auto& iter : ENGINE::GetTextureMgr()->GetMapTexture_Single())
	{
		CString strCheckDDS = iter->wstrFileName.c_str();
		if (strCheckDDS.Find(L".dds") > 0)
		{
			hr = m_pResourceMgr->AddTexture(
				m_pDeviceMgr->GetDevice(),
				ENGINE::RESOURCE_DYNAMIC,
				ENGINE::TEX_CUBE,
				iter->wstrFileName,
				iter->wstrImgPath, 1);
			FAILED_CHECK_MSG(hr, iter->wstrFileName.c_str());
		}
		else
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
}

HRESULT CToolView::Initialize()
{
	cout << "PipeLine Setup" << endl;
	PipeLineSetup();

	cout << "Add Buffer" << endl;

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

	hr = m_pResourceMgr->AddBuffer(
		m_pDeviceMgr->GetDevice(),
		ENGINE::RESOURCE_STATIC,
		ENGINE::CVIBuffer::BUFFER_CUBETEX,
		L"Buffer_CubeTex");
	FAILED_CHECK_MSG_RETURN(hr, L"Buffer_CubeTex Add Failed", E_FAIL);

	cout << "Load Texture" << endl;
	LoadTexture();

	cout << "Add Layer" << endl;
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

	// Camera
	ENGINE::CGameObject* pObject = CCamera::Create(m_pDeviceMgr->GetDevice(), nullptr);
	NULL_CHECK_MSG_RETURN(pObject, L"Camera Create Failed", E_FAIL);
	pObject_Layer->AddObject(ENGINE::OBJECT_TYPE::CAMERA, pObject);
	//pObject_Layer->Get_Player()->Set_MainCamera(pObject_Layer->Get_MainCamera());

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

void CToolView::Update()
{
	// Form View Update
	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	NULL_CHECK(pMainFrm);

	CMyFormView* pFormView = dynamic_cast<CMyFormView*>(pMainFrm->m_MainSplitter.GetPane(0, 0));
	NULL_CHECK(pFormView);

	pFormView->Update();

	//// Update =========================================================================
	MAP_LAYER::iterator iter_begin = m_mapLayer.begin();
	MAP_LAYER::iterator iter_end = m_mapLayer.end();

	for (; iter_begin != iter_end; ++iter_begin)
		iter_begin->second->Update();

	//if (!CheckGrid())
	CubeMoveToMouse();

	CCamera* pCamera = dynamic_cast<CCamera*>(m_mapLayer[ENGINE::CLayer::OBJECT]->Get_List(ENGINE::CAMERA).front());
	m_ViewMatrix = pCamera->GetViewMatrix();
	m_ProjMatrix = pCamera->GetProjMatrix();

	//// Update End =======================================================================

	//// Late Update ======================================================================

	iter_begin = m_mapLayer.begin();
	iter_end = m_mapLayer.end();

	for (; iter_begin != iter_end; ++iter_begin)
		iter_begin->second->LateUpdate();
	//// Late Update End===================================================================


	//// Check Cube List
	auto& iter_begin_Cube = m_pCubeList.begin();
	auto& iter_end_Cube = m_pCubeList.end();
	for (; iter_begin_Cube != iter_end_Cube; )
	{
		if ((*iter_begin_Cube) == nullptr || (*iter_begin_Cube)->GetDead() == DEAD_OBJ)
		{
			iter_begin_Cube = m_pCubeList.erase(iter_begin_Cube);
		}
		else
			iter_begin_Cube++;
	}

	m_pDeviceMgr->Render_Begin();

	iter_begin = m_mapLayer.begin();
	iter_end = m_mapLayer.end();

	for (; iter_begin != iter_end; ++iter_begin)
		iter_begin->second->Render();

	m_pDeviceMgr->Render_End();
}

void CToolView::CubeMoveToMouse()
{
	if (!m_pSelectCube)
		return;

	if ((GetAsyncKeyState('1') & 0x8000))
	{
		POINT ptNowCursor;
		GetCursorPos(&ptNowCursor);
		SetCursorPos(m_ptFitCursorPos.x, ptNowCursor.y);
	}

	if ((GetAsyncKeyState('2') & 0x8000))
	{
		POINT ptNowCursor;
		GetCursorPos(&ptNowCursor);
		SetCursorPos(ptNowCursor.x, m_ptFitCursorPos.y);
	}

	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	NULL_CHECK(pMainFrm);

	CMyFormView* pFormView = dynamic_cast<CMyFormView*>(pMainFrm->m_MainSplitter.GetPane(0, 0));
	NULL_CHECK(pFormView);


	if (!(pFormView->m_CheckButton_Grid.GetCheck()))
	{
		m_pSelectCube->SetFitGrid(false);

		ENGINE::CTransform* pTransform = dynamic_cast<ENGINE::CTransform*>(m_pSelectCube->Get_Component(L"Transform"));
		pFormView->UpdateTransformStr(
			pTransform->GetPos(),
			D3DXVECTOR3(pTransform->GetAngle(ENGINE::ANGLE_X), pTransform->GetAngle(ENGINE::ANGLE_Y), pTransform->GetAngle(ENGINE::ANGLE_Z)),
			pTransform->GetSize()
		);
	}
	else
	{
		m_pSelectCube->SetFitGrid(true);
		ENGINE::CTransform* pTransform = dynamic_cast<ENGINE::CTransform*>(m_pSelectCube->Get_Component(L"Transform"));
		pFormView->UpdateTransformStr(
			pTransform->GetPos(),
			D3DXVECTOR3(pTransform->GetAngle(ENGINE::ANGLE_X), pTransform->GetAngle(ENGINE::ANGLE_Y), pTransform->GetAngle(ENGINE::ANGLE_Z)),
			pTransform->GetSize()
		);
	}

	pFormView->EditDataExchange();
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
	m_pSelectCube->SetObjType(pFormView->m_wstrObjType);
	m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::PROPS, m_pSelectCube);

	pFormView->EditDataExchange();
	pFormView->InitData();
}

void CToolView::DragPicking(float _fPointX, float _fPointY)
{
	// Drag Picking
	if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && m_pSelectCube)
	{
		ENGINE::CTransform* pTransform = dynamic_cast<ENGINE::CTransform*>(m_pSelectCube->Get_Component(L"Transform"));

		if (m_vLastPickedCubePos.x >= pTransform->GetPos().x + 2
			|| m_vLastPickedCubePos.x <= pTransform->GetPos().x - 2)
		{
			m_vLastPickedCubePos = pTransform->GetPos();

			m_pSelectCube->SetClicked();
			CreateCube(false);
		}

		if (m_vLastPickedCubePos.y >= pTransform->GetPos().y + 2
			|| m_vLastPickedCubePos.y <= pTransform->GetPos().y - 2)
		{
			m_vLastPickedCubePos = pTransform->GetPos();

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
	NULL_CHECK_RETURN(pMainFrm, false);

	CMyFormView* pFormView = dynamic_cast<CMyFormView*>(pMainFrm->m_MainSplitter.GetPane(0, 0));
	NULL_CHECK_RETURN(pFormView, false);

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

	if (!m_pSelectCube) return;

	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	NULL_CHECK(pMainFrm);

	CMyFormView* pFormView = dynamic_cast<CMyFormView*>(pMainFrm->m_MainSplitter.GetPane(0, 0));
	NULL_CHECK(pFormView);

	if (GetAsyncKeyState('Z') & 0x8000)
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

	if (GetAsyncKeyState('X') & 0x8000)
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

void CToolView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CView::OnLButtonUp(nFlags, point);
	m_bIsMousePressing = false;
}
