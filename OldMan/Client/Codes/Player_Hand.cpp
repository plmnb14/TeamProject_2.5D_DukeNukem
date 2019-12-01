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
#include "SoundMgr.h"

CPlayer_Hand::CPlayer_Hand(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTransform(nullptr), m_pAnimator(nullptr),
	m_pTexture(nullptr), m_pCameraObserver(nullptr), m_pBuffer(nullptr),
	m_pTarget(nullptr),
	m_pCameraSubject(ENGINE::GetCameraSubject()),
	m_fSizeY(0), m_fSizeX(0), m_fFrame(0), m_eWeapon(ENGINE::MELLE),
	m_eActState(CPlayer::W_DRAW), m_eOldAcState(CPlayer::W_WALK),
	m_bPump(false), m_bPumpIn(false), m_bPumpOut(false)
{

}

CPlayer_Hand::~CPlayer_Hand()
{
	CGameObject::Release();
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
}

void CPlayer_Hand::Render()
{
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE); // 알파블렌딩 on
	m_pGraphicDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	// D3DBLEND_SRCALPHA:  (As, As, As, As)
	// As: Source픽셀의 알파 값을 0 ~ 1 범위로 치환.
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);

	// D3DBLEND_INVSRCALPHA: (1-As, 1-As, 1-As, 1-As)
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


	// Set Proj BeforRender ==========================================================
	D3DXMATRIX matWorld, matView, matProj, matTempView, matTempProj;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixIdentity(&matView);
	D3DXMatrixIdentity(&matProj);
	D3DXMatrixIdentity(&matTempView);
	D3DXMatrixIdentity(&matTempProj);

	// Get Temp
	if (m_pCameraObserver != nullptr)
	{
		matTempView = m_pCameraObserver->GetViewMatrix();
		matTempProj = m_pCameraObserver->GetProjMatrix();
		matProj = m_pCameraObserver->GetProjMatrix();
	}

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
	m_pAnimator->RenderSet(m_pTimeMgr->GetDelta());
	m_pTexture->Render(m_pAnimator->Get_Frame());
	m_pBuffer->Render();

	// Set Proj AfterRender ==========================================================
	// Set Device Original Transform
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matTempView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matTempProj);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE); // 알파블렌딩 off

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x00000088); // 88 ~ 77 ... 등의 값 아래의 알파값은 제외시킴
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
}

HRESULT CPlayer_Hand::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	// 트랜스폼 세팅
	m_pTransform->SetPos(D3DXVECTOR3(5.f, 285.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(13.f, 13.f, 13.f));

	m_fSizeX = 50.f;
	m_fSizeY = 50.f;

	m_pAnimator->Set_FrameAmp(1.f);
	m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_ZERO);
	m_pAnimator->Set_Reverse(false);
	m_pAnimator->Set_MaxFrame(0);
	m_pAnimator->Stop_Animation(false);

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
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, L"Hand_Idle");
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Texture", pComponent });
	
	m_pTexture = static_cast<ENGINE::CTexture*>(pComponent);
	NULL_CHECK_RETURN(m_pTexture, E_FAIL);

	// Buffer
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_DYNAMIC, L"Buffer_RcTex");
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Buffer", pComponent });

	m_pBuffer = static_cast<ENGINE::CVIBuffer*>(pComponent);
	NULL_CHECK_RETURN(m_pBuffer, E_FAIL);

	// Transform
	pComponent = ENGINE::CTransform::Create(D3DXVECTOR3(0.f, 0.f, 1.f));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Transform", pComponent });

	m_pTransform = static_cast<ENGINE::CTransform*>(pComponent);
	NULL_CHECK_RETURN(m_pTransform, E_FAIL);

	// Animator
	pComponent = ENGINE::CAnimator::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Animator", pComponent });

	m_pAnimator = static_cast<ENGINE::CAnimator*>(pComponent);
	NULL_CHECK_RETURN(m_pAnimator, E_FAIL);

	return S_OK;
}

void CPlayer_Hand::Set_Pos(D3DXVECTOR3 _Pos)
{
	m_pTransform->SetPos(_Pos);
}

