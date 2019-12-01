#include "stdafx.h"
#include "Stage.h"

#include "Player.h"

#include "Terrain.h"
#include "TerrainCube.h"
#include "TerrainWallCube.h"
#include "TerrainRect.h"

#include "Door.h"
#include "Elevator.h"
#include "Skybox.h"
#include "Trigger.h"
#include "PickupItem.h"

#include "Camera.h"
#include "Monster.h"

#include "UI.h"
#include "Number.h"
#include "Aim.h"
#include "GaugeBar.h"
#include "WeaponIcon.h"
#include "HittedCircle.h"

#include "Trasform.h"
#include "Weapon_Revolver.h"
#include "Weapon_SMG.h"
#include "Weapon_Pump.h"
#include "Weapon_Rocket.h"
#include "Boss_Overload.h"

#include "Player_Hand.h"


CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CScene(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pManagement(ENGINE::GetManagement()),
	m_fTrigger_Index(0), m_Monster_Index(0)
{
}

CStage::~CStage()
{
	Release();
}

void CStage::Update()
{
	ENGINE::CScene::Update();
}

void CStage::LateUpdate()
{
	ENGINE::CScene::LateUpdate();
}

void CStage::Render()
{
	ENGINE::CScene::Render();
}

map<WORD, ENGINE::CLayer*> CStage::Get_MapLayer()
{
	return m_mapLayer;
}

HRESULT CStage::Add_Environment_Layer()
{
	return S_OK;
}

HRESULT CStage::Add_Object_Layer()
{
	// Object Layer
	ENGINE::CLayer* pObject_Layer = ENGINE::CLayer::Create(m_pGraphicDev);
	NULL_CHECK_MSG_RETURN(pObject_Layer, L"Object Layer Create Failed", E_FAIL);
	m_mapLayer.insert({ ENGINE::CLayer::OBJECT, pObject_Layer });

	// Player
	ENGINE::CGameObject* pObject = CPlayer::Create(m_pGraphicDev);
	NULL_CHECK_MSG_RETURN(pObject, L"Player Create Failed", E_FAIL);
	pObject_Layer->AddObject(ENGINE::OBJECT_TYPE::PLAYER, pObject);
	pObject->Set_MapLayer(m_mapLayer);

	ENGINE::CTransform* pTransform = static_cast<ENGINE::CTransform*>(pObject->Get_Component(L"Transform"));
	pTransform->SetPos({8,6,0});

	// Camera
	pObject = CCamera::Create(m_pGraphicDev, pObject_Layer->Get_Player());
	NULL_CHECK_MSG_RETURN(pObject, L"Terrain Create Failed", E_FAIL);
	pObject_Layer->AddObject(ENGINE::OBJECT_TYPE::CAMERA, pObject);
	pObject_Layer->Get_Player()->Set_MainCamera(pObject_Layer->Get_MainCamera());

	// Player_Hand
	pObject = CPlayer_Hand::Create(m_pGraphicDev, pObject_Layer->Get_Player());
	NULL_CHECK_MSG_RETURN(pObject, L"Player Create Failed", E_FAIL);
	pObject_Layer->AddObject(ENGINE::OBJECT_TYPE::UI, pObject);
	pObject->Set_MapLayer(m_mapLayer);

	//// Boss
	//pObject = CBoss_Overload::Create(m_pGraphicDev);
	//NULL_CHECK_MSG_RETURN(pObject, L"Boss Create Failed", E_FAIL);
	//pObject_Layer->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
	//pObject->Set_MapLayer(m_mapLayer);

	//// Revolver
	//pObject = CWeapon_Revolver::Create(m_pGraphicDev, D3DXVECTOR3{ -7,2,8 });
	//NULL_CHECK_MSG_RETURN(pObject, L"Weapon Create Failed", E_FAIL);
	//pObject_Layer->AddObject(ENGINE::OBJECT_TYPE::WEAPON, pObject);
	//pObject->Set_MapLayer(m_mapLayer);
	//
	// SMG
	//pObject = CWeapon_SMG::Create(m_pGraphicDev, D3DXVECTOR3{ 8,0,0 });
	//NULL_CHECK_MSG_RETURN(pObject, L"Weapon Create Failed", E_FAIL);
	//pObject_Layer->AddObject(ENGINE::OBJECT_TYPE::WEAPON, pObject);
	//pObject->Set_MapLayer(m_mapLayer);
	
	//// PumpShotgun
	//pObject = CWeapon_Pump::Create(m_pGraphicDev, D3DXVECTOR3{ -3,2,8 });
	//NULL_CHECK_MSG_RETURN(pObject, L"Weapon Create Failed", E_FAIL);
	//pObject_Layer->AddObject(ENGINE::OBJECT_TYPE::WEAPON, pObject);
	//pObject->Set_MapLayer(m_mapLayer);
	//
	//// RocketLuncher
	//pObject = CWeapon_Rocket::Create(m_pGraphicDev, D3DXVECTOR3{ -1,2,8 });
	//NULL_CHECK_MSG_RETURN(pObject, L"Weapon Create Failed", E_FAIL);
	//pObject_Layer->AddObject(ENGINE::OBJECT_TYPE::WEAPON, pObject);
	//pObject->Set_MapLayer(m_mapLayer);
	
	////// Monster
	//pObject = CMonster::Create(m_pGraphicDev, pObject_Layer->Get_Player());
	//NULL_CHECK_MSG_RETURN(pObject, L"Monster Create Failed", E_FAIL);
	//pObject_Layer->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
	//pObject->Set_MapLayer(m_mapLayer);
	////octabrain
	
	// Skybox
	pObject = CSkybox::Create(m_pGraphicDev, L"skybox_ufo.dds", pObject_Layer->Get_Player());
	NULL_CHECK_MSG_RETURN(pObject, L"Skybox Create Failed", E_FAIL);
	pObject_Layer->AddObject(ENGINE::OBJECT_TYPE::PROPS, pObject);

	//// Camera
	//pObject = CCamera::Create(m_pGraphicDev, pObject_Layer->Get_Player());
	//NULL_CHECK_MSG_RETURN(pObject, L"Terrain Create Failed", E_FAIL);
	//pObject_Layer->AddObject(ENGINE::OBJECT_TYPE::CAMERA, pObject);
	//pObject_Layer->Get_Player()->Set_MainCamera(pObject_Layer->Get_MainCamera());

	return S_OK;
}

HRESULT CStage::Add_UI_Layer()
{
	// Object Layer
	ENGINE::CLayer* pUILayer = ENGINE::CLayer::Create(m_pGraphicDev);
	NULL_CHECK_MSG_RETURN(pUILayer, L"UI Layer Create Failed", E_FAIL);
	m_mapLayer.insert({ ENGINE::CLayer::UI, pUILayer });
	
	// Aim
	ENGINE::CGameObject* pObject = CAim::Create(m_pGraphicDev);
	NULL_CHECK_MSG_RETURN(pObject, L"Aim Create Failed", E_FAIL);
	pUILayer->AddObject(ENGINE::OBJECT_TYPE::UI, pObject);
	pObject->Set_MapLayer(m_mapLayer);

	float fUITop = 280.f;
	float fUIBottom = -330.f;

	// HP Number
	pObject = CNumber::Create(m_pGraphicDev, CNumber::NUMBER_HP);
	NULL_CHECK_MSG_RETURN(pObject, L"NUMBER_HP Create Failed", E_FAIL);
	pUILayer->AddObject(ENGINE::OBJECT_TYPE::UI, pObject);
	pObject->Set_MapLayer(m_mapLayer);
	static_cast<CUI*>(pObject)->SetSize(6.f, 12.f);
	static_cast<CUI*>(pObject)->SetPos(D3DXVECTOR3(-580.f, fUITop, 0.f));

	// Shield Number
	pObject = CNumber::Create(m_pGraphicDev, CNumber::NUMBER_SHIELD);
	NULL_CHECK_MSG_RETURN(pObject, L"NUMBER_SHIELD Create Failed", E_FAIL);
	pUILayer->AddObject(ENGINE::OBJECT_TYPE::UI, pObject);
	pObject->Set_MapLayer(m_mapLayer);
	static_cast<CUI*>(pObject)->SetSize(6.f, 12.f);
	static_cast<CUI*>(pObject)->SetPos(D3DXVECTOR3(-580.f, fUITop + 35.f, 0.f));
	
	// HP Icon
	pObject = CUI::Create(m_pGraphicDev, L"HpIcon.png");
	NULL_CHECK_MSG_RETURN(pObject, L"HpIcon Create Failed", E_FAIL);
	pUILayer->AddObject(ENGINE::OBJECT_TYPE::UI, pObject);
	pObject->Set_MapLayer(m_mapLayer);
	static_cast<CUI*>(pObject)->SetSize(10.f, 10.f);
	static_cast<CUI*>(pObject)->SetPos(D3DXVECTOR3(-555.f, fUITop, 0.f));

	// Shield Icon
	pObject = CUI::Create(m_pGraphicDev, L"ArmorIcon.png");
	NULL_CHECK_MSG_RETURN(pObject, L"ArmorIcon Create Failed", E_FAIL);
	pUILayer->AddObject(ENGINE::OBJECT_TYPE::UI, pObject);
	pObject->Set_MapLayer(m_mapLayer);
	static_cast<CUI*>(pObject)->SetSize(10.f, 13.f);
	static_cast<CUI*>(pObject)->SetPos(D3DXVECTOR3(-555.f, fUITop + 35.f, 0.f));
	
	// Bullet
	pObject = CNumber::Create(m_pGraphicDev, CNumber::NUMBER_BULLET);
	NULL_CHECK_MSG_RETURN(pObject, L"NUMBER_CURBULLET Create Failed", E_FAIL);
	pUILayer->AddObject(ENGINE::OBJECT_TYPE::UI, pObject);
	pObject->Set_MapLayer(m_mapLayer);
	static_cast<CUI*>(pObject)->SetSize(10.f, 15.f);
	static_cast<CUI*>(pObject)->SetPos(D3DXVECTOR3(580.f, fUITop + 50.f, 0.f));

	
	// Grenade
	pObject = CNumber::Create(m_pGraphicDev, CNumber::NUMBER_GRENADE);
	NULL_CHECK_MSG_RETURN(pObject, L"NUMBER_CURBULLET Create Failed", E_FAIL);
	pUILayer->AddObject(ENGINE::OBJECT_TYPE::UI, pObject);
	pObject->Set_MapLayer(m_mapLayer);
	static_cast<CUI*>(pObject)->SetSize(10.f, 15.f);
	static_cast<CUI*>(pObject)->SetPos(D3DXVECTOR3(580.f, fUITop + 100.f, 0.f));

	
	// Buller Line
	pObject = CUI::Create(m_pGraphicDev, L"WeaponRightLine.png");
	NULL_CHECK_MSG_RETURN(pObject, L"WeaponRightLine Create Failed", E_FAIL);
	pUILayer->AddObject(ENGINE::OBJECT_TYPE::UI, pObject);
	pObject->Set_MapLayer(m_mapLayer);
	static_cast<CUI*>(pObject)->SetSize(50.f, 50.f);
	static_cast<CUI*>(pObject)->SetPos(D3DXVECTOR3(600.f, fUITop + 20.f, 0.f));
	
	// HP Bar
	pObject = CGaugeBar::Create(m_pGraphicDev, CGaugeBar::BAR_HP);
	NULL_CHECK_MSG_RETURN(pObject, L"BAR_HP Create Failed", E_FAIL);
	pUILayer->AddObject(ENGINE::OBJECT_TYPE::UI, pObject);
	pObject->Set_MapLayer(m_mapLayer);
	static_cast<CUI*>(pObject)->SetSize(30.f, 30.f);
	static_cast<CUI*>(pObject)->SetPos(D3DXVECTOR3(-525.f, fUITop, 0.f));
	
	// Shield Bar
	pObject = CGaugeBar::Create(m_pGraphicDev, CGaugeBar::BAR_SHIELD);
	NULL_CHECK_MSG_RETURN(pObject, L"BAR_SHIELD Create Failed", E_FAIL);
	pUILayer->AddObject(ENGINE::OBJECT_TYPE::UI, pObject);
	pObject->Set_MapLayer(m_mapLayer);
	static_cast<CUI*>(pObject)->SetSize(30.f, 30.f);
	static_cast<CUI*>(pObject)->SetPos(D3DXVECTOR3(-525.f, fUITop + 35.f, 0.f));
	
	// Weapon Icon
	pObject = CWeaponIcon::Create(m_pGraphicDev);
	NULL_CHECK_MSG_RETURN(pObject, L"Weapon Icon Create Failed", E_FAIL);
	pUILayer->AddObject(ENGINE::OBJECT_TYPE::UI, pObject);
	pObject->Set_MapLayer(m_mapLayer);
	static_cast<CUI*>(pObject)->SetSize(50.f, 50.f);
	static_cast<CUI*>(pObject)->SetPos(D3DXVECTOR3(530.f, fUITop, 0.f));

	return S_OK;
}

HRESULT CStage::Initialize()
{
	PipeLineSetUp();
	
	// Player Buffer
	HRESULT hr = m_pResourceMgr->AddBuffer(
		m_pGraphicDev,
		ENGINE::RESOURCE_DYNAMIC,
		ENGINE::CVIBuffer::BUFFER_RCTEX,
		L"Buffer_Player");
	FAILED_CHECK_MSG_RETURN(hr, L"Buffer_Player Add Failed", E_FAIL);

	// Terrain Buffer


	// 
	hr = m_pResourceMgr->AddBuffer(
		m_pGraphicDev,
		ENGINE::RESOURCE_DYNAMIC,
		ENGINE::CVIBuffer::BUFFER_CUBECOL,
		L"Buffer_CubeCol");
	FAILED_CHECK_MSG_RETURN(hr, L"Buffer_CubeCol Add Failed", E_FAIL);

	//
	hr = m_pResourceMgr->AddBuffer(
		m_pGraphicDev,
		ENGINE::RESOURCE_DYNAMIC,
		ENGINE::CVIBuffer::BUFFER_WALLCUBECOL,
		L"Buffer_WallCubeCol");
	FAILED_CHECK_MSG_RETURN(hr, L"Buffer_WallCubeCol Add Failed", E_FAIL);

	//
	hr = m_pResourceMgr->AddBuffer(
		m_pGraphicDev,
		ENGINE::RESOURCE_DYNAMIC,
		ENGINE::CVIBuffer::BUFFER_CUBETEX,
		L"Buffer_CubeTex");
	FAILED_CHECK_MSG_RETURN(hr, L"Buffer_CubeTex Add Failed", E_FAIL);

	//
	hr = m_pResourceMgr->AddBuffer(
		m_pGraphicDev,
		ENGINE::RESOURCE_DYNAMIC,
		ENGINE::CVIBuffer::BUFFER_RCTEX,
		L"Buffer_RcTex");
	FAILED_CHECK_MSG_RETURN(hr, L"Buffer_RcTex Add Failed", E_FAIL);

	// Environment Layer
	hr = Add_Environment_Layer();
	FAILED_CHECK_RETURN(hr, E_FAIL);

	// Object Layer
	hr = Add_Object_Layer();
	FAILED_CHECK_RETURN(hr, E_FAIL);

	// UI Layer
	hr = Add_UI_Layer();
	FAILED_CHECK_RETURN(hr, E_FAIL);

	LoadMapObj();

	return S_OK;
}

void CStage::Release()
{
	m_pResourceMgr->ResetDynamicResource();
}

void CStage::PipeLineSetUp()
{
	// 조명 off
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	// 후면 추려내기 off
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// WireFrame
	//m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	// 알파테스트 
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x00000088); // 88 ~ 77 ... 등의 값 아래의 알파값은 제외시킴
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	D3DXMATRIX matView, matProj; // 뷰행렬, 투영행렬

								 // 뷰행렬(카메라의 역행렬) 생성하는 함수
	D3DXMatrixLookAtLH(
		&matView, /* 반환 값*/
		&D3DXVECTOR3(0.f, 5.f, -20.f), /* Eye (카메라 위치) */
		&D3DXVECTOR3(0.f, 0.f, 0.f), /* At (바라볼 위치) */
		&D3DXVECTOR3(0.f, 1.f, 0.f) /* Up (업벡터) */);

	// 장치에게 뷰행렬 전달.
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);

	// 원근투영 행렬 생성하는 함수
	D3DXMatrixPerspectiveFovLH(
		&matProj, /* 반환 값 */
		D3DXToRadian(45.f), /* FOV (Field Of View, 시야각) */
		WINCX / (float)WINCY, /* Aspect (종횡비) */
		1.f, /* Near (근평면) */
		1000.f /*Far (원평면) */);

	// 장치에게 투영 행렬 전달.
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);
}

