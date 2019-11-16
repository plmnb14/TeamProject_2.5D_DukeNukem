#pragma once

#ifndef __ELEVATOR_H__

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

class CElevator : public ENGINE::CGameObject
{
protected:
	explicit CElevator(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CElevator();

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

protected:
	HRESULT AddComponent();
	void Move();

public:
	static CElevator* Create(LPDIRECT3DDEVICE9 pGraphicDev);

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
	bool					m_bIsUp;
	D3DXVECTOR3				m_vOriPos;

};

#define __ELEVATOR_H__
#endif