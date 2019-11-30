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

public:
	list<ENGINE::PATH_INFO*> GetMapTexture_Multi();
	list<ENGINE::PATH_INFO*> GetMapTexture_Single();

public:
	HRESULT InitTextureMgr(LPDIRECT3DDEVICE9 pGraphicDev);
	HRESULT LoadTextureFromImgPath(const wstring & wstrImgPath);
	HRESULT LoadTexture(
		TEXTURE_TYPE eTexType,
		const wstring& wstrFilePath,
		const wstring& wstrObjectKey,
		const wstring& wstrStateKey = L"",
		int iImgCount = 0);

	int Get_MaxTextureCount() { return iTextureCount; }

private:
	void Release();

private:
	map<wstring, CAPITexture*>	m_mapTexture;
	list<ENGINE::PATH_INFO*>	m_PathInfoLst_Multi;
	list<ENGINE::PATH_INFO*>	m_PathInfoLst_Single;

	LPDIRECT3DDEVICE9			m_pGraphicDev;
	int iTextureCount;
};

END

#define __TEXTUREMGR_H__
#endif