#pragma once
#ifndef __TOOL_TERRAIN_H__

#include "GameObject.h"
class CToolTerrain : public ENGINE::CGameObject
{
protected:
	CToolTerrain(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CToolTerrain();

public:
	virtual bool CheckGrid(D3DXVECTOR3& _vVtx);

public:
	void SetClicked();
	void SetFitGrid(bool _bIsFit);

protected:
	bool					m_bSetted;
	bool					m_bIsFitGrid;

};

#define __TOOL_TERRAIN_H__
#endif

