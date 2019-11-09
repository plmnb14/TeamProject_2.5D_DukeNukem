#include "stdafx.h"
#include "TerrainCube.h"
#include "Trasform.h"


CTerrainCube::CTerrainCube(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr)
{
}

CTerrainCube::~CTerrainCube()
{
	Release();
}

void CTerrainCube::Update()
{
	ENGINE::CGameObject::Update();

	KeyInput();
	MouseInput();
}

void CTerrainCube::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
}

void CTerrainCube::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &(m_pTransform->GetWorldMatrix()));
	//m_pTexture->Render(0);
	m_pBuffer->Render();
}

HRESULT CTerrainCube::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));

	return S_OK;
}

void CTerrainCube::Release()
{
}

void CTerrainCube::SetClicked()
{
	m_bSetted = true;
}

HRESULT CTerrainCube::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	// Texture
	//pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, L"Texture_Player");
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent.insert({ L"Texture", pComponent });
	//
	//m_pTexture = dynamic_cast<ENGINE::CTexture*>(pComponent);
	//NULL_CHECK_RETURN(m_pTexture, E_FAIL);

	// Buffer
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, L"Buffer_Player");
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

void CTerrainCube::KeyInput()
{
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

void CTerrainCube::MouseInput()
{
	POINT pt = {};

	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	if (!m_bSetted)
	{
		D3DXVECTOR3 v3 = D3DXVECTOR3((float)pt.x, (float)pt.y, 1.f);

		D3DVIEWPORT9 vp;
		m_pGraphicDev->GetViewport(&vp);

		D3DXMATRIX matProj, matProjInverse, matView;

		m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
		m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

		v3.x = (((v3.x * 2.f) / vp.Width) - 1.f) / matProj(0, 0);
		v3.y = (-((v3.y * 2.f) / vp.Height) + 1.f) / matProj(1, 1);

		/*D3DXMatrixInverse(&matProjInverse, 0, &matView);
		matProjInverse = matProj * matProjInverse;
		D3DXVec3TransformCoord(&v3, &v3, &matProjInverse);*/

		float fTestMul = 15.f;
		D3DXVECTOR3 vPos = D3DXVECTOR3(v3.x * fTestMul, 0, v3.y * fTestMul);
		m_pTransform->SetPos(vPos);
	}
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
