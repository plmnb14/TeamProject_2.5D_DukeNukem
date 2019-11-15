#include "stdafx.h"
#include "Bullet.h"
#include "Camera.h"
#include "Trasform.h"
#include "Camera_Component.h"


CBullet::CBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pKeyMgr(ENGINE::GetKeyMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr)
{
}

CBullet::~CBullet()
{
	Release();
}

int CBullet::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	ENGINE::CGameObject::Update();

	D3DXVECTOR3 vDir = m_pTransform->GetDir();

	m_pTransform->MovePos(0.5f);
	//m_pTransform->Move_AdvancedPos(vDir ,0.5f);

	KeyInput();

	return NO_EVENT;
}

void CBullet::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
}

void CBullet::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &(m_pTransform->GetWorldMatrix()));
	//m_pTexture->Render(0);
	m_pBuffer->Render();
}

HRESULT CBullet::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));

	return S_OK;
}

void CBullet::Release()
{
}

HRESULT CBullet::AddComponent()
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

void CBullet::KeyInput()
{
	
}

void CBullet::Set_Target(CGameObject * _Target)
{
	m_pTarget = _Target;
}

void CBullet::Set_Device(LPDIRECT3DDEVICE9 pGraphicDev)
{
	m_pGraphicDev = pGraphicDev;
}

void CBullet::Set_Pos(D3DXVECTOR3 _Pos)
{
	m_pTransform->SetPos(_Pos);
}

void CBullet::Set_Dir(D3DXVECTOR3 _Dir)
{
	m_pTransform->SetDir(_Dir);
}

void CBullet::Set_Angle(float * _Angle)
{
	m_pTransform->SetAngle(_Angle[0], ENGINE::ANGLE_X);
	m_pTransform->SetAngle(_Angle[1], ENGINE::ANGLE_Y);
	m_pTransform->SetAngle(_Angle[2], ENGINE::ANGLE_Z);

}

CBullet* CBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos, D3DXVECTOR3 _Dir, float* _Angle)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CBullet* pInstance = new CBullet(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	pInstance->Set_Pos(_Pos);
	pInstance->Set_Dir(_Dir);
	pInstance->Set_Angle(_Angle);

	return pInstance;
}
