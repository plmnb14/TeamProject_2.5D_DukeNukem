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
	class CCollider;
	class CCameraSubject;
	class CBillborad;
	class CRigidBody;

}
class CCameraObserver;
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
	virtual HRESULT LateInit();
	virtual void Release() override;

private:
	HRESULT AddComponent();
	void KeyInput();

private:
	void Set_Target(CGameObject* _Target);
	void Set_Device(LPDIRECT3DDEVICE9 pGraphicDev);
	void Set_Pos(D3DXVECTOR3 _Pos);
	void Set_Dir(D3DXVECTOR3 _Dir);
	void Set_Angle(float* _Angle);

public:
	static CBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos, D3DXVECTOR3 _Dir , float* _Angle);

private:
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;
	ENGINE::CKeyMgr*		m_pKeyMgr;

	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;
	ENGINE::CCollider*		m_pCollider;
	ENGINE::CCameraSubject*	m_pSubject;
	ENGINE::CBillborad*		m_pBillborad;
	ENGINE::CRigidBody*		m_pRigid;



private:
	CGameObject*			m_pTarget;
	CCameraObserver*		m_pObserver;
	D3DXMATRIX              m_matView;
	bool					m_bullet;
};

#define __BULLET_H__
#endif