#include "stdafx.h"
#include "ToolTerrain.h"
#include "Ray.h"

CToolTerrain::CToolTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev),
	m_bSetted(false), m_bIsFitGrid(false),
	m_eTerrainType(ENGINE::TERRAIN_END),
	m_wstrTex(L"")
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
