#pragma once

#ifndef __RESOURCEMGR_H__

#include "Engine_Include.h"
#include "VIBuffer.h"
#include "Texture.h"

BEGIN(ENGINE)

class ENGINE_DLL CResourceMgr
{
	DECLARE_SINGLETON(CResourceMgr)

private:
	CResourceMgr();
	~CResourceMgr();

public:
	HRESULT AddBuffer(
		LPDIRECT3DDEVICE9 pGraphicDev,
		RESOURCE_TYPE eResourceType,
		CVIBuffer::BUFFER_TYPE eBufferType,
		const wstring& wstrResourceKey,
		const WORD& wCntX = 0, /* ������ġ�� �� ���� ���ؽ� ���� */
		const WORD& wCntZ = 0, /* ������ġ�� �� ���� ���ؽ� ���� */
		const WORD& wItv = 1 /* ������ġ�� �� ���ؽ� ���� */);
	HRESULT AddTexture(
		LPDIRECT3DDEVICE9 pGraphicDev,
		RESOURCE_TYPE eResourceType, 
		TEXTRUE_TYPE eTextureType,
		const wstring& wstrResourceKey,
		const wstring& wstrFilePath,
		const DWORD& dwCnt);
	void Render(const wstring& wstrResourceKey, const int& iIndex = -1);
	void ResetDynamicResource();

	// ������ Ÿ��
	CComponent* CloneResource(
		RESOURCE_TYPE eResourceType,
		const wstring& wstrResourceKey);

	void GetVertexInfo(RESOURCE_TYPE eResourceType, const wstring& wstrResourceKey, void* pVertex);

private:
	void Release();

private:
	typedef map<wstring, CResources*>	MAP_RESOURCE;
	MAP_RESOURCE	m_mapResource[RESOURCE_END];
	CResources*		m_pResource;
};

END

#define __RESOURCEMGR_H__
#endif