#include "stdafx.h"
#include "PickupItem.h"
#include "Trasform.h"
#include "Collider.h"
#include "Condition.h"
#include "Billborad.h"
#include "Player.h"
#include "CameraObserver.h"
#include "CameraSubject.h"

CPickupItem::CPickupItem(LPDIRECT3DDEVICE9 pGraphicDev)
	:ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr),
	m_pSubject(ENGINE::GetCameraSubject())
{
}

CPickupItem::~CPickupItem()
{
	Release();
}

int CPickupItem::Update()
{
	if (m_bIsDead)
	{
		Hitted();
		return DEAD_OBJ;
	}

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();

	return NO_EVENT;
}

void CPickupItem::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
	m_pCollider->LateUpdate(m_pTransform->GetPos());

	D3DXMATRIX Localmatrix, Cameramatrix;													  //  로컬, 카메라 행렬 
	D3DXVECTOR3 vSize;																		  // 대상의 사이즈 
	Localmatrix = m_pTransform->GetWorldMatrix();
	Cameramatrix = m_pObserver->GetViewMatrix();
	vSize = m_pTransform->GetSize();

	m_pBillborad->Billborad_Front(Localmatrix, Cameramatrix, vSize);                          // 빌보드 설정
	m_matView = m_pBillborad->GetWorldMatrix_Billborad();                                    // 빌보드에서 설정된 행렬을 받아온다. 

}

void CPickupItem::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matView);

	m_pTexture->Render(0);
	m_pBuffer->Render();

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

HRESULT CPickupItem::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));

	return S_OK;
}

HRESULT CPickupItem::LateInit()
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

	m_pObserver = CCameraObserver::Create();
	NULL_CHECK_RETURN(m_pObserver, E_FAIL);

	m_pSubject->Subscribe(m_pObserver);

	return S_OK;
}

void CPickupItem::Release()
{
	m_pSubject->UnSubscribe(m_pObserver);
	ENGINE::Safe_Delete(m_pObserver);
}

void CPickupItem::ChangeTex(wstring _wstrTex)
{
	// Change Texture component
	m_mapComponent.erase(L"Texture");

	ENGINE::CComponent* pComponent = nullptr;
	pComponent = ENGINE::GetResourceMgr()->CloneResource(ENGINE::RESOURCE_DYNAMIC, _wstrTex);
	NULL_CHECK(pComponent);
	m_mapComponent.insert({ L"Texture", pComponent });

	m_pTexture = dynamic_cast<ENGINE::CTexture*>(pComponent);
	NULL_CHECK(m_pTexture);
}

HRESULT CPickupItem::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	// Texture
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_DYNAMIC, L"Tile256x256_0.png");
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

	// Billboard
	pComponent = ENGINE::CBillborad::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_pBillborad = dynamic_cast<ENGINE::CBillborad*>(pComponent);
	NULL_CHECK_RETURN(m_pBillborad, E_FAIL);
	m_mapComponent.insert({ L"BillBoard", pComponent });

	return S_OK;
}

