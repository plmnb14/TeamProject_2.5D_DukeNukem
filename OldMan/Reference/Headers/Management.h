#pragma once

#ifndef __MANAGEMENT_H__

#include "Renderer.h"

BEGIN(ENGINE)

class CComponent;
class CLayer;
class ENGINE_DLL CManagement
{
	DECLARE_SINGLETON(CManagement)

private:
	CManagement();
	~CManagement();

public:
	HRESULT InitManagement(LPDIRECT3DDEVICE9 pGraphicDev);
	void Update();
	void LateUpdate();
	void Render();

public:

private:
	void Release();

public:
	template <typename T>
	HRESULT SceneChange(T& functor)
	{
		Safe_Delete(m_pScene);

		HRESULT hr = functor(&m_pScene, m_pGraphicDev);
		FAILED_CHECK_RETURN(hr, E_FAIL);

		m_pRenderer->SetScene(m_pScene);

		return S_OK;
	}

private:
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	CRenderer*			m_pRenderer;
	CScene*				m_pScene;

	typedef map<WORD, CLayer*>	MAP_LAYER;
	MAP_LAYER	m_mapLayer;
};

END

#define __MANAGEMENT_H__
#endif

