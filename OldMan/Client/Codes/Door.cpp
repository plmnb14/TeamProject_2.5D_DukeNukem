#include "stdafx.h"
#include "Door.h"
#include "Trasform.h"
#include "Collider.h"

CDoor::CDoor(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr),
	m_eTerrainType(ENGINE::TERRAIN_END),
	m_bIsOpened(false)
{
}

CDoor::~CDoor()
{
}

int CDoor::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();

	Move();

	// �ӽ�
	if (GetAsyncKeyState('Z') & 0x8000)
		m_bIsOpened = !m_bIsOpened;

	return NO_EVENT;
}

void CDoor::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
}

void CDoor::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &(m_pTransform->GetWorldMatrix()));

	m_pTexture->Render(0);
	m_pBuffer->Render();
}

HRESULT CDoor::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 2.f, -10.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));
	m_eTerrainType = ENGINE::TERRAIN_RECT;
	m_fMoveSpeed = 3.f;

	m_pCollider->Set_Radius({ 1.0f , 1.0f, 1.0f });			// �� �࿡ �ش��ϴ� �������� ����
	m_pCollider->Set_Dynamic(false);							// ����, ���� Collider ����
	m_pCollider->Set_Trigger(false);						// Ʈ���� ����
	m_pCollider->Set_CenterPos(m_pTransform->GetPos());		// Collider �� ���߾���ǥ
	m_pCollider->Set_UnderPos();							// Collider �� �ϴ��߾� ��ǥ
	m_pCollider->SetUp_Box();								// ������ �͵��� Collider �� �ݿ��մϴ�.

	return S_OK;
}

HRESULT CDoor::LateInit()
{
	m_vOriPos = m_pTransform->GetPos();

	return S_OK;
}

void CDoor::Release()
{
}

void CDoor::ChangeTex(wstring _wstrTex)
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

HRESULT CDoor::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	// Texture
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_DYNAMIC, L"Tile256x256_15");
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Texture", pComponent });

	m_pTexture = dynamic_cast<ENGINE::CTexture*>(pComponent);
	NULL_CHECK_RETURN(m_pTexture, E_FAIL);

	// Buffer
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_DYNAMIC, L"Buffer_RcTex");
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

void CDoor::Move()
{
	// Up Door
	float fMoveDistY = 3.f;
	if (m_bIsOpened && m_pTransform->GetPos().y <= m_vOriPos.y + (fMoveDistY * m_pTransform->GetSize().y))
	{
		D3DXVECTOR3 vMovePos =
		{
			m_pTransform->GetPos().x, 
			m_pTransform->GetPos().y + (m_fMoveSpeed * m_pTimeMgr->GetDelta()), 
			m_pTransform->GetPos().z };
		m_pTransform->SetPos(vMovePos);
	}
	else if(!m_bIsOpened && m_pTransform->GetPos().y > m_vOriPos.y)
	{
		D3DXVECTOR3 vMovePos =
		{
			m_pTransform->GetPos().x,
			m_pTransform->GetPos().y - (m_fMoveSpeed * m_pTimeMgr->GetDelta()),
			m_pTransform->GetPos().z };
		m_pTransform->SetPos(vMovePos);
	}
}

CDoor* CDoor::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CDoor* pInstance = new CDoor(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}