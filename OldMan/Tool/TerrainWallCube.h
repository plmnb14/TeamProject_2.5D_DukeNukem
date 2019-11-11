#pragma once
#ifndef __TERRAIN_WALLCUBE_H__

#include "ToolTerrain.h"
#include "GameObject.h"
#include "Ray.h"

namespace ENGINE
{
	class CResourceMgr;
	class CTimeMgr;
	class CVIBuffer;
	class CTexture;
	class CTransform;
}

class CTerrainWallCube : public CToolTerrain
{
private:
	explicit CTerrainWallCube(LPDIRECT3DDEVICE9 pGraphicDev);
public:
	virtual ~CTerrainWallCube();

public:
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

private:
	virtual HRESULT Initialize() override;
	virtual void Release() override;

public:

private:
	HRESULT AddComponent();
	void KeyInput();
	void MouseInput();

public:
	static CTerrainWallCube* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;

	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;

	bool					m_bIsPicked;

	//Test
	CRay					m_Ray;
	ENGINE::VTX_TEX			m_tVtx;
};

#define __TERRAIN_WALLCUBE_H__
#endif
