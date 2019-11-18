#include "stdafx.h"
#include "Elevator.h"
#include "Trasform.h"
#include "Collider.h"

CElevator::CElevator(LPDIRECT3DDEVICE9 pGraphicDev)
	:ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pKeyMgr(ENGINE::GetKeyMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr),
	m_eTerrainType(ENGINE::TERRAIN_END),
	m_bIsUp(false)
{
}

CElevator::~CElevator()
{
}

int CElevator::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();

	Move();

	// 임시
	if (m_pKeyMgr->KeyDown(ENGINE::KEY_LCTRL))
		m_bIsUp = !m_bIsUp;

	return NO_EVENT;
}

void CElevator::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
	m_pCollider->LateUpdate(m_pTransform->GetPos());
}

void CElevator::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &(m_pTransform->GetWorldMatrix()));

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//m_pTexture->Render(0);
	m_pBuffer->Render();
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

}

HRESULT CElevator::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(10.f, 2.f, 10.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));
	m_eTerrainType = ENGINE::TERRAIN_CUBE;
	m_fMoveSpeed = 2.f;

	m_pCollider->Set_Radius({ 1.0f , 1.0f, 1.0f });			// 각 축에 해당하는 반지름을 설정
	m_pCollider->Set_Dynamic(false);							// 동적, 정적 Collider 유무
	m_pCollider->Set_Trigger(false);						// 트리거 유무
	m_pCollider->Set_CenterPos(m_pTransform->GetPos());		// Collider 의 정중앙좌표
	m_pCollider->Set_UnderPos();							// Collider 의 하단중앙 좌표
	m_pCollider->SetUp_Box();								// 설정된 것들을 Collider 에 반영합니다.

	return S_OK;
}

HRESULT CElevator::LateInit()
{
	m_vOriPos = m_pTransform->GetPos();

	return S_OK;
}

void CElevator::Release()
{
}

void CElevator::ChangeTex(wstring _wstrTex)
{
	m_wstrTex = _wstrTex;

	// Change Texture component
	m_mapComponent.erase(L"Texture");

	ENGINE::CComponent* pComponent = nullptr;
	pComponent = ENGINE::GetResourceMgr()->CloneResource(ENGINE::RESOURCE_DYNAMIC, _wstrTex);
	NULL_CHECK(pComponent);
	m_mapComponent.insert({ L"Texture", pComponent });

	m_pTexture = dynamic_cast<ENGINE::CTexture*>(pComponent);
	NULL_CHECK(m_pTexture);
}

HRESULT CElevator::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	// Texture
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_DYNAMIC, L"Tile256x256_15");
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Texture", pComponent });

	m_pTexture = dynamic_cast<ENGINE::CTexture*>(pComponent);
	NULL_CHECK_RETURN(m_pTexture, E_FAIL);

	// Buffer
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_DYNAMIC, L"Buffer_CubeCol");
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

void CElevator::Move()
{
	// Up Door
	float fMoveDistY = 3.f;
	if (m_bIsUp && m_pTransform->GetPos().y <= m_vOriPos.y + (fMoveDistY * m_pTransform->GetSize().y))
	{
		D3DXVECTOR3 vMovePos =
		{
			m_pTransform->GetPos().x,
			m_pTransform->GetPos().y + (m_fMoveSpeed * m_pTimeMgr->GetDelta()),
			m_pTransform->GetPos().z };
		m_pTransform->SetPos(vMovePos);
	}
	else if (!m_bIsUp && m_pTransform->GetPos().y > m_vOriPos.y)
	{
		D3DXVECTOR3 vMovePos =
		{
			m_pTransform->GetPos().x,
			m_pTransform->GetPos().y - (m_fMoveSpeed * m_pTimeMgr->GetDelta()),
			m_pTransform->GetPos().z };
		m_pTransform->SetPos(vMovePos);
	}
}

CElevator* CElevator::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CElevator* pInstance = new CElevator(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}
