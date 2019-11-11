#include "stdafx.h"
#include "Player.h"
#include "Camera.h"
#include "Trasform.h"
#include "Camera_Component.h"
#include "Bullet.h"


CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),	
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pKeyMgr(ENGINE::GetKeyMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr)
{	
}

CPlayer::~CPlayer()
{
	Release();
}

void CPlayer::Update()
{
	ENGINE::CGameObject::Update();

	KeyInput();
}

void CPlayer::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
}

void CPlayer::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &(m_pTransform->GetWorldMatrix()));
	//m_pTexture->Render(0);
	m_pBuffer->Render();
}

HRESULT CPlayer::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));

	return S_OK;
}

void CPlayer::Release()
{
}

HRESULT CPlayer::AddComponent()
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

void CPlayer::KeyInput()
{
	float fMoveSpeed = 5.f * m_pTimeMgr->GetDelta();
	float fAngleSpeed = 90.f * m_pTimeMgr->GetDelta();

	if (m_pKeyMgr->KeyPressing(ENGINE::KEY_LBUTTON))
	{
		//float fAngle[3];
		//
		//fAngle[0] = m_pTransform->GetAngle(ENGINE::ANGLE_X);
		//fAngle[1] = m_pTransform->GetAngle(ENGINE::ANGLE_Y);
		//fAngle[2] = m_pTransform->GetAngle(ENGINE::ANGLE_Z);

		D3DXVECTOR3 tmpDir;
		D3DXVECTOR3 tmpPos = { dynamic_cast<CCamera*>(m_pCamera)->Get_Pos().x , dynamic_cast<CCamera*>(m_pCamera)->Get_Pos().y - 1.5f , dynamic_cast<CCamera*>(m_pCamera)->Get_Pos().z };

		// 3ÀÎÄª
		//D3DXVECTOR3 tmpDir = dynamic_cast<CCamera*>(m_pCamera)->Get_Pos() - dynamic_cast<CCamera*>(m_pCamera)->Get_LookAt();
		//D3DXVec3Normalize(&tmpDir, &tmpDir);
		float fAngle[3];

		fAngle[0] = dynamic_cast<CCamera*>(m_pCamera)->Get_YAngle();
		fAngle[1] = dynamic_cast<CCamera*>(m_pCamera)->Get_XAngle();
		fAngle[2] = 1;

		CGameObject* pInstance = CBullet::Create(m_pGraphicDev, tmpPos , tmpDir, fAngle);
		m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::BULLET, pInstance);

	}


	if (GetAsyncKeyState('W'))
	{
		m_pTransform->MovePos(fMoveSpeed);
		dynamic_cast<CCamera*>(m_pCamera)->Walk(fMoveSpeed);
	}

	if (GetAsyncKeyState('S'))
	{
		m_pTransform->MovePos(-fMoveSpeed);
		dynamic_cast<CCamera*>(m_pCamera)->Walk(-fMoveSpeed);
	}

	if (GetAsyncKeyState('A'))
	{
		D3DXVECTOR3 vDir, vWorldUp;
		vWorldUp = { 0.f , 1.f , 0.f };
		
		D3DXVec3Cross(&vDir, &m_pTransform->GetDir(), &vWorldUp);
		m_pTransform->Move_AdvancedPos(D3DXVECTOR3(vDir.x, 0.f, vDir.z) , fMoveSpeed);

		dynamic_cast<CCamera*>(m_pCamera)->Starfe(-fMoveSpeed);
	}

	if (GetAsyncKeyState('D'))
	{
		D3DXVECTOR3 vDir, vWorldUp;
		vWorldUp = { 0.f , 1.f , 0.f };

		D3DXVec3Cross(&vDir, &m_pTransform->GetDir(), &vWorldUp);
		m_pTransform->Move_AdvancedPos(D3DXVECTOR3(vDir.x, 0.f, vDir.z), -fMoveSpeed);

		dynamic_cast<CCamera*>(m_pCamera)->Starfe(fMoveSpeed);
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		dynamic_cast<CCamera*>(m_pCamera)->Yaw(-fAngleSpeed);
		m_pTransform->MoveAngle(ENGINE::ANGLE_Y, -fAngleSpeed);
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		dynamic_cast<CCamera*>(m_pCamera)->Yaw(fAngleSpeed);
		m_pTransform->MoveAngle(ENGINE::ANGLE_Y, fAngleSpeed);
	}


	if (m_pKeyMgr->KeyPressing(ENGINE::KEY_Q))
	{
		dynamic_cast<CCamera*>(m_pCamera)->Roll(-1.f);
	}

	else if (m_pKeyMgr->KeyPressing(ENGINE::KEY_E))
	{

	}
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CPlayer* pInstance = new CPlayer(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}
