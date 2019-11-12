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
<<<<<<< Updated upstream
	void SetClicked();

protected:
	bool					m_bSetted;

=======
	virtual bool CheckGrid(D3DXVECTOR3& _vVtx);

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
	wstring					m_wstrTex;
	ENGINE::TERRAIN_TYPE	m_eTerrainType;
>>>>>>> Stashed changes
};

#define __TOOL_TERRAIN_H__
#endif

