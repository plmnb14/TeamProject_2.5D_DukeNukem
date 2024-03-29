#include "Layer.h"
#include "GameObject.h"
#include "Camera_Component.h"

USING(ENGINE)

CLayer::CLayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

CLayer::~CLayer()
{
	Release();
}

HRESULT CLayer::AddObject(ENGINE::OBJECT_TYPE _ObjType, CGameObject* pObject)
{
	NULL_CHECK_RETURN(pObject, E_FAIL);
	m_mapGameObject[_ObjType].push_back(pObject);

	return S_OK;
}

CGameObject * CLayer::Get_Target(ENGINE::OBJECT_TYPE _ObjType)
{
	return m_mapGameObject[_ObjType].back();

	//for (auto& piter : m_mapGameObject[_ObjType])
	//{
	//	return piter;
	//}

	return nullptr;
}

CGameObject* CLayer::Get_Player()
{
	return m_mapGameObject[ENGINE::OBJECT_TYPE::PLAYER].back();
}

CGameObject* CLayer::Get_MainCamera()
{
	for (auto& piter : m_mapGameObject[ENGINE::OBJECT_TYPE::CAMERA])
	{
		if (dynamic_cast<CCamera_Component*>(piter->Get_Component(L"Camera"))->Get_MainCamera() == true)
			return piter;
	}

	return nullptr;
}

CGameObject* CLayer::Get_Camera_By_Index(int _Index)
{
	return nullptr;
}

void CLayer::Update()
{
	MAP_GAMEOBJECT::iterator iter_begin = m_mapGameObject.begin();
	MAP_GAMEOBJECT::iterator iter_end = m_mapGameObject.end();

	for (; iter_begin != iter_end; ++iter_begin)
	{
		auto& iter_begin_Obj = iter_begin->second.begin();
		auto& iter_end_Obj = iter_begin->second.end();
		for (; iter_begin_Obj != iter_end_Obj; )
		{
			int iEvent = (*iter_begin_Obj)->Update();
			
			if (DEAD_OBJ == iEvent)
			{
				Safe_Delete(*iter_begin_Obj);
				iter_begin_Obj = iter_begin->second.erase(iter_begin_Obj);
			}
			else
			{
				++iter_begin_Obj;
			}
		}
	}
}

void CLayer::LateUpdate()
{
	MAP_GAMEOBJECT::iterator iter_begin = m_mapGameObject.begin();
	MAP_GAMEOBJECT::iterator iter_end = m_mapGameObject.end();

	for (; iter_begin != iter_end; ++iter_begin)
	{
		for (auto& pObject : iter_begin->second)
			pObject->LateUpdate();
	}
}

void CLayer::Render()
{
	MAP_GAMEOBJECT::iterator iter_begin = m_mapGameObject.begin();
	MAP_GAMEOBJECT::iterator iter_end = m_mapGameObject.end();

	for (; iter_begin != iter_end; ++iter_begin)
	{
		for (auto& pObject : iter_begin->second)
			pObject->Render();
	}
}

void CLayer::Release()
{
	for_each(m_mapGameObject.begin(), m_mapGameObject.end(),
		[](auto& MyPair)
	{
		for_each(MyPair.second.begin(), MyPair.second.end(), Safe_Delete<CGameObject*>);
		MyPair.second.clear();
	});

	m_mapGameObject.clear();

	Safe_Release(m_pGraphicDev);
}

CLayer* CLayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);
	return new CLayer(pGraphicDev);
}
