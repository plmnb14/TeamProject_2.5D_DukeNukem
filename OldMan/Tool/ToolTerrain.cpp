#include "stdafx.h"
#include "ToolTerrain.h"
#include "Ray.h"

CToolTerrain::CToolTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev),
	m_bSetted(false), m_bIsFitGrid(false)
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

bool CToolTerrain::CheckGrid(D3DXVECTOR3& _vVtx)
{
	return false;
}
