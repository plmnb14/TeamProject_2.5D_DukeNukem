#pragma once

#ifndef __LOGO_H__

#include "Scene.h"

namespace ENGINE
{
	class CManagement;
	class CResourceMgr;
}

class CLogo : public ENGINE::CScene
{
private:
	explicit CLogo(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CLogo();

public:	
	virtual void Update();
	virtual void LateUpdate();
	virtual void Render();

private:
	virtual HRESULT Add_Environment_Layer();
	virtual HRESULT Add_Object_Layer();
	virtual HRESULT Add_UI_Layer();
	virtual HRESULT Initialize();
	virtual void Release();

private:
	void PipeLineSetUp();

public:
	static CLogo* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	ENGINE::CManagement*	m_pManagement;
	ENGINE::CResourceMgr*	m_pResourceMgr;
};

#define __LOGO_H__
#endif
