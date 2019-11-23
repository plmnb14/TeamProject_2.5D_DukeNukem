#pragma once

#ifndef __PLAYER_H__

#include "GameObject.h"

namespace ENGINE
{
	class CResourceMgr;
	class CTimeMgr;
	class CVIBuffer;
	class CTexture;
	class CTransform;
	class CCollider;
	class CCameraSubject;
	class CPlayerSubject;
	class CRigidBody;
	class CCondition;
}

class CCameraObserver;
class CPlayer : public ENGINE::CGameObject
{
public:
	enum WEAPON
	{
		MELLE, REVOLVER, RIFLE, SHOTGUN, LUNCHER
	};

	enum WEAPONACT
	{
		W_NONE, W_WALK, W_RUN,
		W_FIRST, W_DRAW, W_FIRE, W_RELOAD,
		W_ZOOMIN, W_ZOOMOUT, W_ZOOMFIRE
	};

private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CPlayer();

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
	void KeyInput();
	void UpdateObserverData();

private:
	void Shoot();
	void Shoot_Shotgun();
	void ShootDelay();
	void ShootType();
	void Zoom();
	void Reload();

private:
	void WeaponActState();

private:
	void Check_Physic();
	void Check_Slide();
	void Check_Run();

public:
	void Set_WeaponInfo(ENGINE::W_INFO* _WeaponInfo);
	bool Get_Zoom() { return m_bZoom; }
	WEAPONACT Get_WeaponAct() { return m_eActState; }
	void Set_WaponAct(WEAPONACT _Act) { m_eActState = _Act;};
	ENGINE::W_INFO* Get_WInfo() { return &m_pWInfo; }

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:	
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;
	ENGINE::CKeyMgr*		m_pKeyMgr;

	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;
	ENGINE::CCollider*		m_pCollider;
	ENGINE::CCollider*		m_pGroundChekCollider;
	ENGINE::CRigidBody*		m_pRigid;

	ENGINE::VTX_TEX*		m_myVtx;

	ENGINE::CCameraSubject*	m_pSubject;
	CCameraObserver*		m_pObserver;
	ENGINE::CPlayerSubject*	m_pPlayerSubject;

	// 현재 장착중인 무기 정보
	ENGINE::W_INFO			m_pWInfo;

	// 현재 플레이어의 정보 ( 체력 등 수치 값 & 각종 bool 값 )
	ENGINE::CCondition*		m_pCondition;

	// 현재 장착중인 무기 정보
	ENGINE::WEAPON_TAG		m_eWeaponState;
	map<ENGINE::WEAPON_TAG, ENGINE::W_INFO*>	m_mWeaponInfo;

private:
	WEAPONACT		m_eActState;

private:
	float m_fSlideUp;
	bool  m_bZoom;
	float m_fZoomSpeed;
	float m_fZoomAccel;
	float m_fMaxZoom;
	float m_fMinZoom;
	

};

#define __PLAYER_H__
#endif