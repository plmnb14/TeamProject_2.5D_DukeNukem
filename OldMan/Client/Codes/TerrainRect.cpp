#include "stdafx.h"
#include "TerrainRect.h"
#include "Trasform.h"
#include "Collider.h"

CTerrainRect::CTerrainRect(LPDIRECT3DDEVICE9 pGraphicDev)
	:CTerrain(pGraphicDev)
{
}

CTerrainRect::~CTerrainRect()
{
}

int CTerrainRect::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();

	return NO_EVENT;
}

void CTerrainRect::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
	m_pCollider->LateUpdate(m_pTransform->GetPos());
	m_pCollider->Set_PlaneVtx(m_pTransform->GetAllAngle() , m_pTransform->GetWorldMatrix());
}

void CTerrainRect::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &(m_pTransform->GetWorldMatrix()));

	//m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pTexture->Render(0);
	m_pBuffer->Render();

	//m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

HRESULT CTerrainRect::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));
	m_eTerrainType = ENGINE::TERRAIN_CUBE;


	// 물리적 콜라이더
	m_pCollider->Set_Radius({ 1.0f , 1.0f, 0.0f });			// 각 축에 해당하는 반지름을 설정
	m_pCollider->Set_Dynamic(false);						// 동적, 정적 Collider 유무
	m_pCollider->Set_Trigger(false);						// 트리거 유무
	m_pCollider->Set_CenterPos(m_pTransform->GetPos());		// Collider 의 정중앙좌표
	m_pCollider->Set_UnderPos();							// Collider 의 하단중앙 좌표
	m_pCollider->SetUp_Box();								// 설정된 것들을 Collider 에 반영합니다.
	m_pCollider->Set_Type(ENGINE::COLLISION_PLANE);

	return S_OK;
}

HRESULT CTerrainRect::LateInit()
{
	return S_OK;
}

void CTerrainRect::Release()
{
}

void CTerrainRect::ChangeTex(wstring _wstrTex)
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

HRESULT CTerrainRect::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	// Texture
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_DYNAMIC, L"Tile256x256_0");
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


	// Terrain 버텍스
	ENGINE::CVIBuffer* pBuffer = dynamic_cast<ENGINE::CVIBuffer*>(this->Get_Component(L"Buffer"));
	m_myVtx = new ENGINE::VTX_TEX[4];
	pBuffer->GetVertexInfo(m_myVtx);
	m_pCollider->Set_CollisionVertex(m_pTransform, m_myVtx);



	return S_OK;
}

CTerrainRect* CTerrainRect::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CTerrainRect* pInstance = new CTerrainRect(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}
