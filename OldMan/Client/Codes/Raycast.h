#pragma once

#ifndef __RAYCAST_H__

#include "GameObject.h"

namespace ENGINE
{
	class CResourceMgr;
	class CTimeMgr;
	class CVIBuffer;
	class CTexture;
	class CTransform;
	class CCameraSubject;
}

class CCameraObserver;
class CRayCast : public ENGINE::CGameObject
{
private:
	explicit CRayCast(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CRayCast();

public:
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

private:
	virtual HRESULT Initialize() override;
	virtual void Release() override;

	HRESULT AddComponent();

public:
	void SetUp_ViewSpace();
	void SetUp_LocalSpace();

public:
	POINT Get_MousePos();


public:
	static CRayCast* Create();

private:
	ENGINE::CCameraSubject*	m_pSubject;
	CCameraObserver*		m_pObserver;
};

#define __RAYCAST_H__
#endif