void CPlayer_Hand::Set_WeaponAct()
{
	m_eOldAcState = m_eActState;
	m_eActState = static_cast<CPlayer*>(m_pTarget)->Get_WeaponAct();
	m_eWeapon = static_cast<CPlayer*>(m_pTarget)->Get_WState();
	WeaponActState();
}

void CPlayer_Hand::WeaponActState()
{
	if (m_eActState == CPlayer::W_GRENADE)
	{
		m_pTransform->SetPos(D3DXVECTOR3(0.f, 570.f, 0.f));
		m_pTransform->SetSize(D3DXVECTOR3(20.f, 20.f, 20.f));
		m_pAnimator->Stop_Animation(false);
		ChangeTex(L"Grenade_Throw");
		m_pAnimator->Set_FrameAmp(10.f);
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_ZERO);

		if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
		{
			m_pTransform->SetPos(D3DXVECTOR3(0.f, 285.f, 0.f));
			m_pTransform->SetSize(D3DXVECTOR3(13.f, 13.f, 13.f));
			static_cast<CPlayer*>(m_pTarget)->Set_WaponAct(CPlayer::W_DRAW);
			static_cast<CPlayer*>(m_pTarget)->Set_Grenade(false);
			m_pAnimator->Stop_Animation(false);
		}
	}

	switch(m_eWeapon)
	{
	case ENGINE::MELLE:
	{
		Weapon_Melee();
		break;
	}
	case ENGINE::REVOLVER:
	{
		Weapon_Revolver();
		break;
	}
	case ENGINE::SMG:
	{
		Weapon_SMG();
		break;
	}
	case ENGINE::RIFLE:
	{
		Weapon_Rifle();
		break;
	}
	case ENGINE::SHOTGUN:
	{
		Weapon_Shotgun();
		break;
	}
	case ENGINE::LUNCHER:
	{
		Weapon_Luncher();
		break;
	}
	}
}