void CPickupItem::Hitted()
{
	int iAmount = 0;

	switch (m_eItemType)
	{
	case CPickupItem::ITEM_HEALPACK:
	{
		iAmount = 30;

		ENGINE::CCondition* pPlayerCondition = dynamic_cast<ENGINE::CCondition*>(m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player()->Get_Component(L"Condition"));

		pPlayerCondition->Set_Hp(pPlayerCondition->Get_Hp() + iAmount);
		if (pPlayerCondition->Get_Hp() > 100)
			pPlayerCondition->Set_Hp(100);

		break;
	}
	case CPickupItem::ITEM_ARMORPACK:
	{
		iAmount = 30;

		ENGINE::CCondition* pPlayerCondition = dynamic_cast<ENGINE::CCondition*>(m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player()->Get_Component(L"Condition"));

		pPlayerCondition->Set_Armor(pPlayerCondition->Get_Armor() + iAmount);
		if (pPlayerCondition->Get_Armor() > 100)
			pPlayerCondition->Set_Armor(100);
		break;
	}
	case CPickupItem::ITEM_AMMOBOX_SHORT: // for SMG, Revolver
	{
		iAmount = 30;

		CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player());

		ENGINE::W_INFO* pEquipInfo = pPlayer->Get_WInfo();
		if (pEquipInfo->eWeaponTag == ENGINE::WEAPON_TAG::SMG ||
			pEquipInfo->eWeaponTag == ENGINE::WEAPON_TAG::REVOLVER)
		{
			pEquipInfo->wCurBullet += iAmount;
			if (pEquipInfo->wMaxBullet < pEquipInfo->wCurBullet)
				pEquipInfo->wCurBullet = pEquipInfo->wMaxBullet;
			pPlayer->Set_WeaponInfo(pEquipInfo);
		}

		ENGINE::W_INFO* pWInfo = pPlayer->Get_WInfo(ENGINE::WEAPON_TAG::SMG);
		if (pWInfo)
		{
			pWInfo->wCurBullet += iAmount;
			if (pWInfo->wMaxBullet < pWInfo->wCurBullet)
				pWInfo->wCurBullet = pWInfo->wMaxBullet;
			pPlayer->Set_WeaponInfo(ENGINE::WEAPON_TAG::SMG, pWInfo);
		}

		pWInfo = pPlayer->Get_WInfo(ENGINE::WEAPON_TAG::REVOLVER);
		if (pWInfo)
		{
			pWInfo->wCurBullet += iAmount;
			if (pWInfo->wMaxBullet < pWInfo->wCurBullet)
				pWInfo->wCurBullet = pWInfo->wMaxBullet;
			pPlayer->Set_WeaponInfo(ENGINE::WEAPON_TAG::REVOLVER, pWInfo);
		}

		break;
	}
	case CPickupItem::ITEM_AMMOBOX_LONG: // for Rifle, Sniper Rifle
	{
		//iAmount = 30;

		//CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player());
		//ENGINE::W_INFO* pWInfo = pPlayer->Get_WInfo(ENGINE::WEAPON_TAG::RIFLE);
		//pWInfo->wCurBullet += iAmount;
		//if (pWInfo->wMaxBullet < pWInfo->wCurBullet)
		//	pWInfo->wCurBullet = pWInfo->wMaxBullet;
		//pPlayer->Set_WeaponInfo(pWInfo);
		break;
	}
	case CPickupItem::ITEM_AMMOBOX_SHOTGUN:
	{
		iAmount = 30;

		CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player());

		ENGINE::W_INFO* pEquipInfo = pPlayer->Get_WInfo();
		if (pEquipInfo->eWeaponTag == ENGINE::WEAPON_TAG::SHOTGUN)
		{
			pEquipInfo->wCurBullet += iAmount;
			if (pEquipInfo->wMaxBullet < pEquipInfo->wCurBullet)
				pEquipInfo->wCurBullet = pEquipInfo->wMaxBullet;
			pPlayer->Set_WeaponInfo(pEquipInfo);
		}

		ENGINE::W_INFO* pWInfo = pPlayer->Get_WInfo(ENGINE::WEAPON_TAG::SHOTGUN);
		if (pWInfo)
		{
			pWInfo->wCurBullet += iAmount;
			if (pWInfo->wMaxBullet < pWInfo->wCurBullet)
				pWInfo->wCurBullet = pWInfo->wMaxBullet;
			pPlayer->Set_WeaponInfo(ENGINE::WEAPON_TAG::SHOTGUN, pWInfo);
		}
		break;
	}
	case CPickupItem::ITEM_AMMOBOX_ROCKET:
	{
		iAmount = 30;

		CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player());
		
		ENGINE::W_INFO* pEquipInfo = pPlayer->Get_WInfo();
		if (pEquipInfo->eWeaponTag == ENGINE::WEAPON_TAG::LUNCHER)
		{
			ENGINE::W_INFO* pEquipInfo = pPlayer->Get_WInfo();
			pEquipInfo->wCurBullet += iAmount;
			if (pEquipInfo->wMaxBullet < pEquipInfo->wCurBullet)
				pEquipInfo->wCurBullet = pEquipInfo->wMaxBullet;
			pPlayer->Set_WeaponInfo(pEquipInfo);
		}

		ENGINE::W_INFO* pWInfo = pPlayer->Get_WInfo(ENGINE::WEAPON_TAG::LUNCHER);
		if (pWInfo)
		{
			pWInfo->wCurBullet += iAmount;
			if (pWInfo->wMaxBullet < pWInfo->wCurBullet)
				pWInfo->wCurBullet = pWInfo->wMaxBullet;
			pPlayer->Set_WeaponInfo(ENGINE::WEAPON_TAG::LUNCHER, pWInfo);
		}
		break;
	}
	}
}


CPickupItem* CPickupItem::Create(LPDIRECT3DDEVICE9 pGraphicDev, ITEM_TYPE _eType)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CPickupItem* pInstance = new CPickupItem(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	pInstance->m_eItemType = _eType;

	switch (_eType)
	{
	case CPickupItem::ITEM_HEALPACK:
		pInstance->ChangeTex(L"Pack_2.png");
		break;
	case CPickupItem::ITEM_ARMORPACK:
		pInstance->ChangeTex(L"Pack_1.png");
		break;
	case CPickupItem::ITEM_AMMOBOX_SHORT:
		pInstance->ChangeTex(L"AmmoBox_1.png");
		break;
	case CPickupItem::ITEM_AMMOBOX_LONG:
		pInstance->ChangeTex(L"AmmoBox_0.png");
		break;
	case CPickupItem::ITEM_AMMOBOX_SHOTGUN:
		pInstance->ChangeTex(L"AmmoBox_2.png");
		break;
	case CPickupItem::ITEM_AMMOBOX_ROCKET:
		pInstance->ChangeTex(L"AmmoBox_3.png");
		break;
	}

	return pInstance;
}
