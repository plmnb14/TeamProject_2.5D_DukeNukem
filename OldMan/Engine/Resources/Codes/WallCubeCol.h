#pragma once

#ifndef __WALLCUBECOL_H__

#include "VIBuffer.h"

BEGIN(ENGINE)

class ENGINE_DLL CWallCubeCol : public CVIBuffer
{
private:
	explicit CWallCubeCol(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CWallCubeCol();

public:
	virtual HRESULT CreateBuffer();
	virtual void Render();

private:
	void Release();

public:
	static CWallCubeCol* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	IDirect3DTexture9*		Tex = 0;
};

END

#define __WALLCUBECOL_H__
#endif
