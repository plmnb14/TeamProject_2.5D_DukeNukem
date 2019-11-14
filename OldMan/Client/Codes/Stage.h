#pragma once

#ifndef __STAGE_H__

#include "Scene.h"

namespace ENGINE
{
	class CManagement;
	class CResourceMgr;
}

class CPlayer;
class CTerrain;
class CStage : public ENGINE::CScene
{
private:
	explicit CStage(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CStage();

public:	
	virtual void Update();
	virtual void LateUpdate();
	virtual void Render();

	virtual map<WORD, ENGINE::CLayer*> Get_MapLayer();

private:
	virtual HRESULT Add_Environment_Layer();
	virtual HRESULT Add_Object_Layer();
	virtual HRESULT Add_UI_Layer();
	virtual HRESULT Initialize();
	virtual void Release();

private:
	void PipeLineSetUp();
	void LoadTexture();
	void LoadMapObj();

public:
	static CStage* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	ENGINE::CManagement*	m_pManagement;
	ENGINE::CResourceMgr*	m_pResourceMgr;
};

#define __STAGE_H__
#endif
