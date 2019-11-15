#pragma once
#ifndef __TERRAINCUBE_H__

#include "Terrain.h"


class CTerrainCube : public CTerrain
{
private:
	explicit CTerrainCube(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CTerrainCube();

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
	static CTerrainCube* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:

};


#define __TERRAINCUBE_H__
#endif