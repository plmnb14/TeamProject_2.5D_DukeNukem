#pragma once

#ifndef __GRENADE_H__

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
	class CBillborad;
	class CCameraSubject;
	class CCondition;
}
class CCameraObserver;
class CGrenade : public ENGINE::CGameObject
{
private:
	explicit CGrenade(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CGrenade();

public:
	enum GRENADE_TYPE
	{
		FRAG, FROZEN
	};

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
	void BulletType();
	void Check_Physic();

private:
	void Set_Target(CGameObject* _Target);
	void Set_Device(LPDIRECT3DDEVICE9 pGraphicDev);
	void Set_Pos(D3DXVECTOR3 _Pos);
	void Set_Dir(D3DXVECTOR3 _Dir);
	void Set_Angle(float* _Angle);
	void Set_Speed(float _Speed) { m_fSpeed = _Speed; }
	void Set_WeaponTag(ENGINE::WEAPON_TAG _WeaponTag) { m_eWeaponTag = _WeaponTag; }
	void Set_Attack(float _Attack);

public:
	static CGrenade* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos, D3DXVECTOR3 _Dir, float* _Angle, float _Speed, ENGINE::WEAPON_TAG _WeaponTag, float _Attack = 10);

private:
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;
	ENGINE::CBillborad*		m_pBillborad;
	ENGINE::CCameraSubject*	m_pSubject;
	ENGINE::CCondition*		m_pCondition;

	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;
	D3DXVECTOR3				m_dir;
	CCameraObserver*		m_pObserver;
private:
	CGameObject*	m_pTarget;
	float			m_fSpeed;
	ENGINE::WEAPON_TAG	m_eWeaponTag;
	ENGINE::CRigidBody*		m_pRigid;
	ENGINE::CCollider*		m_pCollider;
	ENGINE::CCollider*		m_pGroundChekCollider;
	D3DXMATRIX              m_matView;



	float m_fLifetime;
	float m_fAttack;

	float m_fBoundAngle;
	float m_fOrigin_YAngle;

};

#define __GRENADE_H__
#endif