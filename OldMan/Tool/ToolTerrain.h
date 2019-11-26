#pragma once
#ifndef __TOOL_TERRAIN_H__

namespace ENGINE
{
	class CResourceMgr;
	class CTimeMgr;
	class CVIBuffer;
	class CTexture;
	class CTransform;
}

#include "GameObject.h"
class CRay;
class CToolTerrain : public ENGINE::CGameObject
{
protected:
	CToolTerrain(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CToolTerrain();

public:
	virtual bool CheckGrid(D3DXVECTOR3& _vVtx);
	virtual void ChangeTex();

public:
	wstring GetTexName();
	wstring GetObjType();
	 ENGINE::TERRAIN_TYPE GetTerrainType();
	 bool GetPicked();

public:
	void SetClicked(bool _bIsSet = true);
	void SetFitGrid(bool _bIsFit);
	void SetFitX(bool _bIsFit);
	void SetTexName(wstring _wstrTex);
	void SetObjType(wstring _wstrType);

protected:
	void MouseInput();

protected:
	ENGINE::TERRAIN_TYPE	m_eTerrainType;
	wstring					m_wstrTex;
	wstring					m_wstrObjType;

	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;

	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;

	ENGINE::VTX_TEX*		m_myVtxTex;
	ENGINE::VTX_CUBE*		m_myVtxCube;
	ENGINE::VTX_COL*		m_myVtxCol;

	CRay*					m_pRay;

	bool					m_bIsPicked;
	bool					m_bSetted;
	bool					m_bIsFitGrid;
	bool					m_bIsFitX;
};

#define __TOOL_TERRAIN_H__
#endif

