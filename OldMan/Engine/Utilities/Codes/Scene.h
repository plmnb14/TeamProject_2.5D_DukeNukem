#pragma once

#ifndef __SCENE_H__

#include "Layer.h"

BEGIN(ENGINE)
class ENGINE_DLL CScene
{
protected:
	explicit CScene(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CScene();

public:	
	virtual void Update();
	virtual void LateUpdate();
	virtual void Render();

public:
	virtual map<WORD, ENGINE::CLayer*> Get_MapLayer() PURE;

protected:
	virtual HRESULT Add_Environment_Layer() PURE;
	virtual HRESULT Add_Object_Layer()		PURE;
	virtual HRESULT Add_UI_Layer()			PURE;
	virtual HRESULT Initialize();
	virtual void Release();

protected:
	LPDIRECT3DDEVICE9 m_pGraphicDev;

	typedef map<WORD, CLayer*>	MAP_LAYER;
	MAP_LAYER	m_mapLayer;

};
END

#define __SCENE_H__
#endif
