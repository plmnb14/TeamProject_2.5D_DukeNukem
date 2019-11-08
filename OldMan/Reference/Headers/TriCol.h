#pragma once

#ifndef __TRICOL_H__

#include "VIBuffer.h"

BEGIN(ENGINE)

class ENGINE_DLL CTriCol : public CVIBuffer
{
private:
	explicit CTriCol(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CTriCol();

public:
	virtual HRESULT CreateBuffer();
	virtual void Render();

private:
	void Release();

public:
	static CTriCol* Create(LPDIRECT3DDEVICE9 pGraphicDev);
};

END

#define __TRICOL_H__
#endif
