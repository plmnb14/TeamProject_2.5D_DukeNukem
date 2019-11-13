#pragma once

#ifndef __GAMEOBJECT_H__

#include "Engine_Include.h"
#include "Layer.h"

BEGIN(ENGINE)

class CComponent;
class ENGINE_DLL CGameObject
{
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CGameObject();

public:
	virtual int Update();
	virtual void LateUpdate();
	virtual void Render();

protected:
	virtual HRESULT Initialize();
	virtual HRESULT LateInit();
	virtual void Release();


public:
	void Set_MainCamera(ENGINE::CGameObject* _Camera) { m_pCamera = _Camera; }
	void Set_MapLayer(map<WORD, CLayer*> _mapLayer) { m_mapLayer = _mapLayer; }

public:
	CComponent* Get_Component(wstring _key) { return m_mapComponent[_key]; }
	int GetDead();

public:
	void SetDead();

protected:
	LPDIRECT3DDEVICE9 m_pGraphicDev;

	typedef map<wstring, CComponent*>	MAP_COMPONENT;
	MAP_COMPONENT	m_mapComponent;

	typedef map<WORD, CLayer*>	MAP_LAYER;
	MAP_LAYER	m_mapLayer;

	CGameObject*	m_pCamera;

	bool			m_bIsDead;
	bool			m_bIsInit;
};

END

#define __GAMEOBJECT_H__
#endif