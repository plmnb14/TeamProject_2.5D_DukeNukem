#include "stdafx.h"
#include "ToolTerrainCube.h"
#include "Trasform.h"

#include "Ray.h"


CToolTerrainCube::CToolTerrainCube(LPDIRECT3DDEVICE9 pGraphicDev)
	:CToolTerrain(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr),
	m_bIsPicked(false)
{
}

CToolTerrainCube::~CToolTerrainCube()
{
	Release();
}

int CToolTerrainCube::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::Update();

	//KeyInput();
	MouseInput();

	return NO_EVENT;
}

void CToolTerrainCube::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
}

void CToolTerrainCube::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &(m_pTransform->GetWorldMatrix()));

	//if (m_bIsPicked)
		ENGINE::GetGraphicDev()->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	//m_pTexture->Render(0);
	m_pBuffer->Render();

	//if (m_bIsPicked)
		ENGINE::GetGraphicDev()->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

}

HRESULT CToolTerrainCube::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));

	return S_OK;
}

void CToolTerrainCube::Release()
{
}


HRESULT CToolTerrainCube::AddComponent()
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
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, L"Buffer_CubeCol");
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

void CToolTerrainCube::KeyInput()
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

void CToolTerrainCube::MouseInput()
{
	POINT pt = {};

	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	if (!m_bSetted)
	{
		D3DXVECTOR3 v3 = D3DXVECTOR3((float)pt.x, (float)pt.y, 1.f);

		CRay r = CRay::RayAtWorldSpace((int)v3.x, (int)v3.y);
		
		float fTestMul = 15.f;
		D3DXVECTOR3 vPos = D3DXVECTOR3(r.m_vDirection.x * fTestMul, m_pTransform->GetPos().y, r.m_vDirection.y * fTestMul);
		m_pTransform->SetPos(vPos);
	}
}

CToolTerrainCube* CToolTerrainCube::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CToolTerrainCube* pInstance = new CToolTerrainCube(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}
