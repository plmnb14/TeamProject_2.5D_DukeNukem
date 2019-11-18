#pragma once
#ifndef __TERRAINRECT_H__

#include "Terrain.h"

class CTerrainRect : public CTerrain
{
protected:
	explicit CTerrainRect(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CTerrainRect();

public:
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

protected:
	virtual HRESULT Initialize() override;
	virtual HRESULT LateInit() override;
	virtual void Release() override;

public:
	virtual void ChangeTex(wstring _wstrTex) override;

protected:
	HRESULT AddComponent();

public:
	static CTerrainRect* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:

};


#define __TERRAINRECT_H__
#endif