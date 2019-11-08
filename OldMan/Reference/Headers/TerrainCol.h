#pragma once

#ifndef __TERRAINCOL_H__

#include "VIBuffer.h"

BEGIN(ENGINE)

class ENGINE_DLL CTerrainCol : public CVIBuffer
{
private:
	explicit CTerrainCol(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CTerrainCol();

public:
	virtual HRESULT CreateBuffer(
		const WORD& wCntX,
		const WORD& wCntZ,
		const WORD& wItv);
	virtual void Render();

private:
	void Release();

public:
	static CTerrainCol* Create(
		LPDIRECT3DDEVICE9 pGraphicDev,
		const WORD& wCntX,
		const WORD& wCntZ,
		const WORD& wItv);
};

END

#define __TERRAINCOL_H__
#endif
