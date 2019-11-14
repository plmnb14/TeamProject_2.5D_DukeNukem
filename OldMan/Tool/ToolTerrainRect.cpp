#include "stdafx.h"
#include "ToolTerrainRect.h"
#include "Trasform.h"

#include "Ray.h"

CToolTerrainRect::CToolTerrainRect(LPDIRECT3DDEVICE9 pGraphicDev)
	:CToolTerrain(pGraphicDev)
{
}

CToolTerrainRect::~CToolTerrainRect()
{
	Release();
}

int CToolTerrainRect::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::Update();

	//KeyInput();
	MouseInput();

	return NO_EVENT;
}

void CToolTerrainRect::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
}

void CToolTerrainRect::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &(m_pTransform->GetWorldMatrix()));

	if (m_bIsPicked)
		ENGINE::GetGraphicDev()->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pTexture->Render(0);
	m_pBuffer->Render();

	if (m_bIsPicked)
		ENGINE::GetGraphicDev()->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

}

bool CToolTerrainRect::CheckGrid(D3DXVECTOR3& _vVtx)
{
	if (!m_bSetted)
		return false;

	POINT pt = {};

	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	D3DXVECTOR3 v3 = CRay::GetDirection();

	float fTestMul = 1.f;
	D3DXVECTOR3 vRayPos = D3DXVECTOR3(v3.x * fTestMul, m_pTransform->GetPos().y, v3.y * fTestMul);
	D3DXMATRIX matWorld;
	ENGINE::GetGraphicDev()->GetDevice()->GetTransform(D3DTS_WORLD, &matWorld);
	D3DXMatrixInverse(&matWorld, 0, &matWorld);
	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);

	//cout << vRayPos.x <<", "<< vRayPos.y<<", "<< vRayPos.z << endl;

	int iVtxCount = 4;
	ENGINE::VTX_TEX* pVtx = new ENGINE::VTX_TEX[iVtxCount];;
	m_pResourceMgr->GetVertexInfo(ENGINE::RESOURCE_STATIC, L"Buffer_RcTex", pVtx);

	float fGridRange = 1.f;
	for (int i = 0; i < iVtxCount; i++)
	{
		D3DXVECTOR3 vVtxPos = pVtx[i].vPos;
		D3DXVECTOR3 vVtxWorldPos;
		//D3DXVec3Cross(&vVtxWorldPos, &vVtxPos, &m_pTransform->GetPos());
		vVtxWorldPos = m_pTransform->GetPos();
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

void CToolTerrainRect::ChangeTex()
{
	// Change Texture
	m_mapComponent.erase(L"Texture");

	ENGINE::CComponent* pComponent = nullptr;
	pComponent = ENGINE::GetResourceMgr()->CloneResource(ENGINE::RESOURCE_DYNAMIC, m_wstrTex);
	NULL_CHECK(pComponent);
	m_mapComponent.insert({ L"Texture", pComponent });

	m_pTexture = dynamic_cast<ENGINE::CTexture*>(pComponent);
	NULL_CHECK(m_pTexture);
}

HRESULT CToolTerrainRect::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));
	m_eTerrainType = ENGINE::TERRAIN_RECT;

	return S_OK;
}

void CToolTerrainRect::Release()
{
}


HRESULT CToolTerrainRect::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	//Texture
	m_wstrTex = L"Tile64x64_0";
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_DYNAMIC, m_wstrTex);
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

void CToolTerrainRect::KeyInput()
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

CToolTerrainRect* CToolTerrainRect::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CToolTerrainRect* pInstance = new CToolTerrainRect(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}
