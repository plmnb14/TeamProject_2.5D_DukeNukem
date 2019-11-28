#pragma once

#ifndef __MAINAPP_H__

namespace ENGINE
{
	class CGraphicDev;
	class CManagement;	
	class CResourceMgr;
	class CTimeMgr;
}

class CSoundMgr;
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

public:
	void FrameRender();

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
	ENGINE::CKeyMgr*		m_pKeyMgr;
	ENGINE::CTextureMgr*	m_pTextureMgr;

private:
	float	m_fTimeCount;
	int		m_iFps;
};

#define __MAINAPP_H__
#endif