#include "stdafx.h"
#include "ToolTerrain.h"


CToolTerrain::CToolTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev),
<<<<<<< Updated upstream
	m_bSetted(false)
=======
	m_bSetted(false), m_bIsFitGrid(false),
	m_wstrTex(L""), m_eTerrainType(ENGINE::TERRAIN_END)
>>>>>>> Stashed changes
{
}


CToolTerrain::~CToolTerrain()
{
}

void CToolTerrain::SetClicked()
{
	m_bSetted = true;
}
<<<<<<< Updated upstream
=======

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

wstring CToolTerrain::GetTexName()
{
	return m_wstrTex;
}

ENGINE::TERRAIN_TYPE CToolTerrain::GetTerrainType()
{
	return m_eTerrainType;
}
>>>>>>> Stashed changes
