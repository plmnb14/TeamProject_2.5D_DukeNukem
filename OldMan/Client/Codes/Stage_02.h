#pragma once

#ifndef __STAGE_02_H__

#include "Scene.h"

namespace ENGINE
{
	class CManagement;
	class CResourceMgr;
}

class CPlayer;
class CTerrain;
class CStage_02 : public ENGINE::CScene
{
private:
	explicit CStage_02(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CStage_02();

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
	void LoadMapObj();

public:
	static CStage_02* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	ENGINE::CManagement*	m_pManagement;
	ENGINE::CResourceMgr*	m_pResourceMgr;

	float m_fTrigger_Index;
	float m_Monster_Index;
};

#define __STAGE_02_H__
#endif
