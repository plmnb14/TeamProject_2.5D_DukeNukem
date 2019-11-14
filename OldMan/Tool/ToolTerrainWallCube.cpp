#include "stdafx.h"
#include "ToolTerrainWallCube.h"
#include "Trasform.h"

#include "Ray.h"


CToolTerrainWallCube::CToolTerrainWallCube(LPDIRECT3DDEVICE9 pGraphicDev)
	:CToolTerrain(pGraphicDev)
{
}

CToolTerrainWallCube::~CToolTerrainWallCube()
{
	Release();
}

int CToolTerrainWallCube::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::Update();

	//KeyInput();
	MouseInput();

	return NO_EVENT;
}

void CToolTerrainWallCube::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
}

void CToolTerrainWallCube::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &(m_pTransform->GetWorldMatrix()));

	//if (m_bIsPicked)
	ENGINE::GetGraphicDev()->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	//m_pTexture->Render(0);
	m_pBuffer->Render();

	//if (m_bIsPicked)
	ENGINE::GetGraphicDev()->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

}

HRESULT CToolTerrainWallCube::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));
	m_eTerrainType = ENGINE::TERRAIN_WALL;

	return S_OK;
}

void CToolTerrainWallCube::Release()
{
}


HRESULT CToolTerrainWallCube::AddComponent()
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
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, L"Buffer_WallCubeCol");
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

	return S_OK;
}

void CToolTerrainWallCube::KeyInput()
{
	if (m_bSetted)
		return;

	float fMoveSpeed = 5.f * m_pTimeMgr->GetDelta();
	float fAngleSpeed = 90.f * m_pTimeMgr->GetDelta();

	if (GetAsyncKeyState(VK_UP) & 0x8000)
		m_pTransform->MovePos(fMoveSpeed);
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		m_pTransform->MovePos(-fMoveSpeed);
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		m_pTransform->MoveAngle(ENGINE::ANGLE_Y, -fAngleSpeed);
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		m_pTransform->MoveAngle(ENGINE::ANGLE_Y, fAngleSpeed);
}

CToolTerrainWallCube* CToolTerrainWallCube::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CToolTerrainWallCube* pInstance = new CToolTerrainWallCube(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}