void CPlayer_Hand::Weapon_Revolver()
{
	switch (m_eActState)
	{
	case CPlayer::W_LEDGE:
	{
		m_pAnimator->Stop_Animation(false);
		ChangeTex(L"Ledge_Hand");
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_ZERO);
		m_pAnimator->Set_FrameAmp(20.f);

		if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
		{
			static_cast<CPlayer*>(m_pTarget)->Set_WaponAct(CPlayer::W_DRAW);
		}

		break;
	}

	case CPlayer::W_IDLE:
	{
		ChangeTex(L"Revolver_Idle");
		m_pAnimator->Set_Frame(0);
		m_pAnimator->Set_FrameAmp(1.f);
		break;
	}

	case CPlayer::W_FIRST:
	{
		ChangeTex(L"Revolver_First");
		break;
	}

	case CPlayer::W_FIRE:
	{
		m_pAnimator->Stop_Animation(false);
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_STOP);
		m_pAnimator->Set_FrameAmp(30.f);
		ChangeTex(L"Revolver_Fire");

		if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
		{
			static_cast<CPlayer*>(m_pTarget)->Set_WaponAct(CPlayer::W_IDLE);
		}

		break;
	}

	case CPlayer::W_RELOAD:
	{
		m_pAnimator->Stop_Animation(false);

		if (m_eOldAcState == CPlayer::W_IDLE)
		{
			m_pAnimator->Stop_Animation(false);
			m_pAnimator->Set_Frame(0);
		}

		if (m_pAnimator->Get_Frame() == 4)
		{
			CSoundMgr::GetInstance()->SetVolume(CSoundMgr::EFFECT, 0.5f);
			CSoundMgr::GetInstance()->StopSound(CSoundMgr::EFFECT);
			CSoundMgr::GetInstance()->MyPlaySound(L"Revolver_Open.wav", CSoundMgr::EFFECT);

			CSoundMgr::GetInstance()->SetVolume(CSoundMgr::WEAPON, 0.5f);
			CSoundMgr::GetInstance()->StopSound(CSoundMgr::WEAPON);
			CSoundMgr::GetInstance()->MyPlaySound(L"Weapon_Spin.wav", CSoundMgr::WEAPON);
		}

		if (m_pAnimator->Get_Frame() == 25)
		{
			CSoundMgr::GetInstance()->SetVolume(CSoundMgr::WEAPON, 0.5f);
			CSoundMgr::GetInstance()->StopSound(CSoundMgr::WEAPON);
			CSoundMgr::GetInstance()->MyPlaySound(L"Revolver_Load.wav", CSoundMgr::WEAPON);
		}

		if (m_pAnimator->Get_Frame() == 32)
		{
			CSoundMgr::GetInstance()->SetVolume(CSoundMgr::EFFECT, 0.5f);
			CSoundMgr::GetInstance()->StopSound(CSoundMgr::EFFECT);
			CSoundMgr::GetInstance()->MyPlaySound(L"Revolver_Up.wav", CSoundMgr::EFFECT);
		}

		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_STOP);
		m_pAnimator->Set_FrameAmp(30.f);
		ChangeTex(L"Revolver_Reload");

		if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
		{
			static_cast<CPlayer*>(m_pTarget)->Set_WaponAct(CPlayer::W_IDLE);
		}

		break;
	}

	case CPlayer::W_DRAW:
	{
		m_pAnimator->Stop_Animation(false);
		ChangeTex(L"Revolver_Draw");
		m_pAnimator->Set_FrameAmp(15.f);
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_STOP);

		if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
		{
			static_cast<CPlayer*>(m_pTarget)->Set_WaponAct(CPlayer::W_IDLE);
			m_pAnimator->Stop_Animation(false);
		}

		break;
	}

	case CPlayer::W_SPECIAL_READY:
	{
		m_pAnimator->Stop_Animation(false);
		ChangeTex(L"Revolver_FaseReady");
		m_pAnimator->Set_FrameAmp(30.f);
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_ZERO);

		if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
		{
			static_cast<CPlayer*>(m_pTarget)->Set_WaponAct(CPlayer::W_SPECIAL_SHOT);
			m_pAnimator->Stop_Animation(false);
		}

		break;
	}

	case CPlayer::W_SPECIAL_SHOT:
	{
		m_pAnimator->Stop_Animation(false);
		ChangeTex(L"Revolver_Fast");
		m_pAnimator->Set_FrameAmp(60.f);
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_ZERO);

		if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
		{
			if (static_cast<CPlayer*>(m_pTarget)->Get_WInfo()->wMagazineBullet <= 0)
			{
				m_pAnimator->Set_Frame(0);
				static_cast<CPlayer*>(m_pTarget)->Set_SpecialShot(false);
				static_cast<CPlayer*>(m_pTarget)->Set_WaponAct(CPlayer::W_SPECIAL_END);
			}

			else
			{
				static_cast<CPlayer*>(m_pTarget)->Set_SpecialShot(true);
			}

			m_pAnimator->Stop_Animation(false);
		}

		break;
	}

	case CPlayer::W_SPECIAL_END:
	{
		m_pAnimator->Stop_Animation(false);
		ChangeTex(L"Revolver_FastEnd");
		m_pAnimator->Set_FrameAmp(30.f);
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_STOP);

		if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
		{
			static_cast<CPlayer*>(m_pTarget)->Set_WaponAct(CPlayer::W_IDLE);
			m_pAnimator->Stop_Animation(false);
		}

		break;
	}
	}
}

void CPlayer_Hand::Weapon_Melee()
{
	switch (m_eActState)
	{
	case CPlayer::W_LEDGE:
	{
		//m_pTransform->SetPos(D3DXVECTOR3(0.f, 520.f, 0.f));
		//m_pTransform->SetSize(D3DXVECTOR3(18.f, 18.f, 18.f));

		m_pAnimator->Stop_Animation(false);
		ChangeTex(L"Ledge_Hand");
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_ZERO);
		m_pAnimator->Set_FrameAmp(20.f);

		if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
		{
			//m_pTransform->SetPos(D3DXVECTOR3(0.f, 285.f, 0.f));
			//m_pTransform->SetSize(D3DXVECTOR3(13.f, 13.f, 13.f));

			static_cast<CPlayer*>(m_pTarget)->Set_WaponAct(CPlayer::W_DRAW);
		}

		break;
	}

	case CPlayer::W_IDLE:
	{
		ChangeTex(L"Hand_Idle");
		m_pAnimator->Set_Frame(0);
		m_pAnimator->Set_FrameAmp(1.f);
		break;
	}
	case CPlayer::W_DRAW:
	{
		m_pAnimator->Stop_Animation(false);
		ChangeTex(L"Hand_Draw");
		m_pAnimator->Set_FrameAmp(10.f);
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_STOP);

		if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
		{
			static_cast<CPlayer*>(m_pTarget)->Set_WaponAct(CPlayer::W_IDLE);
			m_pAnimator->Stop_Animation(false);
		}

		break;
	}
	}

}

