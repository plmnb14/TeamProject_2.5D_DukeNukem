#pragma once

#ifndef __PLAYER_HAND__

#include "GameObject.h"

namespace ENGINE
{
	class CResourceMgr;
	class CTimeMgr;
	class CVIBuffer;
	class CTexture;
	class CTransform;
	class CCameraSubject;
}

class CCameraObserver;
class CPlayer_Hand : public ENGINE::CGameObject
{
private:
	explicit CPlayer_Hand(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CPlayer_Hand();

public:
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

private:
	virtual HRESULT Initialize() override;
	virtual HRESULT LateInit();
	virtual void Release() override;

private:
	HRESULT AddComponent();

public:
	void ChangeTex(wstring _wstrTex, int _CurFrame);
	void Set_Pos(D3DXVECTOR3 _Pos);

public:
	static CPlayer_Hand* Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _Target);

private:
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;

	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;

	ENGINE::CCameraSubject*	m_pCameraSubject;
	CCameraObserver*		m_pCameraObserver;

	CGameObject*			m_pTarget;
	wstring					m_wstrTex;
	float					m_fSizeX;
	float					m_fSizeY;
	D3DXVECTOR3				m_vPos;
	float m_fFrame;
};

#define __PLAYER_HAND__
#endif