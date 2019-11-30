#include "stdafx.h"
#include "Trigger.h"
#include "Trasform.h"
#include "Collider.h"
#include "Player.h"

CTrigger::CTrigger(LPDIRECT3DDEVICE9 pGraphicDev)
	:ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr),
	m_iIndex(0)
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
	CheckTriggerActive();

	return NO_EVENT;
}

void CTrigger::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
	m_pCollider->LateUpdate(m_pTransform->GetPos());
}

void CTrigger::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &(m_pTransform->GetWorldMatrix()));

	m_pBuffer->Render();

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
			cout << "Next Stage Trigger ON!!!!!" << endl;
			m_bIsDead = true;
			break;
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
			break;
		}
		case CTrigger::TRIGGER_WAYPOINT:
		{
			cout << "웨이포인트 번호 : " << m_iIndex << endl;

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
