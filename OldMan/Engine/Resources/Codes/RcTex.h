#pragma once

#ifndef __RCTEX_H__

#include "VIBuffer.h"

BEGIN(ENGINE)

class ENGINE_DLL CRcTex : public CVIBuffer
{
private:
	explicit CRcTex(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CRcTex();

public:
	virtual HRESULT CreateBuffer();
	virtual void Render();

private:
	void Release();

public:
	static CRcTex* Create(LPDIRECT3DDEVICE9 pGraphicDev);
};

END

#define __RCTEX_H__
#endif
