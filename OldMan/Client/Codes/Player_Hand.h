#pragma once

#ifndef __PLAYER_HAND__

#include "GameObject.h"
#include "Player.h"

namespace ENGINE
{
	class CResourceMgr;
	class CTimeMgr;
	class CVIBuffer;
	class CTexture;
	class CTransform;
	class CCameraSubject;
	class CAnimator;
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
	void ChangeTex(wstring _wstrTex);
	void Set_Pos(D3DXVECTOR3 _Pos);
	void Set_WeaponAct();
	void WeaponActState();
	

public:
	static CPlayer_Hand* Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _Target);

private:
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;

	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;
	ENGINE::CAnimator*		m_pAnimator;

	ENGINE::CCameraSubject*	m_pCameraSubject;
	CCameraObserver*		m_pCameraObserver;

	CGameObject*			m_pTarget;
	wstring					m_OldwstrTex;

	wstring					m_wstrTex;

	float					m_fSizeX;
	float					m_fSizeY;
	D3DXVECTOR3				m_vPos;
	float m_fFrame;

	CPlayer::WEAPONACT		m_eOldAcState;
	CPlayer::WEAPONACT		m_eActState;
};

#define __PLAYER_HAND__
#endif