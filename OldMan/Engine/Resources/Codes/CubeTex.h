#pragma once

#ifndef __CUBETEX_H__

#include "VIBuffer.h"

BEGIN(ENGINE)

class ENGINE_DLL CCubeTex : public CVIBuffer
{
private:
	explicit CCubeTex(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CCubeTex();

public:
	virtual HRESULT CreateBuffer();
	virtual void Render();

private:
	void Release();

public:
	static CCubeTex* Create(LPDIRECT3DDEVICE9 pGraphicDev);
};

END

#define __CUBETEX_H__
#endif
