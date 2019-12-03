#pragma once

#ifndef __ROCK_H__

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

class CRock : public ENGINE::CGameObject
{

protected:
	explicit CRock(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CRock();

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
	void Set_DelayTime(float _DelayTime) { m_fDelayTime = _DelayTime; };

protected:
	HRESULT AddComponent();
	void Move();

public:
	static CRock* Create(LPDIRECT3DDEVICE9 pGraphicDev , float _DelayTime);

protected:
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;

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

	float					m_fLifeTime;
	float					m_fDelayTime;
	float					m_fAccel;
};

#define __ROCK_H__
#endif