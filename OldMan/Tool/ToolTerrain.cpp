#include "stdafx.h"
#include "ToolTerrain.h"
#include "Ray.h"
#include "Trasform.h"

#include "ToolView.h"
#include "MyFormView.h"
#include "MainFrm.h"

CToolTerrain::CToolTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev),
	m_bSetted(false), m_bIsFitGrid(false),
	m_eTerrainType(ENGINE::TERRAIN_END),
	m_wstrTex(L""),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr),
	m_bIsPicked(false),
	m_myVtxCol(nullptr), m_myVtxTex(nullptr), m_myVtxCube(nullptr), m_pRay(new CRay),
	m_wstrObjType(L"Terrain"), m_iIndex(0)
{
}


CToolTerrain::~CToolTerrain()
{
	ENGINE::Safe_Delete(m_pRay);
	ENGINE::Safe_Delete(m_myVtxTex);
	ENGINE::Safe_Delete(m_myVtxCol);
	ENGINE::Safe_Delete(m_myVtxCube);
}

void CToolTerrain::SetClicked(bool _bIsSet)
{
	m_bSetted = _bIsSet;
}

void CToolTerrain::SetFitGrid(bool _bIsFit)
{
	m_bIsFitGrid = _bIsFit;
}

void CToolTerrain::SetFitX(bool _bIsFit)
{
	m_bIsFitX = true;
}

void CToolTerrain::SetTexName(wstring _wstrTex)
{
	m_wstrTex = _wstrTex;
}

void CToolTerrain::SetObjType(wstring _wstrType)
{
	m_wstrObjType = _wstrType;
}

void CToolTerrain::SetIndex(int _iIdx)
{
	m_iIndex = _iIdx;
}

void CToolTerrain::MouseInput()
{
	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	NULL_CHECK(pMainFrm);

	CToolView* pView = dynamic_cast<CToolView*>(pMainFrm->m_MainSplitter.GetPane(0, 1));
	NULL_CHECK(pView);

	CMyFormView* pFormView = dynamic_cast<CMyFormView*>(pMainFrm->m_MainSplitter.GetPane(0, 0));
	NULL_CHECK(pFormView);

	if (!m_bSetted)
	{
		//if (m_bIsFitGrid)
		//	return;
		
		D3DXMATRIX matView, matProj;
		matView = pView->m_ViewMatrix;
		matProj = pView->m_ProjMatrix;

		D3DXVECTOR3 v3 = CRay::GetDirection();

		float fMulX = (v3.x > 0) ? v3.x * (CRay::GetMousePos().x / (WINCX >> 4)) : abs(v3.x) * ((CRay::GetMousePos().x / (WINCX >> 4)) - (WINCX / (WINCX >> 4)));
		float fMulY = (v3.y > 0) ? v3.y * ((WINCY - CRay::GetMousePos().y) / (WINCY >> 4)) : abs(v3.y) * (((WINCY - CRay::GetMousePos().y) / (WINCY >> 4)) - (WINCY / (WINCY >> 4)));
		
		D3DXVECTOR3 vPos = D3DXVECTOR3(fMulX, 0.f, fMulY + 10.f);
		D3DXMatrixInverse(&matView, 0, &matView);
		D3DXVec3TransformCoord(&vPos, &vPos, &matView);

		vPos.y = m_pTransform->GetPos().y;
		m_pTransform->SetPos(vPos);

		if (m_bIsFitGrid)
		{
			int iPosArr[3] = { int(m_pTransform->GetPos().x) , int(m_pTransform->GetPos().y) , (int)(m_pTransform->GetPos().z) };
			if(iPosArr[0] % 2 == 0)
				m_pTransform->SetPos(D3DXVECTOR3(float(iPosArr[0]), m_pTransform->GetPos().y, m_pTransform->GetPos().z));
			if (iPosArr[2] % 2 == 0)
				m_pTransform->SetPos(D3DXVECTOR3(m_pTransform->GetPos().x, m_pTransform->GetPos().y, float(iPosArr[2])));
		}

	}
	else if (m_bSetted)
	{
		//if (pFormView->m_bIsOnDlg)
		//	return;

		if (!pView->m_bIsEraseMode)
			return;

		if (pView->m_pDeleteCube)
		{
			if (pView->m_pDeleteCube != this)
				return;
		}

		// 마우스 호버 시, 알 수 있도록 함.
		D3DXVECTOR3 vPos;
		m_pRay->GetDirection(); // Update Ray Dir
		
		switch (m_eTerrainType)
		{
		case ENGINE::TERRAIN_CUBE:
		{
			if (!m_myVtxCube)
			{
				m_myVtxCube = new ENGINE::VTX_CUBE[8];

				m_pResourceMgr->GetVertexInfo(
					ENGINE::RESOURCE_STATIC,
					L"Buffer_CubeTex",
					m_myVtxCube);
			}

			if (m_pRay->IsPicked(m_myVtxCube[0].vPos, m_myVtxCube[1].vPos, m_myVtxCube[2].vPos, vPos, m_pTransform->GetWorldMatrix())
				|| m_pRay->IsPicked(m_myVtxCube[0].vPos, m_myVtxCube[2].vPos, m_myVtxCube[3].vPos, vPos, m_pTransform->GetWorldMatrix()))
				m_bIsPicked = true;
			else
				m_bIsPicked = false;
								
			break;
		}
		case ENGINE::TERRAIN_WALL:

			break;
		case ENGINE::TERRAIN_RECT:
		{
			if (!m_myVtxTex)
			{
				m_myVtxTex = new ENGINE::VTX_TEX[4];

				m_pResourceMgr->GetVertexInfo(
					ENGINE::RESOURCE_STATIC,
					L"Buffer_RcTex",
					m_myVtxTex);
			}

			if (m_pRay->IsPicked(m_myVtxTex[0].vPos, m_myVtxTex[1].vPos, m_myVtxTex[2].vPos, vPos, m_pTransform->GetWorldMatrix())
				|| m_pRay->IsPicked(m_myVtxTex[0].vPos, m_myVtxTex[2].vPos, m_myVtxTex[3].vPos, vPos, m_pTransform->GetWorldMatrix()))
			{
				m_bIsPicked = true;
			}
		else
			m_bIsPicked = false;
			break;
		}
		case ENGINE::TERRAIN_END:
			break;
		}

	}
}

bool CToolTerrain::CheckGrid(D3DXVECTOR3& _vVtx)
{
	return false;
}

void CToolTerrain::ChangeTex()
{
}

wstring CToolTerrain::GetTexName()
{
	return m_wstrTex;
}

wstring CToolTerrain::GetObjType()
{
	return m_wstrObjType;
}

ENGINE::TERRAIN_TYPE CToolTerrain::GetTerrainType()
{
	return m_eTerrainType;
}

bool CToolTerrain::GetPicked()
{
	return m_bIsPicked;
}

int CToolTerrain::GetIndex()
{
	return m_iIndex;
}
