#pragma once
#ifndef __TERRAINRECT_H__

#include "Terrain.h"

class CTerrainRect : public CTerrain
{
private:
	explicit CTerrainRect(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CTerrainRect();

public:
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

private:
	virtual HRESULT Initialize() override;
	virtual void Release() override;

private:
	HRESULT AddComponent();

public:
	static CTerrainRect* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:

};


#define __TERRAINRECT_H__
#endif