void CPlayer_Hand::Weapon_Rifle()
{
}

void CPlayer_Hand::Weapon_SMG()
{
	switch (m_eActState)
	{
	case CPlayer::W_LEDGE:
	{
		m_pAnimator->Stop_Animation(false);
		ChangeTex(L"Ledge_Hand");
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_ZERO);
		m_pAnimator->Set_FrameAmp(20.f);

		if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
		{
			static_cast<CPlayer*>(m_pTarget)->Set_WaponAct(CPlayer::W_DRAW);
		}

		break;
	}

	case CPlayer::W_IDLE:
	{
		ChangeTex(L"SMG_Idle");
		m_pAnimator->Set_Frame(0);
		m_pAnimator->Set_FrameAmp(1.f);
		break;
	}

	case CPlayer::W_FIRST:
	{
		ChangeTex(L"SMG_First");
		break;
	}

	case CPlayer::W_FIRE:
	{
		m_pAnimator->Stop_Animation(false);
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_ZERO);
		m_pAnimator->Set_FrameAmp(50.f);
		ChangeTex(L"SMG_Fire");

		if (static_cast<CPlayer*>(m_pTarget)->Get_WInfo()->wMagazineBullet <= 0)
		{
			m_pAnimator->Set_Frame(0);

			if (static_cast<CPlayer*>(m_pTarget)->Get_Zoom())
			{
				static_cast<CPlayer*>(m_pTarget)->Set_WaponAct(CPlayer::W_ZOOMIN);
				m_pAnimator->Set_Frame(3);
			}

			if (static_cast<CPlayer*>(m_pTarget)->Get_Zoom() == false)
			{
				static_cast<CPlayer*>(m_pTarget)->Set_WaponAct(CPlayer::W_IDLE);
			}
		}

		break;
	}

	case CPlayer::W_RELOAD:
	{
		if (m_eOldAcState == CPlayer::W_ZOOMFIRE ||
			m_eOldAcState == CPlayer::W_ZOOMOUT ||
			m_eOldAcState == CPlayer::W_IDLE ||
			m_eOldAcState == CPlayer::W_ZOOMIN)
		{
			m_pAnimator->Stop_Animation(false);
			m_pAnimator->Set_Frame(0);
		}

		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_STOP);
		m_pAnimator->Set_FrameAmp(30.f);
		ChangeTex(L"SMG_Reload");

		if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
		{
			static_cast<CPlayer*>(m_pTarget)->Set_WaponAct(CPlayer::W_IDLE);
			m_pAnimator->Stop_Animation(false);
		}

		break;
	}

	case CPlayer::W_DRAW:
	{
		m_pAnimator->Stop_Animation(false);
		ChangeTex(L"SMG_Draw");
		m_pAnimator->Set_FrameAmp(10.f);
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_STOP);

		if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
		{
			static_cast<CPlayer*>(m_pTarget)->Set_WaponAct(CPlayer::W_IDLE);
			m_pAnimator->Stop_Animation(false);
		}

		break;
	}

	case CPlayer::W_ZOOMIN:
	{
		if (m_eOldAcState == CPlayer::W_IDLE)
		{
			m_pAnimator->Stop_Animation(false);
			m_pAnimator->Set_Frame(0);
		}

		if (m_eOldAcState == CPlayer::W_ZOOMFIRE ||
			m_eOldAcState == CPlayer::W_ZOOMOUT)
			m_pAnimator->Set_Frame(3);

		ChangeTex(L"SMG_Zoom");
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_STOP);
		m_pAnimator->Set_FrameAmp(20.f);
		break;
	}

	case CPlayer::W_ZOOMOUT:
	{
		ChangeTex(L"SMG_ZoomOut");
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_STOP);
		m_pAnimator->Set_FrameAmp(20.f);

		if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
		{
			static_cast<CPlayer*>(m_pTarget)->Set_WaponAct(CPlayer::W_IDLE);
		}

		break;
	}

	case CPlayer::W_ZOOMFIRE:
	{
		if (m_eOldAcState == CPlayer::W_ZOOMIN)
		{
			m_pAnimator->Set_Frame(0);
			m_pAnimator->Stop_Animation(false);
		}

		m_pAnimator->Stop_Animation(false);
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_ZERO);
		m_pAnimator->Set_FrameAmp(50.f);
		ChangeTex(L"SMG_ZoomFire");
		break;
	}
	}
}

