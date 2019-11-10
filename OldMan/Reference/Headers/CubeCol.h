#pragma once

#ifndef __CUBECOL_H__

#include "VIBuffer.h"

BEGIN(ENGINE)

class ENGINE_DLL CCubeCol : public CVIBuffer
{
private:
	explicit CCubeCol(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CCubeCol();

public:
	virtual HRESULT CreateBuffer();
	virtual void Render();

private:
	void Release();

public:
	static CCubeCol* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	IDirect3DTexture9*		Tex = 0;
};

END

#define __CUBECOL_H__
#endif
