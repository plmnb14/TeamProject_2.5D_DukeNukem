#pragma once
#ifndef __PICKUPITEM_H__

#include "GameObject.h"

namespace ENGINE
{
	class CResourceMgr;
	class CTimeMgr;
	class CVIBuffer;
	class CTexture;
	class CTransform;
	class CCollider;
	class CBillborad;
	class CCameraSubject;
}

class CCameraObserver;
class CPickupItem : public ENGINE::CGameObject
{
public:
	enum ITEM_TYPE
	{
		ITEM_HEALPACK,
		ITEM_ARMORPACK,
		ITEM_AMMOBOX_SHORT, // for SMG, Revolver
		ITEM_AMMOBOX_LONG,  // for Rifle, Sniper Rifle
		ITEM_AMMOBOX_SHOTGUN,
		ITEM_AMMOBOX_ROCKET,
		ITEM_AMMOBOX_GRENADE,
		ITEM_END
	};

protected:
	explicit CPickupItem(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CPickupItem();

public:
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

protected:
	virtual HRESULT Initialize() override;
	virtual HRESULT LateInit() override;
	virtual void Release() override;


public:
	void ChangeTex(wstring _wstrTex);

protected:
	HRESULT AddComponent();
	void Hitted();

public:
	static CPickupItem* Create(LPDIRECT3DDEVICE9 pGraphicDev, ITEM_TYPE _eType);

protected:
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;

	CCameraObserver*		m_pObserver;
	ENGINE::CCameraSubject*	m_pSubject;

	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;
	ENGINE::CCollider*		m_pCollider;
	ENGINE::CBillborad*		m_pBillborad;

	D3DXMATRIX              m_matView;

	ITEM_TYPE				m_eItemType;

};


#define __PICKUPITEM_H__
#endif