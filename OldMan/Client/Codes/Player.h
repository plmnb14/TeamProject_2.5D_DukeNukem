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
	class CRigidBody;
}

class CCameraObserver;
class CPlayer : public ENGINE::CGameObject
{
public:
	enum WEAPON
	{
		MELLE, REVOLVER, RIFLE, SHOTGUN, LUNCHER
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
	void Physic();

private:
	void Shoot();
	void ShootDelay();
	void ShootType();
	void Reload();
	void Swap_Weapon();

public:
	void Set_WeaponInfo(ENGINE::W_INFO* _WeaponInfo);

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

	// 현재 장착중인 무기 정보
	ENGINE::W_INFO			m_pWInfo;

	// 현재 플레이어의 정보 ( 체력 등 수치 값 & 각종 bool 값 )
	ENGINE::CONDITION		m_pCondition;

	// 현재 장착중인 무기 정보
	ENGINE::WEAPON_TAG		m_eWeaponState;
	map<ENGINE::WEAPON_TAG, ENGINE::W_INFO*>	m_mWeaponInfo;
	

};

#define __PLAYER_H__
#endif