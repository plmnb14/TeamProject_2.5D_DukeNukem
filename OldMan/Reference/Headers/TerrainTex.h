#pragma once

#ifndef __TERRAINTEX_H__

#include "VIBuffer.h"

BEGIN(ENGINE)

class ENGINE_DLL CTerrainTex : public CVIBuffer
{
private:
	explicit CTerrainTex(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CTerrainTex();

public:
	virtual HRESULT CreateBuffer(
		const WORD& wCntX,
		const WORD& wCntZ,
		const WORD& wItv);
	virtual void Render();

private:
	DWORD* LoadHeightMap();
	void Release();

public:
	static CTerrainTex* Create(
		LPDIRECT3DDEVICE9 pGraphicDev,
		const WORD& wCntX,
		const WORD& wCntZ,
		const WORD& wItv);
};

END

#define __TERRAINTEX_H__
#endif
