#include "stdafx.h"
#include "Player.h"
#include "Camera.h"
#include "Trasform.h"
#include "Camera_Component.h"
#include "Bullet.h"
#include "Collider.h"
#include "CameraObserver.h"


CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),	
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pKeyMgr(ENGINE::GetKeyMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr), m_pCollider(nullptr),
	m_pSubject(ENGINE::GetCameraSubject()),
	m_pObserver(nullptr)
{	
}

CPlayer::~CPlayer()
{
	Release();
}

int CPlayer::Update()
{
	cout << m_pTransform->GetPos().x << endl;
	cout << dynamic_cast<CCamera*>(m_pCamera)->Get_Pos().x << endl;

	if (m_bIsDead)
		return DEAD_OBJ;
	
	m_pObserver->GetViewMatrix();
	
	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();
	KeyInput();

	m_pCollider->Set_UnderPos(m_pTransform->GetPos());
	m_pCollider->SetUp_Box();
 
	return NO_EVENT;
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

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 2.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));

	return S_OK;
}

HRESULT CPlayer::LateInit()
{
	m_pObserver = CCameraObserver::Create();
	NULL_CHECK_RETURN(m_pObserver, E_FAIL);

	m_pSubject->Subscribe(m_pObserver);

	return S_OK;
}

void CPlayer::Release()
{
	m_pSubject->UnSubscribe(m_pObserver);
	ENGINE::Safe_Delete(m_pObserver);
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


	// Collider
	pComponent = ENGINE::CCollider::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Collider", pComponent });

	m_pCollider = dynamic_cast<ENGINE::CCollider*>(pComponent);
	NULL_CHECK_RETURN(m_pCollider, E_FAIL);

	m_pCollider->Set_UnderPos(m_pTransform->GetPos());
	m_pCollider->Set_Radius({1.f , 1.f, 1.f});
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Trigger(false);

	m_pCollider->SetUp_Box();


	return S_OK;
}

void CPlayer::KeyInput()
{
	float fMoveSpeed = 5.f * m_pTimeMgr->GetDelta();
	float fAngleSpeed = 90.f * m_pTimeMgr->GetDelta();

	srand(unsigned(time(NULL)));

	float xRand = rand() % (100 - 50) * 0.01f;
	float yRand = rand() % (100 - 50) * 0.01f;

	if (m_pKeyMgr->KeyPressing(ENGINE::KEY_LBUTTON))
	{
		D3DXVECTOR3 tmpDir = m_pTransform->GetDir();
		D3DXVECTOR3 tmpLook = dynamic_cast<CCamera*>(m_pCamera)->Get_Look();
		D3DXVECTOR3 tmpUp = dynamic_cast<CCamera*>(m_pCamera)->Get_Up();
		D3DXVECTOR3 tmpRight = dynamic_cast<CCamera*>(m_pCamera)->Get_Right();
		D3DXVECTOR3 tmpPos = {  dynamic_cast<CCamera*>(m_pCamera)->Get_LookAt().x + tmpLook.x * 4 - 2 * tmpUp.x + tmpRight.x * 2,
								dynamic_cast<CCamera*>(m_pCamera)->Get_LookAt().y + tmpLook.y * 4 - 2 * tmpUp.y + tmpRight.y * 2,
								dynamic_cast<CCamera*>(m_pCamera)->Get_LookAt().z + tmpLook.z * 4 - 2 * tmpUp.z + tmpRight.z * 2 };

		float fAngle[3];

		fAngle[0] = dynamic_cast<CCamera*>(m_pCamera)->Get_YAngle() + xRand;
		fAngle[1] = dynamic_cast<CCamera*>(m_pCamera)->Get_XAngle() + yRand;
		fAngle[2] = 0;

		CGameObject* pInstance = CBullet::Create(m_pGraphicDev, tmpPos, tmpDir, fAngle);
		m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::BULLET, pInstance);
	}


	if (GetAsyncKeyState('W'))
	{
		m_pTransform->MovePos(fMoveSpeed);
	}

	if (GetAsyncKeyState('S'))
	{
		m_pTransform->MovePos(-fMoveSpeed);
	}

	if (GetAsyncKeyState('A'))
	{
		D3DXVECTOR3 vDir, vWorldUp;
		vWorldUp = { 0.f , 1.f , 0.f };
		
		D3DXVec3Cross(&vDir, &m_pTransform->GetDir(), &vWorldUp);
		m_pTransform->Move_AdvancedPos(D3DXVECTOR3(vDir.x, 0.f, vDir.z) , fMoveSpeed);
	}

	if (GetAsyncKeyState('D'))
	{
		D3DXVECTOR3 vDir, vWorldUp;
		vWorldUp = { 0.f , 1.f , 0.f };

		D3DXVec3Cross(&vDir, &m_pTransform->GetDir(), &vWorldUp);
		m_pTransform->Move_AdvancedPos(D3DXVECTOR3(vDir.x, 0.f, vDir.z), -fMoveSpeed);
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		m_pTransform->MoveAngle(ENGINE::ANGLE_Y, -fAngleSpeed);
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		m_pTransform->MoveAngle(ENGINE::ANGLE_Y, fAngleSpeed);
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
