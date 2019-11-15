#include "stdafx.h"
#include "TerrainCube.h"
#include "Trasform.h"
#include "Collider.h"

CTerrainCube::CTerrainCube(LPDIRECT3DDEVICE9 pGraphicDev)
	:CTerrain(pGraphicDev)
{
}

CTerrainCube::~CTerrainCube()
{
}

int CTerrainCube::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::Update();

	m_pCollider->Set_UnderPos(m_pTransform->GetPos());
	m_pCollider->SetUp_Box();

	return NO_EVENT;
}

void CTerrainCube::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
}

void CTerrainCube::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &(m_pTransform->GetWorldMatrix()));

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	//m_pTexture->Render(0);
	m_pBuffer->Render();

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

HRESULT CTerrainCube::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));
	m_eTerrainType = ENGINE::TERRAIN_CUBE;

	return S_OK;
}

void CTerrainCube::Release()
{
}

HRESULT CTerrainCube::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	////Texture
	//pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, L"Texture_Player");
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent.insert({ L"Texture", pComponent });
	//
	//m_pTexture = dynamic_cast<ENGINE::CTexture*>(pComponent);
	//NULL_CHECK_RETURN(m_pTexture, E_FAIL);

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

	cout << m_pTransform->GetPos().x << endl;

	m_pCollider->Set_UnderPos(m_pTransform->GetPos());
	m_pCollider->Set_Radius({ 1.f , 1.f , 1.f });
	m_pCollider->Set_CenterPos();
	m_pCollider->Set_Dynamic(false);
	m_pCollider->Set_Trigger(false);
	m_pCollider->SetUp_Box();

	return S_OK;
}

CTerrainCube* CTerrainCube::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CTerrainCube* pInstance = new CTerrainCube(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}
