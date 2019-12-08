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
		MELEE, REVOLVER, RIFLE, SHOTGUN, LUNCHER
	};

	enum WEAPONACT
	{
		W_NONE, W_IDLE, W_WALK, W_RUN,
		W_FIRST, W_DRAW, W_FIRE, W_RELOAD,
		W_SPECIAL_READY, W_SPECIAL_SHOT , W_SPECIAL_END,
		W_ZOOMIN, W_ZOOMOUT, W_ZOOMFIRE,
		W_LEDGE, W_GRENADE,
		W_R_FIST, W_L_FIST
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
	void SpecialShot();
	void Grenade();

private:
	void WeaponActState();

private:
	void Check_Weapon();
	void Check_Physic();
	void Check_Slide();
	void Check_Run();
	void Check_Ledge();
	void Check_Grenade();
	void Check_Hitted();
	void Check_WalkSound(bool _bIsRun = false);
	void Check_Dead();

public:
	void Reload();

public:
	void Add_GrenadeCount(int _GrenadeCount) { m_iGrenadeCount = _GrenadeCount; }

public:
	void Set_WeaponInfo(ENGINE::W_INFO* _WeaponInfo);
	void Set_WeaponInfo(ENGINE::WEAPON_TAG _eTag, ENGINE::W_INFO* _WeaponInfo);
	void Set_SpecialShot(bool _Special) { m_bSpecial = _Special; }
	void Set_CanAttack(bool _Attack) { m_bCanAttack = _Attack; }
	void Set_CanLedge(bool _Ledge) { m_bCanLedge = _Ledge; }
	void Set_IsLedge(bool _IsLedge) { m_bIsLedge = _IsLedge; }
	void Set_LedgeVec(D3DXVECTOR3 _Vec) { m_vLedgeVec = _Vec; }
	void Set_Grenade(bool _Grenade) { m_bGrenade = _Grenade; }
	void Set_GrenadeCount(int _iCount) { m_iGrenadeCount = _iCount; }
	void Set_WayPoint(int _Index) { m_iWaypoint_Index = _Index; }
	void Set_NextStage(bool _bool) { m_bOnNextStage = _bool; }
	void Set_MeleeTrigger(bool _bool);

public:
	bool Get_CanLedge() { return m_bCanLedge; }
	bool Get_IsLedge() { return m_bIsLedge; }
	bool Get_Zoom() { return m_bZoom; }
	WEAPONACT Get_WeaponAct() { return m_eActState; }
	void Set_WaponAct(WEAPONACT _Act) { m_eActState = _Act;};
	ENGINE::W_INFO* Get_WInfo() { return &m_tWInfo; }
	ENGINE::W_INFO* Get_WInfo(ENGINE::WEAPON_TAG _eTag) { auto iter_find = m_mWeaponInfo.find(_eTag); if (m_mWeaponInfo.end() == iter_find) return nullptr; return m_mWeaponInfo[_eTag]; }
	ENGINE::WEAPON_TAG	Get_WState() { return m_eWeaponState; }
	int Get_GrenadeCount() { return m_iGrenadeCount; }
	bool Get_NextStage() { return m_bOnNextStage; }

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:	
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;
	ENGINE::CKeyMgr*		m_pKeyMgr;

	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;
	ENGINE::CRigidBody*		m_pRigid;

	ENGINE::CCollider*		m_pCollider;
	ENGINE::CCollider*		m_pGroundChekCollider;
	ENGINE::CCollider*		m_pColliderLedge;
	ENGINE::CCollider*		m_pMeleeCollider;

	ENGINE::VTX_TEX*		m_myVtx;

	ENGINE::CCameraSubject*	m_pSubject;
	CCameraObserver*		m_pObserver;
	ENGINE::CPlayerSubject*	m_pPlayerSubject;

	// 현재 장착중인 무기 정보
	ENGINE::W_INFO			m_tWInfo;

	// 현재 플레이어 정보 구조체 (일단 체력, 쉴드만)
	ENGINE::CONDITION		m_tCondition;

	// 현재 플레이어의 정보 ( 체력 등 수치 값 & 각종 bool 값 )
	ENGINE::CCondition*		m_pCondition;

	// 현재 장착중인 무기 정보
	ENGINE::WEAPON_TAG		m_eWeaponState;
	map<ENGINE::WEAPON_TAG, ENGINE::W_INFO*>	m_mWeaponInfo;

private:
	WEAPONACT		m_eActState;

private:
	D3DXVECTOR3 m_vLedgeVec;
	D3DXVECTOR3 m_vLedgeUpVec;

	float m_fSlideUp;
	bool  m_bZoom;
	bool  m_bSpecial;
	bool  m_bCanAttack;
	bool  m_bGrenade;

	float m_fZoomSpeed;
	float m_fZoomAccel;
	float m_fMaxZoom;
	float m_fMinZoom;

	bool m_bCanLedge;
	bool m_bIsLedge;

	float m_fLength_Y;
	float m_fHorizontal;

	int	m_iJumpCount;
	int m_iGrenadeCount;
	int m_iMaxGrenadeCount;

	int m_iWaypoint_Index;
	
	float	m_fWalkSoundDelay;
	int		m_iWalkSoundIndex;

	bool	m_bPlaySlideSound;
	bool	m_bOnNextStage;

	float	m_DeathTimer;

};

#define __PLAYER_H__
#endif