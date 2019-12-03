#include "stdafx.h"
#include "FlameBall.h"
#include "Trasform.h"
#include "Animator.h"
#include "CameraObserver.h"
#include "Billborad.h"

CEffect_FlameBall::CEffect_FlameBall(LPDIRECT3DDEVICE9 pGraphicDev)
	:CVfx(pGraphicDev), m_wFrame(0), m_fLifetime(0)
{
}

CEffect_FlameBall::~CEffect_FlameBall()
{
	Release();
}

int CEffect_FlameBall::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();

	return NO_EVENT;
}

void CEffect_FlameBall::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();

	D3DXMatrixIdentity(&m_matView);

	D3DXMATRIX Localmatrix, Cameramatrix;													  //  ����, ī�޶� ��� 
	D3DXVECTOR3 vSize;																		  // ����� ������ 
	Localmatrix = m_pTransform->GetWorldMatrix();
	Cameramatrix = m_pObserver->GetViewMatrix();

	vSize = m_pTransform->GetSize();


	m_pBillborad->Billborad_Front(Localmatrix, Cameramatrix, vSize);                          // ������ ����
	m_matView = m_pBillborad->GetWorldMatrix_Billborad();

	m_fLifetime = m_pTimeMgr->GetDelta();

	D3DXVECTOR3 vTmpSize = m_pTransform->GetSize();

	if (vTmpSize.x < 0.1f)
		m_bIsDead = true;

	m_pTransform->SetSize(D3DXVECTOR3(vTmpSize.x - m_fLifetime, vTmpSize.y - m_fLifetime, vTmpSize.z - m_fLifetime));
}

void CEffect_FlameBall::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matView);
	m_pTexture->Render(m_pAnimator->RenderSet(m_pTimeMgr->GetDelta()));
	m_pBuffer->Render();
}

HRESULT CEffect_FlameBall::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));

	int Frame = dynamic_cast<ENGINE::CResources*>(m_pTexture)->Get_MaxFrame();
	m_pAnimator->Set_MaxFrame(Frame);
	m_pAnimator->Set_FrameAmp(40.f);
	m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_STOP);
	m_pAnimator->Stop_Animation(false);

	return S_OK;
}

HRESULT CEffect_FlameBall::LateInit()
{
	m_pObserver = CCameraObserver::Create();
	NULL_CHECK_RETURN(m_pObserver, E_FAIL);

	m_pSubject->Subscribe(m_pObserver);

	return S_OK;
}

void CEffect_FlameBall::Release()
{
	m_pSubject->UnSubscribe(m_pObserver);
	ENGINE::Safe_Delete(m_pObserver);
}

void CEffect_FlameBall::Set_Pos(D3DXVECTOR3 _Pos)
{
	m_pTransform->SetPos(_Pos);
}

void CEffect_FlameBall::Set_Size(D3DXVECTOR3 _Size)
{
	m_pTransform->SetSize(_Size);
}

HRESULT CEffect_FlameBall::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	// Texture
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, L"Smoke_Gun");
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

	// Animator
	pComponent = ENGINE::CAnimator::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Animator", pComponent });

	m_pAnimator = dynamic_cast<ENGINE::CAnimator*>(pComponent);
	NULL_CHECK_RETURN(m_pAnimator, E_FAIL);

	//������ 
	pComponent = ENGINE::CBillborad::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_pBillborad = dynamic_cast<ENGINE::CBillborad*>(pComponent);
	NULL_CHECK_RETURN(m_pBillborad, E_FAIL);
	m_mapComponent.insert({ L"BillBoard", pComponent });

	return S_OK;
}

CEffect_FlameBall* CEffect_FlameBall::Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos, D3DXVECTOR3 _Size)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CEffect_FlameBall* pInstance = new CEffect_FlameBall(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	pInstance->Set_Pos(_Pos);
	pInstance->Set_Size(_Size);

	return pInstance;
}