void CPlayer_Hand::Weapon_Shotgun()
{
	switch (m_eActState)
	{
	case CPlayer::W_LEDGE:
	{
		m_pAnimator->Stop_Animation(false);
		ChangeTex(L"Ledge_Hand");
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_ZERO);
		m_pAnimator->Set_FrameAmp(20.f);

		if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
		{
			static_cast<CPlayer*>(m_pTarget)->Set_WaponAct(CPlayer::W_DRAW);
		}

		break;
	}

	case CPlayer::W_IDLE:
	{
		ChangeTex(L"PumpShot_Idle");
		m_pAnimator->Set_Frame(0);
		m_pAnimator->Set_FrameAmp(1.f);
		break;
	}

	case CPlayer::W_FIRE:
	{
		m_pAnimator->Stop_Animation(false);
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_ZERO);
		m_pAnimator->Set_FrameAmp(30.f);

		if (!m_bPump)
		{
			ChangeTex(L"PumpShot_Fire");

			if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
			{
				m_bPump = true;
				m_bPumpIn = true;
				m_pAnimator->Set_Frame(0);
			}
		}

		if(m_bPump)
		{
			if (m_bPumpOut)
			{
				if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
				{
					ChangeTex(L"PumpShot_Idle");
					static_cast<CPlayer*>(m_pTarget)->Set_WaponAct(CPlayer::W_IDLE);
					static_cast<CPlayer*>(m_pTarget)->Set_CanAttack(true);
					m_pAnimator->Stop_Animation(false);
		
					m_bPump = false;
					m_bPumpIn = false;
					m_bPumpOut = false;
					m_pAnimator->Set_Frame(0);
					m_pAnimator->Set_FrameAmp(0);

				}
			}
		
			if (m_bPumpIn)
			{
				m_pAnimator->Set_FrameAmp(50.f);
				ChangeTex(L"PumpShot_PumpIn");
				//m_pAnimator->Stop_Animation(false);

				if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
				{
					ChangeTex(L"PumpShot_PumpOut");
					m_pAnimator->Stop_Animation(false);
					m_bPumpOut = true;
					m_bPumpIn = false;
					m_pAnimator->Set_Frame(0);

					if (m_pAnimator->Get_Frame() == 0)
					{
						CSoundMgr::GetInstance()->SetVolume(CSoundMgr::WEAPON, 1.0f);
						CSoundMgr::GetInstance()->StopSound(CSoundMgr::WEAPON);
						CSoundMgr::GetInstance()->MyPlaySound(L"Shotgun_InoutReload.wav", CSoundMgr::WEAPON);
					}
				}
			}
		}

		break;
	}

	case CPlayer::W_RELOAD:
	{
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_ZERO);

		if (static_cast<CPlayer*>(m_pTarget)->Get_WInfo()->wCurBullet > 0 &&
			static_cast<CPlayer*>(m_pTarget)->Get_WInfo()->wMagazineBullet < static_cast<CPlayer*>(m_pTarget)->Get_WInfo()->wMagazineSize)
		{
			ChangeTex(L"PumpShot_Reload");
			m_pAnimator->Set_FrameAmp(30.f);

			if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
			{
				static_cast<CPlayer*>(m_pTarget)->Reload();
				m_bPump = false;
			}

			if (m_pAnimator->Get_Frame() == 4)
			{
				int iSound = rand() % 4;

				CSoundMgr::GetInstance()->SetVolume(CSoundMgr::WEAPON, 1.0f);
				CSoundMgr::GetInstance()->StopSound(CSoundMgr::WEAPON);
				switch (iSound)
				{
				case 0:
					CSoundMgr::GetInstance()->MyPlaySound(L"Shotgun_Load_1.wav", CSoundMgr::WEAPON);
					break;
				case 1:
					CSoundMgr::GetInstance()->MyPlaySound(L"Shotgun_Load_2.wav", CSoundMgr::WEAPON);
					break;
				case 2:
					CSoundMgr::GetInstance()->MyPlaySound(L"Shotgun_Load_3.wav", CSoundMgr::WEAPON);
					break;
				case 3:
					CSoundMgr::GetInstance()->MyPlaySound(L"Shotgun_Load_4.wav", CSoundMgr::WEAPON);
					break;
				}
			}
		}

		else if(!m_bPump)
		{
			ChangeTex(L"PumpShot_PumpIn");
			m_pAnimator->Set_FrameAmp(50.f);

			if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
			{
				ChangeTex(L"PumpShot_PumpOut");
				m_pAnimator->Set_Frame(0);
				m_bPump = true;
			}
		}

		else if (m_bPump)
		{
			ChangeTex(L"PumpShot_PumpOut");

			if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
			{
				ChangeTex(L"PumpShot_Idle");
				static_cast<CPlayer*>(m_pTarget)->Set_WaponAct(CPlayer::W_IDLE);
				static_cast<CPlayer*>(m_pTarget)->Set_CanAttack(true);
				m_bPump = false;

			}

			if (m_pAnimator->Get_Frame() == 0)
			{
				CSoundMgr::GetInstance()->SetVolume(CSoundMgr::WEAPON, 1.0f);
				CSoundMgr::GetInstance()->StopSound(CSoundMgr::WEAPON);
				CSoundMgr::GetInstance()->MyPlaySound(L"Shotgun_InoutReload.wav", CSoundMgr::WEAPON);
			}
		}

		break;
	}

	case CPlayer::W_DRAW:
	{
		m_pAnimator->Stop_Animation(false);
		ChangeTex(L"PumpShot_Draw");
		m_pAnimator->Set_FrameAmp(10.f);
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_STOP);

		if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
		{
			static_cast<CPlayer*>(m_pTarget)->Set_WaponAct(CPlayer::W_IDLE);
			m_pAnimator->Stop_Animation(false);
		}

		break;
	}

	case CPlayer::W_ZOOMIN:
	{
		if (m_eOldAcState == CPlayer::W_IDLE)
		{
			m_pAnimator->Stop_Animation(false);
			m_pAnimator->Set_Frame(0);
		}

		if (m_eOldAcState == CPlayer::W_ZOOMFIRE ||
			m_eOldAcState == CPlayer::W_ZOOMOUT)
			m_pAnimator->Set_Frame(3);

		ChangeTex(L"SMG_Zoom");
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_STOP);
		m_pAnimator->Set_FrameAmp(20.f);
		break;
	}

	case CPlayer::W_ZOOMOUT:
	{
		ChangeTex(L"SMG_ZoomOut");
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_STOP);
		m_pAnimator->Set_FrameAmp(20.f);

		if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
		{
			static_cast<CPlayer*>(m_pTarget)->Set_WaponAct(CPlayer::W_IDLE);
		}

		break;
	}

	case CPlayer::W_ZOOMFIRE:
	{
		if (m_eOldAcState == CPlayer::W_ZOOMIN)
		{
			m_pAnimator->Set_Frame(0);
			m_pAnimator->Stop_Animation(false);
		}

		m_pAnimator->Stop_Animation(false);
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_ZERO);
		m_pAnimator->Set_FrameAmp(50.f);
		ChangeTex(L"SMG_ZoomFire");
		break;
	}
	}
}

