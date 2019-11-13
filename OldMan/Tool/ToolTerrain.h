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
	virtual void ChangeTex();

public:
	 wstring GetTexName();
	 ENGINE::TERRAIN_TYPE GetTerrainType();

public:
	void SetClicked();
	void SetFitGrid(bool _bIsFit);
	void SetTexName(wstring _wstrTex);

protected:
	bool					m_bSetted;
	bool					m_bIsFitGrid;
	ENGINE::TERRAIN_TYPE	m_eTerrainType;
	wstring					m_wstrTex;
};

#define __TOOL_TERRAIN_H__
#endif

