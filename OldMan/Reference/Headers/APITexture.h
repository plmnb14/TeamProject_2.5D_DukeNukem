#pragma once
#ifndef __TEXTURE_H__

#include "Engine_Include.h"
#include "GraphicDev.h"

BEGIN(ENGINE)

class ENGINE_DLL CAPITexture
{
protected:
	CAPITexture();

public:
	virtual ~CAPITexture();

public:
	virtual const TEX_INFO* GetTexInfo(
		const wstring& wstrStateKey = L"",
		int iIndex = 0) PURE;

public:
	virtual HRESULT LoadTexture(
		LPDIRECT3DDEVICE9 pGraphicDev,
		const wstring& wstrFilePath,	/* �ҷ��� �̹��� ��� */
		const wstring& wstrStateKey = L"", /* ��Ƽ �ؽ�ó�� ��� */
		int iImgCount = 0 /* ��Ƽ �ؽ�ó�� ��� */) PURE;

protected:
	virtual void Release() PURE;

protected:
};

END
#define __TEXTURE_H__
#endif
