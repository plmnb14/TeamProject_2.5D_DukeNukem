#pragma once
#ifndef __TRIGGER_H__

#include "GameObject.h"

namespace ENGINE
{
	class CResourceMgr;
	class CTimeMgr;
	class CVIBuffer;
	class CTexture;
	class CTransform;
	class CCollider;
}

class CTrigger : public ENGINE::CGameObject
{
public:
	enum TRIGGER_TYPE
	{ 
		TRIGGER_NEXTSTAGE,
		TRIGGER_LEDGE,
		TRIGGER_END
	};

protected:
	explicit CTrigger(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CTrigger();

public:
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

protected:
	virtual HRESULT Initialize() override;
	virtual HRESULT LateInit() override;
	virtual void Release() override;

public:
	//void SetEvent(void(*_fpClickEvent)());

protected:
	HRESULT AddComponent();
	void CheckTriggerActive();

public:
	static CTrigger* Create(LPDIRECT3DDEVICE9 pGraphicDev, TRIGGER_TYPE _eType, int iIdx);

protected:
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;

	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CTransform*		m_pTransform;
	ENGINE::CCollider*		m_pCollider;

	ENGINE::VTX_TEX*		m_myVtx;

	TRIGGER_TYPE			m_eTriggerType;
	int						m_iIndex;
	
	//void(*m_fpTriggerEvent)();
};


#define __TRIGGER_H__
#endif