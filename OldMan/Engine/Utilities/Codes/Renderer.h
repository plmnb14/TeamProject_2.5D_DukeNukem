#pragma once

#ifndef __RENDERER_H__

#include "Engine_Include.h"

BEGIN(ENGINE)

class CScene;
class ENGINE_DLL CRenderer
{
private:
	explicit CRenderer(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	~CRenderer();

public:
	void SetScene(CScene* pScene);

public:
	void Render();

private:
	HRESULT Initialize();
	void Release();

public:
	static CRenderer* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	CScene*				m_pScene;
};
END

#define __RENDERER_H__
#endif