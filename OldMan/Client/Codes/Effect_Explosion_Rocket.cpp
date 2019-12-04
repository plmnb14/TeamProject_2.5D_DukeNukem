#include "stdafx.h"
#include "Effect_Explosion_Rocket.h"
#include "Condition.h"
#include "Trasform.h"
#include "Animator.h"
#include "CameraObserver.h"
#include "Billborad.h"
#include "Collider.h"
#include "SoundMgr.h"

CEffect_Explosion_Rocket::CEffect_Explosion_Rocket(LPDIRECT3DDEVICE9 pGraphicDev)
	:CVfx(pGraphicDev), m_wFrame(0), m_pBombCollider(nullptr), m_pCondition(nullptr)
{
}

CEffect_Explosion_Rocket::~CEffect_Explosion_Rocket()
{
	Release();
}

int CEffect_Explosion_Rocket::Update()
{
	if (m_bIsDead)
	{
		return DEAD_OBJ;
	}

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();

	return NO_EVENT;
}

void CEffect_Explosion_Rocket::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();

	D3DXMatrixIdentity(&m_matView);

	D3DXMATRIX Localmatrix, Cameramatrix;													  //  로컬, 카메라 행렬 
	D3DXVECTOR3 vSize;																		  // 대상의 사이즈 
	Localmatrix = m_pTransform->GetWorldMatrix();
	Cameramatrix = m_pObserver->GetViewMatrix();

	vSize = m_pTransform->GetSize();


	m_pBillborad->Billborad_Front(Localmatrix, Cameramatrix, vSize);                          // 빌보드 설정
	m_matView = m_pBillborad->GetWorldMatrix_Billborad();

	if (m_pAnimator->Get_Frame() >= m_pAnimator->Get_MaxFrame() - 1)
		m_bIsDead = true;

	m_pBombCollider->LateUpdate(m_pTransform->GetPos());
}

void CEffect_Explosion_Rocket::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matView);
	m_pTexture->Render(m_pAnimator->RenderSet(m_pTimeMgr->GetDelta()));
	m_pBuffer->Render();
}

HRESULT CEffect_Explosion_Rocket::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(20.f, 20.f, 20.f));

	int Frame = dynamic_cast<ENGINE::CResources*>(m_pTexture)->Get_MaxFrame();
	m_pAnimator->Set_MaxFrame(Frame);
	m_pAnimator->Set_FrameAmp(40.f);
	m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_STOP);
	m_pAnimator->Stop_Animation(false);

	m_pBombCollider->Set_Radius({ 15.f , 15.f, 15.f });			// 각 축에 해당하는 반지름을 설정
	m_pBombCollider->Set_Dynamic(false);						// 동적, 정적 Collider 유무
	m_pBombCollider->Set_Trigger(true);						// 트리거 유무
	m_pBombCollider->Set_CenterPos(m_pTransform->GetPos());		// Collider 의 정중앙좌표
	m_pBombCollider->Set_UnderPos();							// Collider 의 하단중앙 좌표
	m_pBombCollider->SetUp_Box();								// 설정된 것들을 Collider 에 반영합니다.
	m_pBombCollider->Set_Type(ENGINE::COLLISION_AABB);

	return S_OK;
}

HRESULT CEffect_Explosion_Rocket::LateInit()
{
	m_pObserver = CCameraObserver::Create();
	NULL_CHECK_RETURN(m_pObserver, E_FAIL);

	m_pSubject->Subscribe(m_pObserver);

	return S_OK;
}

void CEffect_Explosion_Rocket::Release()
{
	m_pSubject->UnSubscribe(m_pObserver);
	ENGINE::Safe_Delete(m_pObserver);
}

void CEffect_Explosion_Rocket::Set_Pos(D3DXVECTOR3 _Pos)
{
	m_pTransform->SetPos(_Pos);
}

void CEffect_Explosion_Rocket::Set_Damage(float _Damage)
{
	m_pCondition->Set_Damage(_Damage);
}

HRESULT CEffect_Explosion_Rocket::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	// Texture
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, L"Explosion_Rocket");
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

	// Collider
	pComponent = ENGINE::CCollider::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"BombCollider", pComponent });

	m_pBombCollider = static_cast<ENGINE::CCollider*>(pComponent);
	NULL_CHECK_RETURN(m_pBombCollider, E_FAIL);

	// conditoin  
	pComponent = ENGINE::CCondition::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Condition", pComponent });
	
	m_pCondition = static_cast<ENGINE::CCondition*>(pComponent);
	NULL_CHECK_RETURN(m_pCondition, E_FAIL);

	return S_OK;
}

CEffect_Explosion_Rocket* CEffect_Explosion_Rocket::Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos , float _Attack)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CEffect_Explosion_Rocket* pInstance = new CEffect_Explosion_Rocket(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	pInstance->Set_Pos(_Pos);
	pInstance->Set_Damage(_Attack);

	return pInstance;
}