void CStage::LoadMapObj()
{
	HANDLE hFile = CreateFile(L"../../Data/MapObject.dat", GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		FAILED_CHECK_MSG(-1, L"Load Failed. [INVALID_HANDLE_VALUE]");

	DWORD dwByte = 0;
	TCHAR szName[MAX_STR] = L"";
	TCHAR szType[MAX_STR] = L"";
	D3DXVECTOR3 vPos, vSize, vAngle;
	ENGINE::TERRAIN_TYPE eTerrainType;
	int iIndex;
	ENGINE::OBJECT_TYPE	tmpTag = ENGINE::OBJECT_TYPE::OBJ_TYPE_END;

	while (true)
	{
		::ReadFile(hFile, &szName, sizeof(TCHAR) * MAX_STR, &dwByte, nullptr);
		::ReadFile(hFile, &szType, sizeof(TCHAR) * MAX_STR, &dwByte, nullptr);
		::ReadFile(hFile, &vPos, sizeof(D3DXVECTOR3), &dwByte, nullptr);
		::ReadFile(hFile, &vSize, sizeof(D3DXVECTOR3), &dwByte, nullptr);
		::ReadFile(hFile, &vAngle, sizeof(D3DXVECTOR3), &dwByte, nullptr);
		::ReadFile(hFile, &eTerrainType, sizeof(ENGINE::TERRAIN_TYPE), &dwByte, nullptr);
		::ReadFile(hFile, &iIndex, sizeof(int), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		ENGINE::CGameObject* pObject = nullptr;
		ENGINE::OBJECT_TYPE eObjType = ENGINE::OBJECT_TYPE::TERRAIN;

		if (!lstrcmp(szType, L"Terrain"))
		{
			CTerrain* pTerrain = nullptr;

			switch (eTerrainType)
			{
			case ENGINE::TERRAIN_CUBE:
				pTerrain = CTerrainCube::Create(ENGINE::GetGraphicDev()->GetDevice());
				pTerrain->ChangeTex(szName);
				break;
			case ENGINE::TERRAIN_WALL:
				pTerrain = CTerrainWallCube::Create(ENGINE::GetGraphicDev()->GetDevice());
				break;
			case ENGINE::TERRAIN_RECT:
				pTerrain = CTerrainRect::Create(ENGINE::GetGraphicDev()->GetDevice());
				pTerrain->ChangeTex(szName);
				break;
			}

			eObjType = ENGINE::OBJECT_TYPE::TERRAIN;
			tmpTag = ENGINE::OBJECT_TYPE::TERRAIN;
			pObject = pTerrain;
			pTerrain = nullptr;
		}
		else if (!lstrcmp(szType, L"Elevator"))
		{
			CElevator* pElevator = CElevator::Create(ENGINE::GetGraphicDev()->GetDevice());
			pElevator->ChangeTex(szName);

			eObjType = ENGINE::OBJECT_TYPE::TERRAIN;
			pObject = pElevator;
			pElevator = nullptr;
		}
		else if (!lstrcmp(szType, L"Door"))
		{
			CDoor* pDoor = CDoor::Create(ENGINE::GetGraphicDev()->GetDevice());
			pDoor->ChangeTex(szName);

			eObjType = ENGINE::OBJECT_TYPE::TERRAIN;
			pObject = pDoor;
			pDoor = nullptr;
		}
		else if (!lstrcmp(szType, L"Door_Left"))
		{
			CDoor* pDoor = CDoor::Create(ENGINE::GetGraphicDev()->GetDevice(), CDoor::DOOR_LEFT);
			pDoor->ChangeTex(szName);

			eObjType = ENGINE::OBJECT_TYPE::TERRAIN;
			pObject = pDoor;
			pDoor = nullptr;
		}

		else if (!lstrcmp(szType, L"Door_Right"))
		{
			CDoor* pDoor = CDoor::Create(ENGINE::GetGraphicDev()->GetDevice(), CDoor::DOOR_RIGHT);
			pDoor->ChangeTex(szName);

			eObjType = ENGINE::OBJECT_TYPE::TERRAIN;
			pObject = pDoor;
			pDoor = nullptr;
		}

		else if (!lstrcmp(szType, L"Stair"))
		{
			CTerrainCube* pStair = CTerrainCube::Create(ENGINE::GetGraphicDev()->GetDevice());
			pStair->ChangeTex(szName);

			eObjType = ENGINE::OBJECT_TYPE::TERRAIN;
			tmpTag = ENGINE::OBJECT_TYPE::TERRAIN;
			pObject = pStair;
			pStair = nullptr;
		}

		else if (!lstrcmp(szType, L"Box_Wood"))
		{
			CTerrainCube* pStair = CTerrainCube::Create(ENGINE::GetGraphicDev()->GetDevice());
			pStair->ChangeTex(szName);
			eObjType = ENGINE::OBJECT_TYPE::TERRAIN;
			tmpTag = ENGINE::OBJECT_TYPE::WOOD_BOX;
			pObject = pStair;
			pStair = nullptr;
		}

		else if (!lstrcmp(szType, L"Box_Metal"))
		{
			CTerrainCube* pStair = CTerrainCube::Create(ENGINE::GetGraphicDev()->GetDevice());
			pStair->ChangeTex(szName);

			eObjType = ENGINE::OBJECT_TYPE::TERRAIN;
			tmpTag = ENGINE::OBJECT_TYPE::METAL_BOX;
			pObject = pStair;
			pStair = nullptr;
		}
		
		// Monster
		else if (!lstrcmp(szType, L"Pigman"))
		{
			pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player());
			eObjType = ENGINE::OBJECT_TYPE::MONSTER;
			tmpTag = ENGINE::OBJECT_TYPE::MONSTER;
		}
		// Trigger

		else if (!lstrcmp(szType, L"Trigger_ToNextStage"))
		{
			CTrigger* pTrigger = nullptr;
			pTrigger = CTrigger::Create(m_pGraphicDev, CTrigger::TRIGGER_NEXTSTAGE, iIndex);
			eObjType = ENGINE::OBJECT_TYPE::TRIGGER;
			pObject = pTrigger;
			pTrigger = nullptr;
		}

		else if (!lstrcmp(szType, L"Trigger_Ledge"))
		{
			CTrigger* pTrigger = nullptr;
			pTrigger = CTrigger::Create(m_pGraphicDev, CTrigger::TRIGGER_LEDGE, iIndex);
			eObjType = ENGINE::OBJECT_TYPE::TRIGGER;
			pObject = pTrigger;
			pTrigger = nullptr;
		}

		else if (!lstrcmp(szType, L"Trigger_WayPoint"))
		{
			CTrigger* pTrigger = nullptr;
			pTrigger = CTrigger::Create(m_pGraphicDev, CTrigger::TRIGGER_WAYPOINT, iIndex);
			eObjType = ENGINE::OBJECT_TYPE::TRIGGER;
			pObject = pTrigger;
			pTrigger = nullptr;
		}

		else if (!lstrcmp(szType, L"Trigger_Monster_Spawner"))
		{
			CTrigger* pTrigger = nullptr;
			pTrigger = CTrigger::Create(m_pGraphicDev, CTrigger::TRIGGER_MONSTER_SPAWNER, iIndex);
			eObjType = ENGINE::OBJECT_TYPE::TRIGGER;
			pObject = pTrigger;
			pTrigger = nullptr;
		}

		else if (!lstrcmp(szType, L"Trigger_Item_Spawner"))
		{
			CTrigger* pTrigger = nullptr;
			pTrigger = CTrigger::Create(m_pGraphicDev, CTrigger::TRIGGER_ITEM_SPAWNER, iIndex);
			eObjType = ENGINE::OBJECT_TYPE::TRIGGER;
			pObject = pTrigger;
			pTrigger = nullptr;
		}

		else if (!lstrcmp(szType, L"Trigger_Event"))
		{
			CTrigger* pTrigger = nullptr;
			pTrigger = CTrigger::Create(m_pGraphicDev, CTrigger::TRIGGER_EVENT, iIndex);
			eObjType = ENGINE::OBJECT_TYPE::TRIGGER;
			pObject = pTrigger;
			pTrigger = nullptr;
		}

		else if (!lstrcmp(szType, L"Trigger_Camera_Event"))
		{
			CTrigger* pTrigger = nullptr;
			pTrigger = CTrigger::Create(m_pGraphicDev, CTrigger::TRIGGER_CAMERA_EVENT, iIndex);
			eObjType = ENGINE::OBJECT_TYPE::TRIGGER;
			pObject = pTrigger;
			pTrigger = nullptr;
		}

		// Weapon
		else if (!lstrcmp(szType, L"Waepon_0"))
		{
			pObject = CWeapon_Revolver::Create(m_pGraphicDev, D3DXVECTOR3{0, 0, 0});
			eObjType = ENGINE::OBJECT_TYPE::EQUIPMENT;
		}
		else if (!lstrcmp(szType, L"Waepon_1"))
		{
			pObject = CWeapon_SMG::Create(m_pGraphicDev, D3DXVECTOR3{ 0, 0, 0 });
			eObjType = ENGINE::OBJECT_TYPE::EQUIPMENT;
		}
		else if (!lstrcmp(szType, L"Waepon_2"))
		{
			pObject = CWeapon_Pump::Create(m_pGraphicDev, D3DXVECTOR3{ 0, 0, 0 });
			eObjType = ENGINE::OBJECT_TYPE::EQUIPMENT;
		}
		else if (!lstrcmp(szType, L"Waepon_5"))
		{
			pObject = CWeapon_Rocket::Create(m_pGraphicDev, D3DXVECTOR3{ 0, 0, 0 });
			eObjType = ENGINE::OBJECT_TYPE::EQUIPMENT;
		}
		// Item
		else if (!lstrcmp(szType, L"HealPack"))
		{
			pObject = CPickupItem::Create(m_pGraphicDev, CPickupItem::ITEM_HEALPACK);
			eObjType = ENGINE::OBJECT_TYPE::PICKUP;
		}
		else if (!lstrcmp(szType, L"ArmorPack"))
		{
			pObject = CPickupItem::Create(m_pGraphicDev, CPickupItem::ITEM_ARMORPACK);
			eObjType = ENGINE::OBJECT_TYPE::PICKUP;
		}
		else if (!lstrcmp(szType, L"AmmoBox_0"))
		{
			pObject = CPickupItem::Create(m_pGraphicDev, CPickupItem::ITEM_AMMOBOX_LONG);
			eObjType = ENGINE::OBJECT_TYPE::PICKUP;
		}
		else if (!lstrcmp(szType, L"AmmoBox_1"))
		{
			pObject = CPickupItem::Create(m_pGraphicDev, CPickupItem::ITEM_AMMOBOX_SHORT);
			eObjType = ENGINE::OBJECT_TYPE::PICKUP;
		}
		else if (!lstrcmp(szType, L"AmmoBox_2"))
		{
			pObject = CPickupItem::Create(m_pGraphicDev, CPickupItem::ITEM_AMMOBOX_SHOTGUN);
			eObjType = ENGINE::OBJECT_TYPE::PICKUP;
		}
		else if (!lstrcmp(szType, L"AmmoBox_3"))
		{
			pObject = CPickupItem::Create(m_pGraphicDev, CPickupItem::ITEM_AMMOBOX_ROCKET);
			eObjType = ENGINE::OBJECT_TYPE::PICKUP;
		}

		else if (!lstrcmp(szType, L"AmmoBox_4"))
		{
			pObject = CPickupItem::Create(m_pGraphicDev, CPickupItem::ITEM_AMMOBOX_ROCKET);
			eObjType = ENGINE::OBJECT_TYPE::PICKUP;
		}

		if (pObject != nullptr)
		{
			ENGINE::CTransform* pTransform = static_cast<ENGINE::CTransform*>(pObject->Get_Component(L"Transform"));
			pTransform->SetPos(vPos);

			if (eObjType == ENGINE::OBJECT_TYPE::TERRAIN
				|| eObjType == ENGINE::OBJECT_TYPE::STAIR)
			{
				pTransform->SetSize(vSize);
				pTransform->SetAngle(vAngle.x, ENGINE::ANGLE_X);
				pTransform->SetAngle(vAngle.y, ENGINE::ANGLE_Y);
				pTransform->SetAngle(vAngle.z, ENGINE::ANGLE_Z);
			}

			else if (eObjType == ENGINE::OBJECT_TYPE::TRIGGER)
			{
				pTransform->SetSize(vSize);
			}

			m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(eObjType, pObject);
			pObject->Set_MapLayer(m_mapLayer);

			if (tmpTag != ENGINE::OBJECT_TYPE::OBJ_TYPE_END)
			{
				if (tmpTag == ENGINE::OBJECT_TYPE::STAIR)
					pObject->Set_Tag(ENGINE::OBJECT_TYPE::TERRAIN);

				else if (tmpTag == ENGINE::OBJECT_TYPE::METAL_BOX)
					pObject->Set_Tag(ENGINE::OBJECT_TYPE::METAL_BOX);

				else if (tmpTag == ENGINE::OBJECT_TYPE::WOOD_BOX)
					pObject->Set_Tag(ENGINE::OBJECT_TYPE::WOOD_BOX);
			}
		}
	}

	CloseHandle(hFile);
}

CStage* CStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CStage* pInstance = new CStage(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}