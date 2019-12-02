#pragma once

#ifndef __LAYER_H__

#include "Engine_Include.h"

BEGIN(ENGINE)

class CGameObject;
class ENGINE_DLL CLayer
{
public:
	enum LAYER_ID
	{
		ENVIRONMENT,
		OBJECT,
		UI
	};

private:
	explicit CLayer(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CLayer();

public:
	CGameObject* Get_SceneChange();
	CGameObject* Get_Target(ENGINE::OBJECT_TYPE _ObjType);
	CGameObject* Get_Player();
	CGameObject* Get_MainCamera();
	CGameObject* Get_Camera_By_Index(int _Index);
	list<CGameObject*> Get_List(ENGINE::OBJECT_TYPE _Type);


public:
	HRESULT AddObject(ENGINE::OBJECT_TYPE _ObjType, CGameObject* pObject);
	void Update();
	void LateUpdate();
	void Render();

private:
	void Release();

public:
	static CLayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	typedef list<CGameObject*>			OBJECT_LIST;
	typedef map<ENGINE::OBJECT_TYPE, OBJECT_LIST>	MAP_GAMEOBJECT;

	MAP_GAMEOBJECT		m_mapGameObject;
	LPDIRECT3DDEVICE9	m_pGraphicDev;
};

END

#define __LAYER_H__
#endif