#include "stdafx.h"
#include "TerrainCube.h"
#include "Trasform.h"
#include "Collider.h"
#include "Condition.h"
#include "SoundMgr.h"

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
	{
		if (m_eTag == ENGINE::WOOD_BOX)
		{
			int iSound = rand() % 3;

			CSoundMgr::GetInstance()->SetVolume(CSoundMgr::ENVIRONMENT, 5.f);
			CSoundMgr::GetInstance()->StopSound(CSoundMgr::ENVIRONMENT);
			switch (iSound)
			{
			case 0:
				CSoundMgr::GetInstance()->MyPlaySound(L"WoodBox_Break_1.wav", CSoundMgr::ENVIRONMENT);
				break;
			case 1:
				CSoundMgr::GetInstance()->MyPlaySound(L"WoodBox_Break_2.wav", CSoundMgr::ENVIRONMENT);
				break;
			case 2:
				CSoundMgr::GetInstance()->MyPlaySound(L"WoodBox_Break_3.wav", CSoundMgr::ENVIRONMENT);
				break;
			}

			CSoundMgr::GetInstance()->SetVolume(CSoundMgr::ENVIRONMENT, 1.f);
		}

		return DEAD_OBJ;
	}

	if (m_pCollider->Get_IsCollision())
	{
		m_pCollider->Set_IsCollision(false);
	}

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();

	return NO_EVENT;
}

void CTerrainCube::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate(); 
	m_pCollider->LateUpdate(m_pTransform->GetPos());

	if (m_eTag == ENGINE::WOOD_BOX)
	{
		if (m_pCondition != nullptr)
		{
			if (m_pCondition->Get_Hp() <= 0)
			{
				m_bIsDead = true;
			}
		}
	}
}

void CTerrainCube::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &(m_pTransform->GetWorldMatrix()));

	if(m_pTexture) m_pTexture->Render(0);
	else m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pBuffer->Render();

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

void CTerrainCube::ChangeTex(wstring _wstrTex)
{
	m_wstrTex = _wstrTex;

	// Change Texture component
	m_mapComponent.erase(L"Texture");
	m_pTexture = nullptr;

	ENGINE::CComponent* pComponent = nullptr;
	pComponent = ENGINE::GetResourceMgr()->CloneResource(ENGINE::RESOURCE_STATIC, _wstrTex);
	if (!pComponent)
		return;
	m_mapComponent.insert({ L"Texture", pComponent });

	m_pTexture = dynamic_cast<ENGINE::CTexture*>(pComponent);
	NULL_CHECK(m_pTexture);
}

HRESULT CTerrainCube::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	//m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	//m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));
	m_eTerrainType = ENGINE::TERRAIN_CUBE;

	return S_OK;
}

HRESULT CTerrainCube::LateInit()
{
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

void CTerrainCube::Release()
{

}

HRESULT CTerrainCube::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	//Texture
	m_wstrTex = L"Tile256x256_0.dds";
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, m_wstrTex);
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

	pComponent = ENGINE::CCondition::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Condition", pComponent });

	m_pCondition = dynamic_cast<ENGINE::CCondition*>(pComponent);
	NULL_CHECK_RETURN(m_pCondition, E_FAIL);

	m_pCondition->Set_Hp(30);

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
