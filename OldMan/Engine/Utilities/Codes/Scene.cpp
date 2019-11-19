#include "Scene.h"
#include "Component.h"

USING(ENGINE)

CScene::CScene(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

CScene::~CScene()
{
	Release();
}


HRESULT CScene::Initialize()
{
	return S_OK;
}

void CScene::Update()
{
	MAP_LAYER::iterator iter_begin = m_mapLayer.begin();
	MAP_LAYER::iterator iter_end = m_mapLayer.end();

	for (; iter_begin != iter_end; ++iter_begin)
		iter_begin->second->Update();
}

void CScene::LateUpdate()
{
	MAP_LAYER::iterator iter_begin = m_mapLayer.begin();
	MAP_LAYER::iterator iter_end = m_mapLayer.end();

	for (; iter_begin != iter_end; ++iter_begin)
		iter_begin->second->LateUpdate();
}

void CScene::Render()
{
	MAP_LAYER::iterator iter_begin = m_mapLayer.begin();
	MAP_LAYER::iterator iter_end = m_mapLayer.end();

	for (; iter_begin != iter_end; ++iter_begin)
		iter_begin->second->Render();
}

void CScene::Release()
{
	for_each(m_mapLayer.begin(), m_mapLayer.end(),
		[](auto& MyPair)
	{
		Safe_Delete(MyPair.second);
	});

	m_mapLayer.clear();

	Safe_Release(m_pGraphicDev);		
}
