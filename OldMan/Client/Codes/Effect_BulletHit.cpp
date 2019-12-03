#include "stdafx.h"
#include "Effect_BulletHit.h"
#include "Trasform.h"
#include "Animator.h"
#include "CameraObserver.h"
#include "Billborad.h"

CEffect_BulletHit::CEffect_BulletHit(LPDIRECT3DDEVICE9 pGraphicDev)
	:CVfx(pGraphicDev), m_wFrame(0)
{
}

CEffect_BulletHit::~CEffect_BulletHit()
{
	Release();
}

int CEffect_BulletHit::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();

	return NO_EVENT;
}

void CEffect_BulletHit::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();

	D3DXMatrixIdentity(&m_matView);

	D3DXMATRIX Localmatrix, Cameramatrix;													  //  로컬, 카메라 행렬 
	D3DXVECTOR3 vSize;																		  // 대상의 사이즈 
	Localmatrix = m_pTransform->GetWorldMatrix();
	if(m_pObserver != nullptr)
		Cameramatrix = m_pObserver->GetViewMatrix();

	vSize = m_pTransform->GetSize();


	m_pBillborad->Billborad_Front(Localmatrix, Cameramatrix, vSize);                          // 빌보드 설정
	m_matView = m_pBillborad->GetWorldMatrix_Billborad();

	if (m_pAnimator->Get_Frame() >= m_pAnimator->Get_MaxFrame() - 1)
		m_bIsDead = true;
}

void CEffect_BulletHit::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matView);
	m_pTexture->Render(m_pAnimator->RenderSet(m_pTimeMgr->GetDelta()));
	m_pBuffer->Render();
}

HRESULT CEffect_BulletHit::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(3.f, 3.f, 3.f));

	int Frame = dynamic_cast<ENGINE::CResources*>(m_pTexture)->Get_MaxFrame();
	m_pAnimator->Set_MaxFrame(Frame);
	m_pAnimator->Set_FrameAmp(40.f);
	m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_STOP);
	m_pAnimator->Stop_Animation(false);

	return S_OK;
}

HRESULT CEffect_BulletHit::LateInit()
{
	m_pObserver = CCameraObserver::Create();
	NULL_CHECK_RETURN(m_pObserver, E_FAIL);

	m_pSubject->Subscribe(m_pObserver);

	return S_OK;
}

void CEffect_BulletHit::Release()
{
	m_pSubject->UnSubscribe(m_pObserver);
	ENGINE::Safe_Delete(m_pObserver);
}

void CEffect_BulletHit::Set_Pos(D3DXVECTOR3 _Pos)
{
	m_pTransform->SetPos(_Pos);
}

HRESULT CEffect_BulletHit::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	int tmp = rand() % 2;
	wstring wTmp = {};

	if (tmp == 0)
		wTmp = (L"Bullet_Hit_YellowB");

	if (tmp == 1)
		wTmp = (L"Bullet_Hit_YellowS");

	// Texture
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, wTmp);
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

	//빌보드 
	pComponent = ENGINE::CBillborad::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_pBillborad = dynamic_cast<ENGINE::CBillborad*>(pComponent);
	NULL_CHECK_RETURN(m_pBillborad, E_FAIL);
	m_mapComponent.insert({ L"BillBoard", pComponent });

	return S_OK;
}

CEffect_BulletHit* CEffect_BulletHit::Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CEffect_BulletHit* pInstance = new CEffect_BulletHit(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	pInstance->Set_Pos(_Pos);

	return pInstance;
}
