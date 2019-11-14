#pragma once
#ifndef __TERRAIN_RECT_H__

#include "ToolTerrain.h"
#include "GameObject.h"

class CToolTerrainRect : public CToolTerrain
{
private:
	explicit CToolTerrainRect(LPDIRECT3DDEVICE9 pGraphicDev);
public:
	virtual ~CToolTerrainRect();

public:
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;
	virtual bool CheckGrid(D3DXVECTOR3& _vVtx) override;
	virtual void ChangeTex();

private:
	virtual HRESULT Initialize() override;
	virtual void Release() override;

private:
	HRESULT AddComponent();
	void KeyInput();

public:
	static CToolTerrainRect* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
};

#define __TERRAIN_RECT_H__
#endif
