#pragma once

#ifndef __TEXTURE_H__

#include "Resources.h"

BEGIN(ENGINE)

class ENGINE_DLL CTexture : public CResources
{
private:
	CTexture(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CTexture();

public:
	virtual void Render();
	virtual void Render(const DWORD& dwIndex);

	// ������Ÿ�� ����
	virtual CResources* CloneResource();

private:
	HRESULT LoadTexture(
		TEXTRUE_TYPE eTexType,
		const wstring& wstrFilePath,
		const DWORD& dwCnt);
	void Release();

public:
	static CTexture* Create(
		LPDIRECT3DDEVICE9 pGraphicDev,
		TEXTRUE_TYPE eTexType,
		const wstring& wstrFilePath,
		const DWORD& dwCnt);

private:
	// IDirect3DBaseTexture9: IDirect3DTexture9�� IDirect3DCubeTexture9�� �θ�
	typedef vector<IDirect3DBaseTexture9*>	VEC_TEXTURE;
	VEC_TEXTURE m_vecTexture;
	DWORD		m_dwContainerSize;
};

END

#define __TEXTURE_H__
#endif