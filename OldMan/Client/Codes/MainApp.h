#pragma once

#ifndef __MAINAPP_H__

namespace ENGINE
{
	class CGraphicDev;
	class CManagement;	
	class CResourceMgr;
	class CTimeMgr;
}

class CMainApp
{
private:
	explicit CMainApp();

public:
	~CMainApp();

public:
	void Update();
	void LateUpdate();
	void Render();

private:
	HRESULT Initialize();
	void Release();

public:
	static CMainApp* Create();

private:
	ENGINE::CGraphicDev*	m_pDeviceMgr;
	ENGINE::CManagement*	m_pManagement;	
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;
};

#define __MAINAPP_H__
#endif