#pragma once
#ifndef __TERRAINWALLCUBE_H__

#include "Terrain.h"

class CTerrainWallCube : public CTerrain
{
private:
	explicit CTerrainWallCube(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CTerrainWallCube();

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
	static CTerrainWallCube* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:

};


#define __TERRAINWALLCUBE_H__
#endif