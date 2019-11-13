#pragma once
#ifndef __SINGLETEXTURE_H__

#include "APITexture.h"

BEGIN(ENGINE)
class CSingleTexture :
	public CAPITexture
{
private:
	CSingleTexture();

public:
	virtual ~CSingleTexture();

public:
	virtual const TEX_INFO* GetTexInfo(
		const wstring& wstrStateKey = L"",
		int iIndex = 0);

public:
	// CTexture��(��) ���� ��ӵ�
	virtual HRESULT LoadTexture(
		const wstring & wstrFilePath,
		const wstring & wstrStateKey = L"",
		int iImgCount = 0) override;

private:
	virtual void Release() override;

public:
	static CSingleTexture* Create(const wstring & wstrFilePath);

private:
	TEX_INFO*	m_pTexInfo;
};
END

#define __SINGLETEXTURE_H__
#endif
