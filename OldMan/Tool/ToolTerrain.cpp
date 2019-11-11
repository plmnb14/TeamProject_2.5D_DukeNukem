#include "stdafx.h"
#include "ToolTerrain.h"


CToolTerrain::CToolTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev),
	m_bSetted(false)
{
}


CToolTerrain::~CToolTerrain()
{
}

void CToolTerrain::SetClicked()
{
	m_bSetted = true;
}