void CPlayer_Hand::Weapon_Luncher()
{
	switch (m_eActState)
	{
	case CPlayer::W_LEDGE:
	{
		m_pAnimator->Stop_Animation(false);
		ChangeTex(L"Ledge_Hand");
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_ZERO);
		m_pAnimator->Set_FrameAmp(20.f);

		if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
		{
			static_cast<CPlayer*>(m_pTarget)->Set_WaponAct(CPlayer::W_DRAW);
		}

		break;
	}

	case CPlayer::W_IDLE:
	{
		if (static_cast<CPlayer*>(m_pTarget)->Get_WInfo()->wMagazineBullet > 0)
		{

			if (static_cast<CPlayer*>(m_pTarget)->Get_WInfo()->fDelayTimer <= 0)
				ChangeTex(L"RocketLuncher_Idle");

			if (static_cast<CPlayer*>(m_pTarget)->Get_WInfo()->fDelayTimer > 0)
				ChangeTex(L"RocketLuncher_Idle_Delay");
		}

		if (static_cast<CPlayer*>(m_pTarget)->Get_WInfo()->wMagazineBullet <= 0)
		{
			ChangeTex(L"RocketLuncher_NoAmmo");
		}

		m_pAnimator->Set_Frame(0);
		m_pAnimator->Set_FrameAmp(1.f);
		break;
	}

	case CPlayer::W_FIRST:
	{
		ChangeTex(L"RocketLuncher_Idle");
		break;
	}

	case CPlayer::W_FIRE:
	{
		m_pAnimator->Stop_Animation(false);
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_STOP);
		m_pAnimator->Set_FrameAmp(30.f);
		ChangeTex(L"RocketLuncher_Fire");

		if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
		{
			static_cast<CPlayer*>(m_pTarget)->Set_WaponAct(CPlayer::W_IDLE);
			m_pAnimator->Stop_Animation(false);
		}

		break;
	}

	case CPlayer::W_RELOAD:
	{
		if (m_eOldAcState == CPlayer::W_ZOOMFIRE ||
			m_eOldAcState == CPlayer::W_ZOOMOUT ||
			m_eOldAcState == CPlayer::W_IDLE ||
			m_eOldAcState == CPlayer::W_ZOOMIN)
		{
			m_pAnimator->Stop_Animation(false);
			m_pAnimator->Set_Frame(0);
		}

		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_STOP);
		m_pAnimator->Set_FrameAmp(30.f);
		ChangeTex(L"RocketLuncher_Reload");

		if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
		{
			static_cast<CPlayer*>(m_pTarget)->Set_WaponAct(CPlayer::W_IDLE);
			m_pAnimator->Stop_Animation(false);
		}

		if (m_pAnimator->Get_Frame() == 0)
		{
			CSoundMgr::GetInstance()->SetVolume(CSoundMgr::WEAPON, 1.0f);
			CSoundMgr::GetInstance()->StopSound(CSoundMgr::WEAPON);
			CSoundMgr::GetInstance()->MyPlaySound(L"RocketLuncher_Load.wav", CSoundMgr::WEAPON);
		}
		break;
	}

	case CPlayer::W_DRAW:
	{
		m_pAnimator->Stop_Animation(false);
		ChangeTex(L"RocketLuncher_Draw");
		m_pAnimator->Set_FrameAmp(10.f);
		m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_STOP);

		if (m_pAnimator->Get_MaxFrame() - 1 <= m_pAnimator->Get_Frame())
		{
			static_cast<CPlayer*>(m_pTarget)->Set_WaponAct(CPlayer::W_IDLE);
			m_pAnimator->Stop_Animation(false);
		}

		break;
	}
	}
}

void CPlayer_Hand::ChangeTex(wstring _wstrTex)
{
	if (m_wstrTex.compare(_wstrTex) == 0)
		return;
	
	m_wstrTex = _wstrTex;
	
	m_mapComponent.erase(L"Texture");
	
	ENGINE::CComponent* pComponent = nullptr;
	pComponent = ENGINE::GetResourceMgr()->CloneResource(ENGINE::RESOURCE_STATIC, _wstrTex);
	NULL_CHECK(pComponent);
	m_mapComponent.insert({ L"Texture", pComponent });
	
	m_pAnimator->Set_MaxFrame(static_cast<ENGINE::CResources*>(pComponent)->Get_MaxFrame());
	
	m_pTexture = static_cast<ENGINE::CTexture*>(pComponent);
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
