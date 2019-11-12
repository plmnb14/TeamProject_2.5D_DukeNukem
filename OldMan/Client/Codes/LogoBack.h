#pragma once

#ifndef __LOGOBACK_H__

#include "GameObject.h"

namespace ENGINE
{
	class CTexture;
	class CVIBuffer;
	class CResourceMgr;
	class CTimeMgr;
}

class CLogoBack : public ENGINE::CGameObject
{
private:
	explicit CLogoBack(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CLogoBack();

public:
	virtual int Update();
	virtual void LateUpdate();
	virtual void Render();

protected:
	virtual HRESULT Initialize();
	virtual void Release();

private:
	HRESULT AddComponent();

public:
	static CLogoBack* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	ENGINE::CTexture*		m_pTexture;
	ENGINE::CVIBuffer*		m_pBuffer;
	ENGINE::CResourceMgr*	m_pResourceMgr;
	ENGINE::CTimeMgr*		m_pTimeMgr;

	float					m_fStartFrame;
	float					m_fMaxFrame;
};

#define __LOGOBACK_H__
#endif