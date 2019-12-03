#include "stdafx.h"
#include "Trigger.h"
#include "Trasform.h"
#include "Collider.h"
#include "Player.h"
#include "Monster.h"
#include "Alien.h"
#include "Trooper.h"
#include "PigMan.h"
#include "OctaBrain.h"
#include "SceneSelector.h"

CTrigger::CTrigger(LPDIRECT3DDEVICE9 pGraphicDev)
	:ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr),
	m_bNextStage(false),
	m_iIndex(0), m_fTriggerTimer(0)
{
}

CTrigger::~CTrigger()
{
}

int CTrigger::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();

	return NO_EVENT;
}

void CTrigger::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
	m_pCollider->LateUpdate(m_pTransform->GetPos());
	CheckTriggerActive();
}

void CTrigger::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &(m_pTransform->GetWorldMatrix()));

	//m_pBuffer->Render();

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

HRESULT CTrigger::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));

	return S_OK;
}

HRESULT CTrigger::LateInit()
{
	D3DXVECTOR3 vSize = m_pTransform->GetSize();

	// 물리적 콜라이더
	m_pCollider->Set_Radius({ 1.0f * vSize.x , 1.0f * vSize.y, 1.0f * vSize.z });			// 각 축에 해당하는 반지름을 설정
	m_pCollider->Set_Dynamic(false);						// 동적, 정적 Collider 유무
	m_pCollider->Set_Trigger(true);							// 트리거 유무
	m_pCollider->Set_CenterPos(m_pTransform->GetPos());		// Collider 의 정중앙좌표
	m_pCollider->Set_UnderPos();							// Collider 의 하단중앙 좌표
	m_pCollider->SetUp_Box();								// 설정된 것들을 Collider 에 반영합니다.
	m_pCollider->Set_Type(ENGINE::COLLISION_AABB);

	return S_OK;
}

void CTrigger::Release()
{

}


HRESULT CTrigger::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	// Buffer
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_DYNAMIC, L"Buffer_CubeTex");
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Buffer", pComponent });

	m_pBuffer = dynamic_cast<ENGINE::CVIBuffer*>(pComponent);
	NULL_CHECK_RETURN(m_pBuffer, E_FAIL);

	// Transform
	pComponent = ENGINE::CTransform::Create(D3DXVECTOR3(0.f, 0.f, 1.f));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Transform", pComponent });

	m_pTransform = dynamic_cast<ENGINE::CTransform*>(pComponent);
	NULL_CHECK_RETURN(m_pTransform, E_FAIL);

	// Collider
	pComponent = ENGINE::CCollider::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Collider", pComponent });

	m_pCollider = dynamic_cast<ENGINE::CCollider*>(pComponent);
	NULL_CHECK_RETURN(m_pCollider, E_FAIL);

	return S_OK;
}

