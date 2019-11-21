#pragma once
#ifndef __TERRAINCUBE_H__

#include "Terrain.h"


class CTerrainCube : public CTerrain
{
protected:
	explicit CTerrainCube(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CTerrainCube();

public:
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;
	virtual void ChangeTex(wstring _wstrTex) override;

protected:
	virtual HRESULT Initialize() override;
	virtual HRESULT LateInit() override;
	virtual void Release() override;

protected:
	HRESULT AddComponent();

public:
	static CTerrainCube* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:

};


#define __TERRAINCUBE_H__
#endif