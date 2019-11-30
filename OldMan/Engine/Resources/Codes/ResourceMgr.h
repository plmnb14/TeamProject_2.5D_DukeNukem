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
	void GetVertexInfo(
		RESOURCE_TYPE eResourceType,
		const wstring& wstrResourceKey,
		void* pVertex);

public:
	HRESULT AddBuffer(
		LPDIRECT3DDEVICE9 pGraphicDev,
		RESOURCE_TYPE eResourceType,
		CVIBuffer::BUFFER_TYPE eBufferType,
		const wstring& wstrResourceKey,
		const WORD& wCntX = 0, /* 지형배치할 때 가로 버텍스 개수 */
		const WORD& wCntZ = 0, /* 지형배치할 때 세로 버텍스 개수 */
		const WORD& wItv = 1 /* 지형배치할 때 버텍스 간격 */);

	HRESULT AddTexture(
		LPDIRECT3DDEVICE9 pGraphicDev,
		RESOURCE_TYPE eResourceType, 
		TEXTRUE_TYPE eTextureType,
		const wstring& wstrResourceKey,
		const wstring& wstrFilePath,
		const DWORD& dwCnt);

	void Render(const wstring& wstrResourceKey, const int& iIndex = -1);
	void ResetDynamicResource();

	int Get_TextureCount() { return iCount; };
	int Get_MaxTextureCount() { return iMaxCount; };

	// 프로토 타입
	CComponent* CloneResource(
		RESOURCE_TYPE eResourceType,
		const wstring& wstrResourceKey);

private:
	void Release();

private:
	typedef map<wstring, CResources*>	MAP_RESOURCE;
	MAP_RESOURCE	m_mapResource[RESOURCE_END];
	CResources*		m_pResource;

	int iCount;
	int iMaxCount;
};

END

#define __RESOURCEMGR_H__
#endif