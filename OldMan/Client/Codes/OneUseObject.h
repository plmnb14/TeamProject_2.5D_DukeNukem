#pragma once
#ifndef __ONEUSEOBJECT_H__

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
class COneUseObject : public ENGINE::CGameObject
{
public:
	enum MAPOBJECT_TYPE
	{
		MAPOBJ_GLASSBOTTLE,
		MAPOBJ_BOX,
		MAPOBJ_GAS,
		MAPOBJ_END
	};

protected:
	explicit COneUseObject(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~COneUseObject();

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
	static COneUseObject* Create(LPDIRECT3DDEVICE9 pGraphicDev, MAPOBJECT_TYPE _eType);

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

	MAPOBJECT_TYPE			m_eItemType;

};


#define __ONEUSEOBJECT_H__
#endif