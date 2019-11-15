#include "stdafx.h"
#include "ToolTerrain.h"
#include "Ray.h"
#include "Trasform.h"

CToolTerrain::CToolTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev),
	m_bSetted(false), m_bIsFitGrid(false),
	m_eTerrainType(ENGINE::TERRAIN_END),
	m_wstrTex(L""),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr),
	m_bIsPicked(false)
{
}


CToolTerrain::~CToolTerrain()
{
}

void CToolTerrain::SetClicked()
{
	m_bSetted = true;
}

void CToolTerrain::SetFitGrid(bool _bIsFit)
{
	m_bIsFitGrid = _bIsFit;
}

void CToolTerrain::SetTexName(wstring _wstrTex)
{
	m_wstrTex = _wstrTex;
}

void CToolTerrain::MouseInput()
{
	if (!m_bSetted)
	{
		//if (m_bIsFitGrid)
		//	return;

		D3DXVECTOR3 v3 = CRay::GetDirection();

		float fTestMul = 15.f;
		D3DXVECTOR3 vPos = D3DXVECTOR3(v3.x * fTestMul, m_pTransform->GetPos().y, v3.y * fTestMul);
		//D3DXMATRIX matWorld;
		//ENGINE::GetGraphicDev()->GetDevice()->GetTransform(D3DTS_WORLD, &matWorld);
		//D3DXMatrixInverse(&matWorld, 0, &matWorld);
		//D3DXVec3TransformCoord(&vPos, &vPos, &matWorld);

		//cout << vPos.x << ", " << vPos.y << ", " << vPos.z << endl;

		m_pTransform->SetPos(vPos);

		if (m_bIsFitGrid)
		{
			int iPosArr[3] = { int(m_pTransform->GetPos().x) , int(m_pTransform->GetPos().y) , (int)(m_pTransform->GetPos().z) };
			m_pTransform->SetPos(D3DXVECTOR3(float(iPosArr[0]), float(iPosArr[1]), float(iPosArr[2])));
		}

	}
	else if (m_bSetted)
	{
		// 마우스 호버 시, 알 수 있도록 함.
		D3DXVECTOR3 v3 = CRay::GetDirection();

		D3DXVECTOR3 vPos;
		CRay _Ray;

		ENGINE::VTX_TEX* pVtxTex = nullptr;
		ENGINE::VTX_COL* pVtxCol = nullptr;
		switch (m_eTerrainType)
		{
		case ENGINE::TERRAIN_CUBE:
			pVtxCol = new ENGINE::VTX_COL[8];
			m_pResourceMgr->GetVertexInfo(ENGINE::RESOURCE_STATIC, L"Buffer_CubeCol", pVtxCol);
			break;
		case ENGINE::TERRAIN_WALL:
			pVtxCol = new ENGINE::VTX_COL[8];
			m_pResourceMgr->GetVertexInfo(ENGINE::RESOURCE_STATIC, L"Buffer_WallCubeCol", pVtxCol);
			break;
		case ENGINE::TERRAIN_RECT:
		{
			pVtxTex = new ENGINE::VTX_TEX[4];
			m_pResourceMgr->GetVertexInfo(ENGINE::RESOURCE_STATIC, L"Buffer_RcTex", pVtxTex);

			if (_Ray.IsPicked(pVtxTex[0].vPos, pVtxTex[1].vPos, pVtxTex[2].vPos, vPos)
				|| _Ray.IsPicked(pVtxTex[0].vPos, pVtxTex[2].vPos, pVtxTex[3].vPos, vPos))
				m_bIsPicked = true;
			else
				m_bIsPicked = false;
			break;
		}
		case ENGINE::TERRAIN_END:
			break;
		}
		ENGINE::Safe_Delete_Array(pVtxTex);
		ENGINE::Safe_Delete_Array(pVtxCol);
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

ENGINE::TERRAIN_TYPE CToolTerrain::GetTerrainType()
{
	return m_eTerrainType;
}
