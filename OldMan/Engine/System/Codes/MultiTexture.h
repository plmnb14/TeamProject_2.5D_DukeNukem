#pragma once
#ifndef __MULTITEXTURE_H__
#include "APITexture.h"

BEGIN(ENGINE)
class CMultiTexture :
	public CAPITexture
{
private:
	CMultiTexture();

public:
	virtual ~CMultiTexture();

public:
	// CTexture을(를) 통해 상속됨
	virtual const TEX_INFO * GetTexInfo(
		const wstring& wstrStateKey = L"",
		int iIndex = 0) override;

public:
	virtual HRESULT LoadTexture(
		LPDIRECT3DDEVICE9 pGraphicDev,
		const wstring& wstrFilePath,
		const wstring& wstrStateKey = L"",
		int iImgCount = 0) override;

private:
	virtual void Release() override;

public:
	static CMultiTexture* Create(
		LPDIRECT3DDEVICE9 pGraphicDev,
		const wstring& wstrFilePath,
		const wstring& wstrStateKey,
		int iImgCount);

private:
	map<wstring, vector<TEX_INFO*>>		m_mapMultiTexture;
};
END

#define __MULTITEXTURE_H__
#endif
