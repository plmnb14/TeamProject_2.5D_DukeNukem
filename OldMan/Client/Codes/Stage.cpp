#include "stdafx.h"
#include "Stage.h"
#include "Player.h"
#include "Terrain.h"
#include "Camera.h"

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

	// Terrain
	pObject = CTerrain::Create(m_pGraphicDev);
	NULL_CHECK_MSG_RETURN(pObject, L"Terrain Create Failed", E_FAIL);
	pObject_Layer->AddObject(ENGINE::OBJECT_TYPE::PROPS, pObject);

	// Camera
	pObject = CCamera::Create(m_pGraphicDev , pObject_Layer->Get_Player());
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

	// Player Texture
	//hr = m_pResourceMgr->AddTexture(
	//	m_pGraphicDev,
	//	ENGINE::RESOURCE_STATIC,
	//	ENGINE::TEX_NORMAL,
	//	L"Texture_Player",
	//	L"../Texture/Player%d.png", 1);
	//FAILED_CHECK_MSG_RETURN(hr, L"Texture_Player Add Failed", E_FAIL);

	// Terrain Texture
	hr = m_pResourceMgr->AddTexture(
		m_pGraphicDev,
		ENGINE::RESOURCE_DYNAMIC,
		ENGINE::TEX_NORMAL,
		L"Texture_Terrain",
		L"../Texture/Terrain/Terrain%d.png", 1);
	FAILED_CHECK_MSG_RETURN(hr, L"Texture_Terrain Add Failed", E_FAIL);

	// Environment Layer
	hr = Add_Environment_Layer();
	FAILED_CHECK_RETURN(hr, E_FAIL);

	// Object Layer
	hr = Add_Object_Layer();
	FAILED_CHECK_RETURN(hr, E_FAIL);

	// UI Layer
	hr = Add_UI_Layer();
	FAILED_CHECK_RETURN(hr, E_FAIL);	

	return S_OK;
}

void CStage::Release()
{

}

void CStage::PipeLineSetUp()
{
	// ���� off
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	// �ĸ� �߷����� off
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// WireFrame
	//pDeviceMgr->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	D3DXMATRIX matView, matProj; // �����, �������

								 // �����(ī�޶��� �����) �����ϴ� �Լ�
	D3DXMatrixLookAtLH(
		&matView, /* ��ȯ ��*/
		&D3DXVECTOR3(0.f, 5.f, -20.f), /* Eye (ī�޶� ��ġ) */
		&D3DXVECTOR3(0.f, 0.f, 0.f), /* At (�ٶ� ��ġ) */
		&D3DXVECTOR3(0.f, 1.f, 0.f) /* Up (������) */);

	// ��ġ���� ����� ����.
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);

	// �������� ��� �����ϴ� �Լ�
	D3DXMatrixPerspectiveFovLH(
		&matProj, /* ��ȯ �� */
		D3DXToRadian(45.f), /* FOV (Field Of View, �þ߰�) */
		WINCX / (float)WINCY, /* Aspect (��Ⱦ��) */
		1.f, /* Near (�����) */
		1000.f /*Far (�����) */);

	// ��ġ���� ���� ��� ����.
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);
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