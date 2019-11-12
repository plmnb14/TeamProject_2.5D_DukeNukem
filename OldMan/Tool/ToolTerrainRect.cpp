#include "stdafx.h"
#include "TerrainRect.h"
#include "Trasform.h"

#include "Ray.h"

CTerrainRect::CTerrainRect(LPDIRECT3DDEVICE9 pGraphicDev)
	:CToolTerrain(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr),
	m_bIsPicked(false)
{
}

CTerrainRect::~CTerrainRect()
{
	Release();
}

int CTerrainRect::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::Update();

	//KeyInput();
	MouseInput();

	return NO_EVENT;
}

void CTerrainRect::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
}

void CTerrainRect::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &(m_pTransform->GetWorldMatrix()));

	if (m_bIsPicked)
		ENGINE::GetGraphicDev()->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pTexture->Render(0);
	m_pBuffer->Render();

	if (m_bIsPicked)
		ENGINE::GetGraphicDev()->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

}

bool CTerrainRect::CheckGrid(D3DXVECTOR3& _vVtx)
{
	if (!m_bSetted)
		return false;

	POINT pt = {};

	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	D3DXVECTOR3 v3 = D3DXVECTOR3((float)pt.x, (float)pt.y, 1.f);
	CRay r = CRay::RayAtWorldSpace(v3.x, v3.y);

	float fTestMul = 15.f;
	D3DXVECTOR3 vRayPos = D3DXVECTOR3(r.m_vDirection.x * fTestMul, m_pTransform->GetPos().y, r.m_vDirection.y * fTestMul);

	D3DXMATRIX matWorld;
	ENGINE::GetGraphicDev()->GetDevice()->GetTransform(D3DTS_WORLD, &matWorld);
	//D3DXMatrixInverse(&matWorld, 0, &matWorld);

	DWORD dwVtxCount = 0;
	ENGINE::VTX_TEX* pVtx = m_pBuffer->GetVtx(dwVtxCount);
	float fGridRange = 1.f;
	for (int i = 0; i < dwVtxCount; i++)
	{
		D3DXVECTOR3 vVtxPos = pVtx[i].vPos;
		D3DXVECTOR3 vVtxWorldPos;
		D3DXVec3TransformCoord(&vVtxWorldPos, &vVtxPos, &matWorld);
		if ((vVtxWorldPos.x + fGridRange > vRayPos.x && vVtxWorldPos.x - fGridRange < vRayPos.x)
			&& (vVtxWorldPos.y + fGridRange > vRayPos.y && vVtxWorldPos.y - fGridRange < vRayPos.y))
		{
			// 임시 기준 Grid 정함
			_vVtx = D3DXVECTOR3(vVtxWorldPos.x, vVtxWorldPos.y, m_pTransform->GetPos().z );
			return true;
		}
	}

	return false;
}

HRESULT CTerrainRect::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));

	return S_OK;
}

void CTerrainRect::Release()
{
}


HRESULT CTerrainRect::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	//Texture
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, L"Texture_Player");
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Texture", pComponent });

	m_pTexture = dynamic_cast<ENGINE::CTexture*>(pComponent);
	NULL_CHECK_RETURN(m_pTexture, E_FAIL);

	// Buffer
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, L"Buffer_RcTex");
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

void CTerrainRect::KeyInput()
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

void CTerrainRect::MouseInput()
{
	POINT pt = {};

	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	if (!m_bSetted)
	{
		if (m_bIsFitGrid)
			return;

		D3DXVECTOR3 v3 = D3DXVECTOR3((float)pt.x, (float)pt.y, 1.f);

		CRay r = CRay::RayAtWorldSpace(v3.x, v3.y);

		float fTestMul = 15.f;
		D3DXVECTOR3 vPos = D3DXVECTOR3(r.m_vDirection.x * fTestMul, m_pTransform->GetPos().y, r.m_vDirection.y * fTestMul);
		m_pTransform->SetPos(vPos);
	}
	else if (m_bSetted)
	{
		//D3DXVECTOR3 v3 = D3DXVECTOR3((float)pt.x, (float)pt.y, 1.f);

		//D3DXVECTOR3 vPos;
		//CRay r = CRay::RayAtWorldSpace(v3.x, v3.y);
		//if (r.IsPicked(m_pBuffer->GetVtx()[0].vPos, m_pBuffer->GetVtx()[1].vPos, m_pBuffer->GetVtx()[2].vPos, vPos)
		//	|| r.IsPicked(m_pBuffer->GetVtx()[0].vPos, m_pBuffer->GetVtx()[2].vPos, m_pBuffer->GetVtx()[3].vPos, vPos))
		//	m_bIsPicked = true;
		//else
		//	m_bIsPicked = false;
	}
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
