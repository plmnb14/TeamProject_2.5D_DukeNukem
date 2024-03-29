#pragma once

#ifndef __TERRAIN_H__

#include "GameObject.h"

namespace ENGINE
{
	class CResourceMgr;
	class CTimeMgr;
	class CVIBuffer;
	class CTexture;
	class CTransform;
}

class CTerrain : public ENGINE::CGameObject
{
protected:
	explicit CTerrain(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CTerrain();

public:
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

protected:
	virtual HRESULT Initialize() override;
	virtual void Release() override;

protected:
	HRESULT AddComponent();
	void KeyInput();

public:
	static CTerrain* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;

	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;

	ENGINE::TERRAIN_TYPE	m_eTerrainType;
};

#define __TERRAIN_H__
#endif