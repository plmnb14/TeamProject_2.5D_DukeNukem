#pragma once
#ifndef __TERRAIN_CUBE_H__

#include "GameObject.h"

namespace ENGINE
{
	class CResourceMgr;
	class CTimeMgr;
	class CVIBuffer;
	class CTexture;
	class CTransform;
}

class CTerrainCube : public ENGINE::CGameObject
{
private:
	explicit CTerrainCube(LPDIRECT3DDEVICE9 pGraphicDev);
public:
	virtual ~CTerrainCube();

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
	static CTerrainCube* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;

	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;

	bool					m_bSetted;
};

#define __TERRAIN_CUBE_H__
#endif
