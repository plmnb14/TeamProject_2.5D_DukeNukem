#include "GameObject.h"
#include "Component.h"

USING(ENGINE)

CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev) , m_pCamera(nullptr), m_bIsDead(false)
{
	m_pGraphicDev->AddRef();
}

CGameObject::~CGameObject()
{
	Release();
}

int CGameObject::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	MAP_COMPONENT::iterator iter_begin = m_mapComponent.begin();
	MAP_COMPONENT::iterator iter_end = m_mapComponent.end();

	for (; iter_begin != iter_end; ++iter_begin)
		iter_begin->second->Update();

	return NO_EVENT;
}

void CGameObject::LateUpdate()
{
	MAP_COMPONENT::iterator iter_begin = m_mapComponent.begin();
	MAP_COMPONENT::iterator iter_end = m_mapComponent.end();

	for (; iter_begin != iter_end; ++iter_begin)
		iter_begin->second->LateUpdate();
}

void CGameObject::Render()
{
}

HRESULT CGameObject::Initialize()
{
	return S_OK;
}

void CGameObject::Release()
{
	for_each(m_mapComponent.begin(), m_mapComponent.end(),
		[](auto& MyPair)
	{
		Safe_Delete(MyPair.second);
	});

	m_mapComponent.clear();


	Safe_Release(m_pGraphicDev);
}

int CGameObject::GetDead()
{
	return m_bIsDead;
}

void CGameObject::SetDead()
{
	m_bIsDead = true;
}
