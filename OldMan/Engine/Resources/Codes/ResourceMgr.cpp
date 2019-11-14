#include "ResourceMgr.h"
#include "TriCol.h"
#include "RcCol.h"
#include "RcTex.h"
#include "TerrainCol.h"
#include "TerrainTex.h"
#include "CubeCol.h"
#include "WallCubeCol.h"

USING(ENGINE)

IMPLEMENT_SINGLETON(CResourceMgr)

CResourceMgr::CResourceMgr()
	: m_pResource(nullptr)
{

}

CResourceMgr::~CResourceMgr()
{
	Release();
}

void CResourceMgr::GetVertexInfo(
	RESOURCE_TYPE eResourceType,
	const wstring& wstrResourceKey,
	void* pVertex)
{
	auto iter_find = m_mapResource[eResourceType].find(wstrResourceKey);

	if (m_mapResource[eResourceType].end() == iter_find)
		return;

	dynamic_cast<CVIBuffer*>(iter_find->second)->GetVertexInfo(pVertex);
}

HRESULT CResourceMgr::AddBuffer(
	LPDIRECT3DDEVICE9 pGraphicDev, 
	RESOURCE_TYPE eResourceType, 
	CVIBuffer::BUFFER_TYPE eBufferType,
	const wstring& wstrResourceKey, 
	const WORD& wCntX, 
	const WORD& wCntZ, 
	const WORD& wItv)
{
	auto iter_find = m_mapResource[eResourceType].find(wstrResourceKey);

	if (m_mapResource[eResourceType].end() != iter_find)
	{
		ERR_MSG(L"키가 중복됩니다.");
		return E_FAIL;
	}

	CResources* pResource = nullptr;

	switch (eBufferType)
	{
	case CVIBuffer::BUFFER_TRICOL:
		pResource = CTriCol::Create(pGraphicDev);
		break;
	case CVIBuffer::BUFFER_RCCOL:
		pResource = CRcCol::Create(pGraphicDev);
		break;
	case CVIBuffer::BUFFER_RCTEX:
		pResource = CRcTex::Create(pGraphicDev);
		break;
	case CVIBuffer::BUFFER_TERRAIN:
		pResource = CTerrainCol::Create(pGraphicDev, wCntX, wCntZ, wItv);
		break;
	case CVIBuffer::BUFFER_TERRAINTEX:
		pResource = CTerrainTex::Create(pGraphicDev, wCntX, wCntZ, wItv);
		break;
	case CVIBuffer::BUFFER_CUBECOL:
		pResource = CCubeCol::Create(pGraphicDev);
		break;
	case CVIBuffer::BUFFER_WALLCUBECOL:
		pResource = CWallCubeCol::Create(pGraphicDev);
		break;
	}

	NULL_CHECK_RETURN(pResource, E_FAIL);
	m_mapResource[eResourceType][wstrResourceKey] = pResource;

	return S_OK;
}

HRESULT CResourceMgr::AddTexture(
	LPDIRECT3DDEVICE9 pGraphicDev, 
	RESOURCE_TYPE eResourceType, 
	TEXTRUE_TYPE eTextureType, 
	const wstring& wstrResourceKey, 
	const wstring& wstrFilePath, 
	const DWORD& dwCnt)
{
	auto iter_find = m_mapResource[eResourceType].find(wstrResourceKey);

	if (m_mapResource[eResourceType].end() != iter_find)
	{
		ERR_MSG(L"키가 중복됩니다.");
		return E_FAIL;
	}

	CResources* pResource = CTexture::Create(
		pGraphicDev, eTextureType, wstrFilePath, dwCnt);

	NULL_CHECK_RETURN(pResource, E_FAIL);
	m_mapResource[eResourceType][wstrResourceKey] = pResource;

	return S_OK;
}

void CResourceMgr::Render(const wstring& wstrResourceKey, const int& iIndex/* = -1*/)
{
	auto iter_find = m_mapResource[RESOURCE_STATIC].find(wstrResourceKey);

	if (m_mapResource[RESOURCE_STATIC].end() == iter_find)
		return;

	switch (iIndex)
	{
	case -1:
		iter_find->second->Render();
		break;
	default:
		dynamic_cast<CTexture*>(iter_find->second)->Render((DWORD)iIndex);
		break;
	}
	
}

void CResourceMgr::ResetDynamicResource()
{
	for (auto& MyPair : m_mapResource[RESOURCE_DYNAMIC])
		Safe_Delete(MyPair.second);

	m_mapResource[RESOURCE_DYNAMIC].clear();
}

CComponent* CResourceMgr::CloneResource(
	RESOURCE_TYPE eResourceType, 
	const wstring& wstrResourceKey)
{
	auto iter_find = m_mapResource[eResourceType].find(wstrResourceKey);

	if(m_mapResource[eResourceType].end() == iter_find)
		return nullptr;

	return iter_find->second->CloneResource();
}

void CResourceMgr::Release()
{
	for (int i = 0; i < RESOURCE_END; ++i)
	{
		for (auto& MyPair : m_mapResource[i])
			Safe_Delete(MyPair.second);

		m_mapResource[i].clear();
	}
}
