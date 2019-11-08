#pragma once

#ifndef __GAMEOBJECT_H__

#include "Engine_Include.h"

BEGIN(ENGINE)

class CComponent;
class ENGINE_DLL CGameObject
{
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CGameObject();

public:
	virtual void Update();
	virtual void LateUpdate();
	virtual void Render();

protected:
	virtual HRESULT Initialize();
	virtual void Release();

public:
	CComponent* Get_Component(wstring _key) { return m_mapComponent[_key]; }

protected:
	LPDIRECT3DDEVICE9 m_pGraphicDev;

	typedef map<wstring, CComponent*>	MAP_COMPONENT;
	MAP_COMPONENT	m_mapComponent;
};

END

#define __GAMEOBJECT_H__
#endif