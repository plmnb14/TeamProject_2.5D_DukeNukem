#pragma once
#ifndef __TERRAIN_WALLCUBE_H__

#include "ToolTerrain.h"
#include "GameObject.h"

class CToolTerrainWallCube : public CToolTerrain
{
private:
	explicit CToolTerrainWallCube(LPDIRECT3DDEVICE9 pGraphicDev);
public:
	virtual ~CToolTerrainWallCube();

public:
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

private:
	virtual HRESULT Initialize() override;
	virtual void Release() override;

public:

private:
	HRESULT AddComponent();
	void KeyInput();

public:
	static CToolTerrainWallCube* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:

};

#define __TERRAIN_WALLCUBE_H__
#endif
