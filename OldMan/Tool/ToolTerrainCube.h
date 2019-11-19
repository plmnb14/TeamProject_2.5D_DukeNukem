#pragma once
#ifndef __TERRAIN_CUBE_H__

#include "ToolTerrain.h"
#include "GameObject.h"

class CToolTerrainCube : public CToolTerrain
{
private:
	explicit CToolTerrainCube(LPDIRECT3DDEVICE9 pGraphicDev);
public:
	virtual ~CToolTerrainCube();

public:
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;
	virtual void ChangeTex() override;

private:
	virtual HRESULT Initialize() override;
	virtual void Release() override;

public:

private:
	HRESULT AddComponent();
	void KeyInput();

public:
	static CToolTerrainCube* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:

};

#define __TERRAIN_CUBE_H__
#endif
