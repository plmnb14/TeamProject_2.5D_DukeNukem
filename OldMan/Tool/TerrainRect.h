#pragma once
#ifndef __TERRAIN_RECT_H__

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

class CTerrainRect : public CToolTerrain
{
private:
	explicit CTerrainRect(LPDIRECT3DDEVICE9 pGraphicDev);
public:
	virtual ~CTerrainRect();

public:
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

private:
	virtual HRESULT Initialize() override;
	virtual void Release() override;

public:
	void SetClicked();

private:
	HRESULT AddComponent();
	void KeyInput();
	void MouseInput();

public:
	static CTerrainRect* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;

	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;

	bool					m_bSetted;
	bool					m_bIsPicked;

	//Test
	CRay					m_Ray;
	ENGINE::VTX_TEX			m_tVtx;
};

#define __TERRAIN_RECT_H__
#endif
