#pragma once
#ifndef __TEXTUREMGR_H__

#include "Engine_Include.h"

BEGIN(ENGINE)

class CAPITexture;
class ENGINE_DLL CTextureMgr
{
	DECLARE_SINGLETON(CTextureMgr)

public:
	enum TEXTURE_TYPE { SINGLE_TEXTURE, MULTI_TEXTURE };

private:
	CTextureMgr();
	~CTextureMgr();

public:
	const TEX_INFO* GetTexInfo(
		const wstring& wstrObjectKey,
		const wstring& wstrStateKey = L"",
		int iIndex = 0);

	map<wstring, CAPITexture*> GetMapTexture();

public:
	HRESULT LoadTextureFromImgPath(const wstring & wstrImgPath);
	HRESULT LoadTexture(
		TEXTURE_TYPE eTexType,
		const wstring& wstrFilePath,
		const wstring& wstrObjectKey,
		const wstring& wstrStateKey = L"",
		int iImgCount = 0);

private:
	void Release();

private:
	map<wstring, CAPITexture*>				m_mapTexture;

};

END

#define __TEXTUREMGR_H__
#endif