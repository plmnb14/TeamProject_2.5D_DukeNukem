#pragma once

#ifndef __BULLET_H__

#include "GameObject.h"

namespace ENGINE
{
	class CResourceMgr;
	class CTimeMgr;
	class CVIBuffer;
	class CTexture;
	class CTransform;
	class CRigidBody;
	class CCollider;
}

class CBullet : public ENGINE::CGameObject
{
private:
	explicit CBullet(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CBullet();

public:
	enum BULLET_TYPE
	{
		PISTOL, RIFLE
	};

public:
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

private:
	virtual HRESULT Initialize() override;
	virtual void Release() override;

private:
	HRESULT AddComponent();
	void KeyInput();
	void BulletType();

private:
	void Set_Target(CGameObject* _Target);
	void Set_Device(LPDIRECT3DDEVICE9 pGraphicDev);
	void Set_Pos(D3DXVECTOR3 _Pos);
	void Set_Dir(D3DXVECTOR3 _Dir);
	void Set_Angle(float* _Angle);
	void Set_Speed(float _Speed) { m_fSpeed = _Speed; }
	void Set_WeaponTag(ENGINE::WEAPON_TAG _WeaponTag) { m_eWeaponTag = _WeaponTag; }

public:
	static CBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos, D3DXVECTOR3 _Dir , float* _Angle, float _Speed, ENGINE::WEAPON_TAG _WeaponTag);
	
private:
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;
	ENGINE::CKeyMgr*		m_pKeyMgr;

	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;
	D3DXVECTOR3				m_dir;

private:
	CGameObject*	m_pTarget;
	float			m_fSpeed;
	ENGINE::WEAPON_TAG	m_eWeaponTag;
	ENGINE::CRigidBody*		m_pRigid;
	ENGINE::CCollider*		m_pCollider;

	float m_fLifetime;

};

#define __BULLET_H__
#endif