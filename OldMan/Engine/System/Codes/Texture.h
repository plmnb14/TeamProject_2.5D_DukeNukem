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
		const wstring& wstrFilePath,	/* �ҷ��� �̹��� ��� */
		const wstring& wstrStateKey = L"", /* ��Ƽ �ؽ�ó�� ��� */
		int iImgCount = 0 /* ��Ƽ �ؽ�ó�� ��� */) PURE;

protected:
	virtual void Release() PURE;

protected:
	CGraphicDev* m_pGraphicDev;
};

END
#define __TEXTURE_H__
#endif
