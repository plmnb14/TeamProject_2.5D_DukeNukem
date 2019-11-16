#include "Management.h"
#include "Renderer.h"
#include "Scene.h"
#include "CollisionMgr.h"

USING(ENGINE)

IMPLEMENT_SINGLETON(CManagement)

CManagement::CManagement()
	: m_pGraphicDev(nullptr), m_pRenderer(nullptr), m_pScene(nullptr)
{
}


CManagement::~CManagement()
{
	Release();
}

HRESULT CManagement::InitManagement(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, E_FAIL);

	m_pGraphicDev = pGraphicDev;
	m_pGraphicDev->AddRef();

	m_pRenderer = CRenderer::Create(pGraphicDev);
	NULL_CHECK_MSG_RETURN(m_pRenderer, L"Renderer Create Failed", E_FAIL);

	return S_OK;
}

void CManagement::Update()
{
	NULL_CHECK(m_pScene);
	m_pScene->Update();
}

void CManagement::LateUpdate()
{
	NULL_CHECK(m_pScene);
	m_pScene->LateUpdate();

	m_mapLayer = m_pScene->Get_MapLayer();

	CCollisionMgr::CollisionSphere(MAP_OBJ(ENGINE::OBJECT_TYPE::PLAYER), MAP_OBJ(ENGINE::OBJECT_TYPE::MONSTER));
	CCollisionMgr::CollisionSphere(MAP_OBJ(ENGINE::OBJECT_TYPE::PLAYER), MAP_OBJ(ENGINE::OBJECT_TYPE::TERRAIN));
	CCollisionMgr::CollisionSphere(MAP_OBJ(ENGINE::OBJECT_TYPE::MONSTER), MAP_OBJ(ENGINE::OBJECT_TYPE::TERRAIN));

}

void CManagement::Render()
{
	NULL_CHECK(m_pRenderer);
	m_pRenderer->Render();
}

void CManagement::Release()
{
	Safe_Delete(m_pScene);
	Safe_Delete(m_pRenderer);
	Safe_Release(m_pGraphicDev);
	
}
