#pragma once

#ifndef __DOOR_H__

#include "GameObject.h"

namespace ENGINE
{
	class CResourceMgr;
	class CTimeMgr;
	class CVIBuffer;
	class CTexture;
	class CTransform;
	class CCollider;
}

class CDoor : public ENGINE::CGameObject
{
public:
	enum DOOR_DIR { DOOR_LEFT, DOOR_RIGHT, DOOR_UP, DOOR_DOWN, DOOR_END };

protected:
	explicit CDoor(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CDoor();

public:
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

protected:
	virtual HRESULT Initialize() override;
	virtual HRESULT LateInit() override;
	virtual void Release() override;

public:
	virtual void ChangeTex(wstring _wstrTex);

public:
	void SetMoveDist(float _fDist);

protected:
	HRESULT AddComponent();
	void Move();
	void CheckOpen();

public:
	static CDoor* Create(LPDIRECT3DDEVICE9 pGraphicDev, DOOR_DIR _eDir = DOOR_DIR::DOOR_UP);

protected:
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;
	ENGINE::CKeyMgr*		m_pKeyMgr;

	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;
	ENGINE::CCollider*		m_pCollider;

	ENGINE::TERRAIN_TYPE	m_eTerrainType;

	wstring					m_wstrTex;
	float					m_fMoveSpeed;
	float					m_fMoveDistY;
	float					m_fActiveDist;
	bool					m_bIsOpened;
	D3DXVECTOR3				m_vOriPos;
	CGameObject*			m_pPlayer;

	DOOR_DIR				m_eDir;
};

#define __DOOR_H__
#endif