void CTrigger::CheckTriggerActive()
{
	if (m_pCollider->Get_IsCollision())
	{
		switch (m_eTriggerType)
		{
		case CTrigger::TRIGGER_NEXTSTAGE:
		{

			//cout << "Next Stage Trigger ON!!!!!" << endl;

			switch (m_iIndex)
			{
			case 0:
			{
				m_bNextStage = true;
				CPlayer* TmpPlayer = static_cast<CPlayer*>(m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player());
				TmpPlayer->Set_NextStage(true);
				

				//HRESULT hr = ENGINE::GetManagement()->SceneChange(CSceneSelector(CSceneSelector::STAGE_02));
				//FAILED_CHECK_MSG(hr, L"STAGE Scene Change Failed");
			}

			m_bIsDead = true;
			break;
			}
		}
		case CTrigger::TRIGGER_LEDGE:
		{
			CPlayer* TmpPlayer = static_cast<CPlayer*>(m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player());

			TmpPlayer->Set_LedgeVec(m_pTransform->GetPos());

			if (TmpPlayer->Get_CanLedge() == false && TmpPlayer->Get_IsLedge() == false)
			{
				TmpPlayer->Set_CanLedge(true);
			}

			break;
		}
		case CTrigger::TRIGGER_EVENT:
		{
			break;
		}

		case CTrigger::TRIGGER_CAMERA_EVENT:
		{
			break;
		}

		case CTrigger::TRIGGER_ITEM_SPAWNER:
		{
			break;
		}
		case CTrigger::TRIGGER_MONSTER_SPAWNER:
		{
			switch (m_iIndex)
			{
			case 0:
			{
				CGameObject* pObject = CTrooper::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 14,10,9 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CTrooper::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 20,10,-4 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				m_bIsDead = true;
				break;
			}
			case 1:
			{
				CGameObject* pObject = CTrooper::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 160,-2,33 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CTrooper::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 155,2,18 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				m_bIsDead = true;
				break;
			}
			case 2:
			{
				CGameObject* pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 196,38,241 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 192,38,198 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 210,38,171 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 211,38,175 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 213,38,148 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				m_bIsDead = true;
				break;
			}
			case 3:
			{
				CGameObject* pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 74,8,208 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				m_bIsDead = true;
				break;
			}
			case 4:
			{
				CGameObject* pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 54,8,184 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 30,20,181 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 78,0,299 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 71,0,309 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 74,0,326 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 30,0,303 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 32,0,319 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);




				pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 2,0,325 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 10,0,361 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 30,0,371 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 35,0,391 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 34,0,397 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 74,0,388 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);



				pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 118,0,364 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 133,0,331 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 165,8,319 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 132,8,247 });
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				m_bIsDead = true;
				break;
			}

			case 20:
			{
				m_fTriggerTimer += 60 * m_pTimeMgr->GetDelta();

				CGameObject* pObject = nullptr;

				if (int(m_fTriggerTimer) == 20)
				{
					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { -24,-6,12 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 40)
				{
					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { -22,-6,37 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 50)
				{
					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 28,-6,22 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 70)
				{
					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { -53,-6,32 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					m_bIsDead = true;
				}

				break;
			}

			case 21:
			{
				m_fTriggerTimer += 60 * m_pTimeMgr->GetDelta();

				CGameObject* pObject = nullptr;

				if (int(m_fTriggerTimer) == 20)
				{
					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 67,-17,121 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 30)
				{
					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 86,-17,102 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 60)
				{
					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 61,-9,89 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 70)
				{
					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), {41,-17,159 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 100)
				{
					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 41,-33,148 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					m_bIsDead = true;
				}

				break;
			}

			case 22:
			{
				m_fTriggerTimer += 60 * m_pTimeMgr->GetDelta();

				CGameObject* pObject = nullptr;

				if (int(m_fTriggerTimer) == 0)
				{
					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 2,-56,121 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 10)
				{
					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { -29,-54,138 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 0,-54,173 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 60)
				{
					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 42,-35,134 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 22,-35,99 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 90)
				{
					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 34,-59,165 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 24,-58,180 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 25,-50,130 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 150)
				{
					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { -57,-58,116 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { -65,-52,106 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { -88,-44,156 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { -109,-28,147 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 100)
				{
					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 41,-33,148 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					m_bIsDead = true;
				}

				break;
			}

			case 23:
			{
				m_fTriggerTimer += 60 * m_pTimeMgr->GetDelta();

				CGameObject* pObject = nullptr;

				if (int(m_fTriggerTimer) == 10)
				{
					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 29,-47,240 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 30)
				{
					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 17,-35, 226 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					m_bIsDead = true;
				}

				break;
			}

			case 24:
			{
				m_fTriggerTimer += 60 * m_pTimeMgr->GetDelta();

				CGameObject* pObject = nullptr;

				if (int(m_fTriggerTimer) == 10)
				{
					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 48,-49,353 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 20)
				{
					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 30,-49, 364 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 50)
				{
					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 7,-49, 367 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 60)
				{
					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { -16,-46, 348 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 70)
				{
					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { -13,-48, 308 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}


				if (int(m_fTriggerTimer) == 120)
				{
					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { -21,-42, 264 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { 2,-44, 255 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), { -32,-38, 290 });
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				break;
			}

			break;
			}
		case CTrigger::TRIGGER_WAYPOINT:
		{
			break;
		}


		case CTrigger::TRIGGER_END:
		{
			m_bIsDead = true;
			break;
		}
		default:
			break;
		}
		}
	}
}

CTrigger* CTrigger::Create(LPDIRECT3DDEVICE9 pGraphicDev, TRIGGER_TYPE _eType, int iIdx)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CTrigger* pInstance = new CTrigger(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	pInstance->m_eTriggerType = _eType;
	pInstance->m_iIndex = iIdx;

	return pInstance;
}
