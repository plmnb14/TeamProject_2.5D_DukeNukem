#include "stdafx.h"
#include "Summon_Effect.h"
#include "Trasform.h"
#include "Animator.h"
#include "CameraObserver.h"
#include "Billborad.h"

#include "Alien.h"
#include "PigMan.h"
#include "OctaBrain.h"
#include "Trooper.h"
#include "Monster.h"

CSummon_Effect::CSummon_Effect(LPDIRECT3DDEVICE9 pGraphicDev)
	:CVfx(pGraphicDev), m_wFrame(0), m_bSummon(false)
{
}

CSummon_Effect::~CSummon_Effect()
{
	Release();
}

int CSummon_Effect::Update()
{
	if (m_bIsDead)
	{
		return DEAD_OBJ;
	}

	if (m_pAnimator->Get_Frame() == 22)
	{
		if (m_bSummon == false)
		{

			m_bSummon = true;

			switch (m_eMonster)
			{
			case PIGMAN:
			{
				CGameObject* pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), m_pTransform->GetPos());
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);
				break;
			}
			case TROPPER:
			{
				CGameObject* pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), m_pTransform->GetPos());
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);
				break;
			}
			case OCTABRAIN:
			{
				CGameObject* pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), m_pTransform->GetPos());
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);
				break;
			}
			case ALIEN:
			{
				CGameObject* pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), m_pTransform->GetPos());
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);
				break;
			}
			case MONSTER:
			{
				CGameObject* pObject = CMonster::Create(m_pGraphicDev, m_mapLayer[ENGINE::CLayer::OBJECT]->Get_Player(), m_pTransform->GetPos());
				m_mapLayer[ENGINE::CLayer::OBJECT]->AddObject(ENGINE::OBJECT_TYPE::MONSTER, pObject);
				pObject->Set_MapLayer(m_mapLayer);
				break;
			}
			}
		}

	}

	ENGINE::CGameObject::LateInit();
	ENGINE::CGameObject::Update();

	return NO_EVENT;
}

void CSummon_Effect::LateUpdate()
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

void CSummon_Effect::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matView);
	m_pTexture->Render(m_pAnimator->RenderSet(m_pTimeMgr->GetDelta()));
	m_pBuffer->Render();
}

HRESULT CSummon_Effect::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(5.f, 5.f, 5.f));

	int Frame = dynamic_cast<ENGINE::CResources*>(m_pTexture)->Get_MaxFrame();
	m_pAnimator->Set_MaxFrame(Frame);
	m_pAnimator->Set_FrameAmp(20.f);
	m_pAnimator->Set_ResetOption(ENGINE::CAnimator::RESET_STOP);
	m_pAnimator->Stop_Animation(false);

	return S_OK;
}

HRESULT CSummon_Effect::LateInit()
{
	m_pObserver = CCameraObserver::Create();
	NULL_CHECK_RETURN(m_pObserver, E_FAIL);

	m_pSubject->Subscribe(m_pObserver);

	return S_OK;
}

void CSummon_Effect::Release()
{
	m_pSubject->UnSubscribe(m_pObserver);
	ENGINE::Safe_Delete(m_pObserver);
}

void CSummon_Effect::Set_Pos(D3DXVECTOR3 _Pos)
{
	m_pTransform->SetPos(_Pos);
}

HRESULT CSummon_Effect::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	// Texture
	pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, L"Monster_Create");
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

CSummon_Effect* CSummon_Effect::Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos, MONSUM _MonsterType)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CSummon_Effect* pInstance = new CSummon_Effect(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	pInstance->Set_Pos(_Pos);
	pInstance->Set_Summon(_MonsterType);

	return pInstance;
}
