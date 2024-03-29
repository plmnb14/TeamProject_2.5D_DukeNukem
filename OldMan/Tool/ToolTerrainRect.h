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
	void MouseInput();

public:
	static CToolTerrainRect* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;

	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;

	bool					m_bIsPicked;
};

#define __TERRAIN_RECT_H__
#endif
