#include "stdafx.h"
#include "Player_Hand.h"
#include "Trasform.h"
#include "Collider.h"
#include "RigidBody.h"
#include "CameraObserver.h"
#include "CameraSubject.h"
#include "Billborad.h"
#include "Player.h"
#include "Condition.h"
#include "Animator.h"

CPlayer_Hand::CPlayer_Hand(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTransform(nullptr), m_pAnimator(nullptr),
	m_pTexture(nullptr), m_pCameraObserver(nullptr), m_pBuffer(nullptr),
	m_pTarget(nullptr),
	m_pCameraSubject(ENGINE::GetCameraSubject()),
	m_fSizeY(0), m_fSizeX(0), m_fFrame(0),
	m_eActState(CPlayer::W_NONE)
{

}

CPlayer_Hand::~CPlayer_Hand()
{
}

int CPlayer_Hand::Update()
{
	Set_WeaponAct();

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();

	return NO_EVENT;
}

void CPlayer_Hand::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();	

	//if (static_cast<CPlayer*>(m_pTarget)->Get_Zoom() == true)
	//{
	//	ChangeTex(L"SMG_Zoom", 1);
	//
	//	if ((int)m_fFrame < 3)
	//		m_fFrame += 20 * m_pTimeMgr->GetDelta();
	//}
	//
	//else if(static_cast<CPlayer*>(m_pTarget)->Get_Zoom() == false)
	//{
	//	if (static_cast<CPlayer*>(m_pTarget)->Get_WInfo()->fDelayTimer > 0)
	//	{
	//		if ((int)m_fFrame > 2)
	//			m_fFrame = 0;
	//
	//		if((int)m_fFrame < 2)
	//			m_fFrame += 40 * m_pTimeMgr->GetDelta();
	//	}
	//
	//	else if (static_cast<CPlayer*>(m_pTarget)->Get_WInfo()->fDelayTimer <= 0)
	//	{
	//		m_fFrame = 0;
	//	}
	//
	//	ChangeTex(L"SMG_Fire", 1);
	//}
}

void CPlayer_Hand::Render()
{
	//m_pGraphicDev->SetTransform(D3DTS_WORLD, &(m_pTransform->GetWorldMatrix()));
	////m_pTexture->Render(0);
	//m_pBuffer->Render();

	//if (!m_bVisible)
	//	return;

	// Set Proj BeforRender ==========================================================
	D3DXMATRIX matWorld, matView, matProj, matTempView, matTempProj;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixIdentity(&matView);
	D3DXMatrixIdentity(&matProj);
	D3DXMatrixIdentity(&matTempView);
	D3DXMatrixIdentity(&matTempProj);

	// Get Temp
	matTempView = m_pCameraObserver->GetViewMatrix();
	matTempProj = m_pCameraObserver->GetProjMatrix();
	matProj = m_pCameraObserver->GetProjMatrix();

	// 직교투영
	D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.f, 1.f);

	// Set UI Size
	float fScale[3];
	fScale[0] = m_pTransform->GetSize().x * m_fSizeX;
	fScale[1] = m_pTransform->GetSize().y * m_fSizeY;
	fScale[2] = 1.f;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
			matView(i, j) *= fScale[i];
	}

	// Set UI Pos
	matView._41 = m_pTransform->GetPos().x;
	matView._42 = m_pTransform->GetPos().y;
	matView._43 = m_pTransform->GetPos().z;

	// Set Device Ortho Transform
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);

	// ===============================================================================
	// Render
	m_pTexture->Render((int)m_fFrame);
	m_pBuffer->Render();

	// Set Proj AfterRender ==========================================================
	// Set Device Original Transform
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matTempView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matTempProj);
}

HRESULT CPlayer_Hand::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	// 트랜스폼 세팅
	m_pTransform->SetPos(D3DXVECTOR3(5.f, 285.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(13.f, 13.f, 13.f));

	m_fSizeX = 50.f;
	m_fSizeY = 50.f;

	//ChangeTex(L"Zoom" , 1);

	return S_OK;
}

HRESULT CPlayer_Hand::LateInit()
{
	m_pCameraObserver = CCameraObserver::Create();
	NULL_CHECK_RETURN(m_pCameraObserver, E_FAIL);

	m_pCameraSubject->Subscribe(m_pCameraObserver);

	return S_OK;
}

void CPlayer_Hand::Release()
{
	m_pCameraSubject->UnSubscribe(m_pCameraObserver);
	ENGINE::Safe_Delete(m_pCameraObserver);
}

HRESULT CPlayer_Hand::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	// Texture
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_DYNAMIC, L"SMG_Fire");
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

	m_pTransform = dynamic_cast<ENGINE::CTransform*>(pComponent);
	NULL_CHECK_RETURN(m_pTransform, E_FAIL);

	return S_OK;
}

void CPlayer_Hand::Set_Pos(D3DXVECTOR3 _Pos)
{
	m_pTransform->SetPos(_Pos);
}

void CPlayer_Hand::Set_WeaponAct()
{
	m_eActState = static_cast<CPlayer*>(m_pTarget)->Get_WeaponAct();
}

void CPlayer_Hand::WeaponActState()
{
	switch (m_eActState)
	{
	case CPlayer::W_NONE:
	{
		ChangeTex(L"SMG_Fire");
		break;
	}

	case CPlayer::W_FIRST:
	{
		ChangeTex(L"SMG_Fire");
		break;
	}

	case CPlayer::W_FIRE:
	{
		ChangeTex(L"SMG_Fire");
		break;
	}

	case CPlayer::W_RELOAD:
	{
		break;
	}

	case CPlayer::W_DRAW:
	{
		break;
	}

	case CPlayer::W_ZOOM:
	{
		break;
	}

	case CPlayer::W_ZOOMFIRE:
	{
		break;
	}
	}
}

void CPlayer_Hand::ChangeTex(wstring _wstrTex)
{
	if (m_wstrTex == _wstrTex)
		return;

	m_wstrTex = _wstrTex;

	m_mapComponent.erase(L"Texture");

	ENGINE::CComponent* pComponent = nullptr;
	pComponent = ENGINE::GetResourceMgr()->CloneResource(ENGINE::RESOURCE_DYNAMIC, _wstrTex);
	NULL_CHECK(pComponent);
	m_mapComponent.insert({ L"Texture", pComponent });

	m_pTexture = dynamic_cast<ENGINE::CTexture*>(pComponent);
	NULL_CHECK(m_pTexture);
}

CPlayer_Hand * CPlayer_Hand::Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _Target)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CPlayer_Hand* pInstance = new CPlayer_Hand(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	pInstance->m_pTarget = _Target;

	return pInstance;
}
