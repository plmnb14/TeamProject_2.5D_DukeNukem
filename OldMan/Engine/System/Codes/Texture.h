#pragma once
#ifndef __TEXTURE_H__

#include "Engine_Include.h"
#include "GraphicDev.h"

BEGIN(ENGINE)

class ENGINE_DLL CTexture
{
protected:
	CTexture();

public:
	virtual ~CTexture();

public:
	virtual const TEX_INFO* GetTexInfo(
		const wstring& wstrStateKey = L"",
		int iIndex = 0) PURE;

public:
	virtual HRESULT LoadTexture(
		const wstring& wstrFilePath,	/* 불러올 이미지 경로 */
		const wstring& wstrStateKey = L"", /* 멀티 텍스처인 경우 */
		int iImgCount = 0 /* 멀티 텍스처인 경우 */) PURE;

protected:
	virtual void Release() PURE;

protected:
	CGraphicDev* m_pGraphicDev;
};

END
#define __TEXTURE_H__
#endif
