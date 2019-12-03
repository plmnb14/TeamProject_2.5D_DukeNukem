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
#include "Boss_Overload.h"

#include "Summon_Effect.h"

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
				CGameObject* pObject = CSummon_Effect::Create(m_pGraphicDev, { 14,8,5 } , CSummon_Effect::MONSTER);
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CSummon_Effect::Create(m_pGraphicDev, { 20,10,-4 } , CSummon_Effect::MONSTER);
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				m_bIsDead = true;
				break;
			}
			case 1:
			{
				CGameObject* pObject = CSummon_Effect::Create(m_pGraphicDev, { 160,-2,33 } , CSummon_Effect::MONSTER);
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CSummon_Effect::Create(m_pGraphicDev, { 155,2,18 } , CSummon_Effect::MONSTER);
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				m_bIsDead = true;
				break;
			}
			case 2:
			{
				CGameObject* pObject = CSummon_Effect::Create(m_pGraphicDev,  { 196,38,241 } , CSummon_Effect::MONSTER);
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CSummon_Effect::Create(m_pGraphicDev, { 192,38,198 } , CSummon_Effect::MONSTER);
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CSummon_Effect::Create(m_pGraphicDev, { 210,38,171 } , CSummon_Effect::MONSTER);
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CSummon_Effect::Create(m_pGraphicDev, { 211,38,175 } , CSummon_Effect::MONSTER);
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				pObject = CSummon_Effect::Create(m_pGraphicDev,  { 213,38,148 } , CSummon_Effect::MONSTER);
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				m_bIsDead = true;
				break;
			}
			case 3:
			{
				CGameObject* pObject = CSummon_Effect::Create(m_pGraphicDev,  { 74,8,208 } , CSummon_Effect::MONSTER);
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
				pObject->Set_MapLayer(m_mapLayer);

				m_bIsDead = true;
				break;
			}
			case 4:
			{
				cout << "여 오긴 합니다" << endl;

				m_fTriggerTimer += 50 * m_pTimeMgr->GetDelta();

				CGameObject* pObject = nullptr;

				if (int(m_fTriggerTimer) == 20)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 54,8,184 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 30,20,181 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 78,0,299 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 71,0,309 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 50)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 74,0,326 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 30,0,303 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 32,0,319 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}


				if (int(m_fTriggerTimer) == 180)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 2,0,325 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 10,0,361 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 30,0,371 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 210)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 35,0,391 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 34,0,397 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 74,0,388 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}


				if (int(m_fTriggerTimer) == 250)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 118,0,364 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 133,0,331 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 165,8,319 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 132,8,247 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					m_bIsDead = true;
				}

				break;
			}

			case 20:
			{
				m_fTriggerTimer += 50 * m_pTimeMgr->GetDelta();

				CGameObject* pObject = nullptr;

				if (int(m_fTriggerTimer) == 20)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { -24,-6,12 } , CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 40)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { -22,-6,37 } , CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 50)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 28,-6,22 } , CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 70)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { -53,-6,32 } , CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					m_bIsDead = true;
				}

				break;
			}

			case 21:
			{
				m_fTriggerTimer += 50 * m_pTimeMgr->GetDelta();

				CGameObject* pObject = nullptr;

				if (int(m_fTriggerTimer) == 20)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev,  { 67,-17,121 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 30)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev,  { 86,-17,102 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 60)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 61,-9,89 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 70)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, {41,-17,159 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 100)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 41,-33,148 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					m_bIsDead = true;
				}

				break;
			}

			case 22:
			{
				m_fTriggerTimer += 50 * m_pTimeMgr->GetDelta();

				CGameObject* pObject = nullptr;

				if (int(m_fTriggerTimer) == 0)
				{
					cout << "여도 오냐" << endl;

					//pObject = CSummon_Effect::Create(m_pGraphicDev, { 2,-50,121 }, CSummon_Effect::OVERLOAD);
					//m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					//pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 10)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev,{ -29,-54,138 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 0,-54,173 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 60)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev,{ 42,-35,134 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev,  { 22,-35,99 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 90)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 34,-59,165 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 24,-58,180 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 25,-50,130 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 150)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { -57,-58,116 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev,  { -65,-52,106 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { -88,-44,156 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev,  { -109,-28,147 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 100)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev,  { 41,-33,148 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					m_bIsDead = true;
				}

				break;
			}

			case 23:
			{
				m_fTriggerTimer += 50 * m_pTimeMgr->GetDelta();

				CGameObject* pObject = nullptr;

				if (int(m_fTriggerTimer) == 10)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev,{ 29,-47,240 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 30)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 17,-35, 226 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					m_bIsDead = true;
				}

				break;
			}

			case 24:
			{
				m_fTriggerTimer += 50 * m_pTimeMgr->GetDelta();

				CGameObject* pObject = nullptr;

				if (int(m_fTriggerTimer) == 10)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 48,-49,353 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 20)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 30,-49, 364 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 50)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev,  { 7,-49, 367 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 60)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev,  { -16,-46, 348 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 70)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { -13,-48, 308 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}


				if (int(m_fTriggerTimer) == 120)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { -21,-42, 264 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 2,-44, 255 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { -32,-38, 290 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					m_bIsDead = true;
				}

				break;
			}

			case 25:
			{
				m_fTriggerTimer += 50 * m_pTimeMgr->GetDelta();

				CGameObject* pObject = nullptr;

				if (int(m_fTriggerTimer) == 10)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 16,-44,476 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 50)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev,{ -12,-34, 481 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 60)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev,{ 50,-24, 472 }, CSummon_Effect::MONSTER);
					pObject->Set_MapLayer(m_mapLayer);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
				}

				if (int(m_fTriggerTimer) == 110)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev,  { -13,-48, 308 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}


				if (int(m_fTriggerTimer) == 120)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { -21,-42, 264 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 2,-44, 255 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev,{ -32,-38, 290 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				break;
			}

			case 26:
			{
				m_fTriggerTimer += 50 * m_pTimeMgr->GetDelta();

				CGameObject* pObject = nullptr;

				if (int(m_fTriggerTimer) == 30)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 22,-24,588 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 40)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 51,-18, 562 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 60)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 44,-30, 596 }, CSummon_Effect::MONSTER);
					pObject->Set_MapLayer(m_mapLayer);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
				}

				if (int(m_fTriggerTimer) == 80)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 63,-42, 601 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}


				if (int(m_fTriggerTimer) == 120)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { -26,-48, 577 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 53,-48, 606 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					m_bIsDead = true;
				}

				break;
			}

			case 27:
			{
				m_fTriggerTimer += 50 * m_pTimeMgr->GetDelta();

				CGameObject* pObject = nullptr;

				if (int(m_fTriggerTimer) == 0)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 147,-29,631 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 40)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 120,-23, 601 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 50)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 138,-35, 596 }, CSummon_Effect::MONSTER);
					pObject->Set_MapLayer(m_mapLayer);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);

					m_bIsDead = true;
				}

				if (int(m_fTriggerTimer) == 10)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 121,-35, 596 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				break;
			}

			case 28:
			{
				m_fTriggerTimer += 50 * m_pTimeMgr->GetDelta();

				CGameObject* pObject = nullptr;

				if (int(m_fTriggerTimer) == 0)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 230,-49,685 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 238,-49,677 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 20)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 245,-49, 691 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 253,-49, 675 }, CSummon_Effect::MONSTER);
					pObject->Set_MapLayer(m_mapLayer);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
				}

				if (int(m_fTriggerTimer) == 70)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 255,-31, 678 }, CSummon_Effect::MONSTER);
					pObject->Set_MapLayer(m_mapLayer);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 286,-31, 702 }, CSummon_Effect::MONSTER);
					pObject->Set_MapLayer(m_mapLayer);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);

					m_bIsDead = true;
				}

				break;
			}

			case 29:
			{
				m_fTriggerTimer += 50 * m_pTimeMgr->GetDelta();

				CGameObject* pObject = nullptr;

				if (int(m_fTriggerTimer) == 0)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 299,-43,930 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 60)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 310,-48, 871 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 301,-48, 876 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 287,-48, 873 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 90)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 289,-48, 860 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 304,-48, 852 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 294,-48, 851 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 286,-48, 846 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 150)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 309,-48, 870 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 298,-48, 866 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 286,-48, 873 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 210)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 297,-48, 708 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 288,-48, 715 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 311,-48, 727 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 230)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 323,-36, 802 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 313,-32, 825 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 260)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 273,-38, 851 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 304,-24, 861 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

				}

				if (int(m_fTriggerTimer) == 290)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 288,-36, 785 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					pObject = CSummon_Effect::Create(m_pGraphicDev, { 311,-30, 920 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 300)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 284,-34, 920 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					m_bIsDead = true;
				}

				break;
			}

			case 30:
			{
				m_fTriggerTimer += 60 * m_pTimeMgr->GetDelta();

				CGameObject* pObject = nullptr;

				if (int(m_fTriggerTimer) == 0)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 288,-48,1020 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);
				}

				if (int(m_fTriggerTimer) == 20)
				{
					pObject = CSummon_Effect::Create(m_pGraphicDev, { 313,-48, 1008 }, CSummon_Effect::MONSTER);
					m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::VFX, pObject);
					pObject->Set_MapLayer(m_mapLayer);

					m_bIsDead = true;
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
