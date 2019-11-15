#include "stdafx.h"
#include "Stage.h"
#include "Player.h"
#include "Terrain.h"
#include "TerrainCube.h"
#include "TerrainWallCube.h"
#include "TerrainRect.h"
#include "Camera.h"
#include "Monster.h"
#include "Trasform.h"

CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CScene(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pManagement(ENGINE::GetManagement())
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

	//// Monster
	//pObject = CMonster::Create(m_pGraphicDev, pObject_Layer->Get_Player());
	//NULL_CHECK_MSG_RETURN(pObject, L"Monster Create Failed", E_FAIL);
	//pObject_Layer->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
	//pObject->Set_MapLayer(m_mapLayer);

	// Terrain
	//pObject = CTerrain::Create(m_pGraphicDev);
	//NULL_CHECK_MSG_RETURN(pObject, L"Terrain Create Failed", E_FAIL);
	//pObject_Layer->AddObject(ENGINE::OBJECT_TYPE::PROPS, pObject);

	// Camera
	pObject = CCamera::Create(m_pGraphicDev, pObject_Layer->Get_Player());
	NULL_CHECK_MSG_RETURN(pObject, L"Terrain Create Failed", E_FAIL);
	pObject_Layer->AddObject(ENGINE::OBJECT_TYPE::CAMERA, pObject);
	pObject_Layer->Get_Player()->Set_MainCamera(pObject_Layer->Get_MainCamera());

	return S_OK;
}

HRESULT CStage::Add_UI_Layer()
{
	return S_OK;
}

HRESULT CStage::Initialize()
{
	PipeLineSetUp();

	// Player Buffer
	HRESULT hr = m_pResourceMgr->AddBuffer(
		m_pGraphicDev,
		ENGINE::RESOURCE_STATIC,
		ENGINE::CVIBuffer::BUFFER_RCTEX,
		L"Buffer_Player");
	FAILED_CHECK_MSG_RETURN(hr, L"Buffer_Player Add Failed", E_FAIL);

	// Terrain Buffer
	hr = m_pResourceMgr->AddBuffer(
		m_pGraphicDev,
		ENGINE::RESOURCE_DYNAMIC,
		ENGINE::CVIBuffer::BUFFER_TERRAINTEX,
		L"Buffer_Terrain",
		TERRAIN_VTX_X, TERRAIN_VTX_Z, TERRAIN_VTX_ITV);
	FAILED_CHECK_MSG_RETURN(hr, L"Buffer_Terrain Add Failed", E_FAIL);

	hr = m_pResourceMgr->AddBuffer(
		m_pGraphicDev,
		ENGINE::RESOURCE_DYNAMIC,
		ENGINE::CVIBuffer::BUFFER_CUBECOL,
		L"Buffer_CubeCol");
	FAILED_CHECK_MSG_RETURN(hr, L"Buffer_CubeCol Add Failed", E_FAIL);

	hr = m_pResourceMgr->AddBuffer(
		m_pGraphicDev,
		ENGINE::RESOURCE_DYNAMIC,
		ENGINE::CVIBuffer::BUFFER_WALLCUBECOL,
		L"Buffer_WallCubeCol");
	FAILED_CHECK_MSG_RETURN(hr, L"Buffer_WallCubeCol Add Failed", E_FAIL);

	hr = m_pResourceMgr->AddBuffer(
		m_pGraphicDev,
		ENGINE::RESOURCE_DYNAMIC,
		ENGINE::CVIBuffer::BUFFER_RCTEX,
		L"Buffer_RcTex");
	FAILED_CHECK_MSG_RETURN(hr, L"Buffer_RcTex Add Failed", E_FAIL);

	LoadTexture();

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

}

void CStage::PipeLineSetUp()
{
	// 조명 off
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	// 후면 추려내기 off
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// WireFrame
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

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

void CStage::LoadTexture()
{
	// 임시.
	HRESULT hr = ENGINE::GetTextureMgr()->LoadTextureFromImgPath(L"../../Data/TexturePath_Client.txt");
	FAILED_CHECK_MSG(hr, L"LoadTextureFromImgPath Failed");

	for (auto& iter : ENGINE::GetTextureMgr()->GetMapTexture_Multi())
	{
		hr = m_pResourceMgr->AddTexture(
			m_pGraphicDev,
			ENGINE::RESOURCE_DYNAMIC,
			ENGINE::TEX_NORMAL,
			iter->wstrStateKey,
			iter->wstrImgPath, iter->iImgCount);
		FAILED_CHECK_MSG(hr, iter->wstrFileName.c_str());
	}

	// Single은 FileName
	for (auto& iter : ENGINE::GetTextureMgr()->GetMapTexture_Single())
	{
		hr = m_pResourceMgr->AddTexture(
			m_pGraphicDev,
			ENGINE::RESOURCE_DYNAMIC,
			ENGINE::TEX_NORMAL,
			iter->wstrFileName,
			iter->wstrImgPath, 1);
		FAILED_CHECK_MSG(hr, iter->wstrFileName.c_str());
	}

	ENGINE::GetTextureMgr()->DestroyInstance();
}

void CStage::LoadMapObj()
{
	HANDLE hFile = CreateFile(L"../../Data/MapObject.dat", GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		FAILED_CHECK_MSG(-1, L"Load Failed. [INVALID_HANDLE_VALUE]");

	DWORD dwByte = 0;
	TCHAR szName[MAX_STR] = L"";
	D3DXVECTOR3 vPos, vSize, vAngle;
	ENGINE::TERRAIN_TYPE eTerrainType;

	while (true)
	{
		::ReadFile(hFile, &szName, sizeof(TCHAR) * MAX_STR, &dwByte, nullptr);
		::ReadFile(hFile, &vPos, sizeof(D3DXVECTOR3), &dwByte, nullptr);
		::ReadFile(hFile, &vSize, sizeof(D3DXVECTOR3), &dwByte, nullptr);
		::ReadFile(hFile, &vAngle, sizeof(D3DXVECTOR3), &dwByte, nullptr);
		::ReadFile(hFile, &eTerrainType, sizeof(ENGINE::TERRAIN_TYPE), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		CTerrain* pTerrain = nullptr;

		switch (eTerrainType)
		{
		case ENGINE::TERRAIN_CUBE:
			pTerrain = CTerrainCube::Create(ENGINE::GetGraphicDev()->GetDevice());
			break;
		case ENGINE::TERRAIN_WALL:
			pTerrain = CTerrainWallCube::Create(ENGINE::GetGraphicDev()->GetDevice());
			break;
		case ENGINE::TERRAIN_RECT:
			pTerrain = CTerrainRect::Create(ENGINE::GetGraphicDev()->GetDevice());
			pTerrain->ChangeTex(szName);
			break;
		}

		ENGINE::CTransform* pTransform = dynamic_cast<ENGINE::CTransform*>(pTerrain->Get_Component(L"Transform"));
		pTransform->SetPos(vPos);
		pTransform->SetSize(vSize);
		pTransform->SetAngle(vAngle.x, ENGINE::ANGLE_X);
		pTransform->SetAngle(vAngle.y, ENGINE::ANGLE_Y);
		pTransform->SetAngle(vAngle.z, ENGINE::ANGLE_Z);

		m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::TERRAIN, pTerrain);
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