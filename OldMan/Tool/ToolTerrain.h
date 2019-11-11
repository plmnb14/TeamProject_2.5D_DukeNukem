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
	void SetClicked();

protected:
	bool					m_bSetted;

};

#define __TOOL_TERRAIN_H__
#endif

