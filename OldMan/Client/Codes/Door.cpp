#include "stdafx.h"
#include "Door.h"
#include "Trasform.h"
#include "Collider.h"

CDoor::CDoor(LPDIRECT3DDEVICE9 pGraphicDev)
	:ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pKeyMgr(ENGINE::GetKeyMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr),
	m_eTerrainType(ENGINE::TERRAIN_END),
	m_bIsOpened(false), m_pPlayer(nullptr)
{
}

CDoor::~CDoor()
{
	Release();
}

int CDoor::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();

	Move();
	CheckOpen();

	return NO_EVENT;
}

void CDoor::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
}

void CDoor::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &(m_pTransform->GetWorldMatrix()));

	if (m_pTexture) m_pTexture->Render(0);
	else m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pBuffer->Render();

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

HRESULT CDoor::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));
	m_eTerrainType = ENGINE::TERRAIN_RECT;
	m_fMoveSpeed = 5.f;
	m_fMoveDistY = 10.f;
	m_fActiveDist = 3.f;

	return S_OK;
}

HRESULT CDoor::LateInit()
{
	m_pPlayer = m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player();
	m_vOriPos = m_pTransform->GetPos();

	D3DXVECTOR3 vSize = m_pTransform->GetSize();

	// 물리적 콜라이더
	m_pCollider->Set_Radius({ 1.0f * vSize.x , 1.0f * vSize.y, 1.0f * vSize.z });			// 각 축에 해당하는 반지름을 설정
	m_pCollider->Set_Dynamic(false);						// 동적, 정적 Collider 유무
	m_pCollider->Set_Trigger(false);						// 트리거 유무
	m_pCollider->Set_CenterPos(m_pTransform->GetPos());		// Collider 의 정중앙좌표
	m_pCollider->Set_UnderPos();							// Collider 의 하단중앙 좌표
	m_pCollider->SetUp_Box();								// 설정된 것들을 Collider 에 반영합니다.
	m_pCollider->Set_Type(ENGINE::COLLISION_AABB);

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

void CDoor::SetMoveDist(float _fDist)
{
	m_fMoveDistY = _fDist;
}

HRESULT CDoor::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	// Texture
	m_wstrTex = L"Tile256x256_0.dds";
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_DYNAMIC, m_wstrTex);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Texture", pComponent });

	m_pTexture = dynamic_cast<ENGINE::CTexture*>(pComponent);
	NULL_CHECK_RETURN(m_pTexture, E_FAIL);

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

void CDoor::Move()
{
	// Up Door
	if (m_bIsOpened && m_pTransform->GetPos().y <= m_vOriPos.y + (m_fMoveDistY))
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

void CDoor::CheckOpen()
{
	D3DXVECTOR3 vPlayerPos = dynamic_cast<ENGINE::CTransform*>(m_pPlayer->Get_Component(L"Transform"))->GetPos();
	D3DXVECTOR3 vMyPos = m_pTransform->GetPos();
	D3DXVECTOR3 vDist = { vPlayerPos.x - vMyPos.x, vPlayerPos.y - vMyPos.y, vPlayerPos.y - vMyPos.y };
	D3DXVECTOR3 vMySize = m_pTransform->GetSize();

	bool bIsCanOpen = abs(vDist.x) < vMySize.x + m_fActiveDist && abs(vDist.y) < vMySize.y + m_fActiveDist && abs(vDist.z) < vMySize.z + m_fActiveDist;

	if (bIsCanOpen &&
		m_pKeyMgr->KeyDown(ENGINE::KEY_F))
		m_bIsOpened = !m_bIsOpened;
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
