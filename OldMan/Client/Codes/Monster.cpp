#include "stdafx.h"
#include "Monster.h"
#include "Trasform.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_pResourceMgr(ENGINE::GetResourceMgr()),
	m_pTimeMgr(ENGINE::GetTimeMgr()),
	m_pTexture(nullptr), m_pBuffer(nullptr), m_pTransform(nullptr)
{
}


CMonster::~CMonster()
{
	Release();
}

void CMonster::Update()
{
	ENGINE::CGameObject::Update();
	Player_Pursue();
}

void CMonster::LateUpdate()
{
	ENGINE::CGameObject::LateUpdate();
}

void CMonster::Render()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &(m_pTransform->GetWorldMatrix()));
	//m_pTexture->Render(0);
	m_pBuffer->Render();
}

HRESULT CMonster::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pTransform->SetPos(D3DXVECTOR3(10.f, 0.f, 0.f));
	m_pTransform->SetSize(D3DXVECTOR3(1.f, 1.f, 1.f));

	return S_OK;
}

void CMonster::Release()
{
}

HRESULT CMonster::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;

	// Texture
	//pComponent = m_pResourceMgr->CloneResource(ENGINE::RESOURCE_STATIC, L"Texture_Player");
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent.insert({ L"Texture", pComponent });

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

void CMonster::Player_Pursue()
{
	D3DXVECTOR3 vTempPos = dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();
	//D3DXVECTOR3 vTempPos = dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();
	D3DXVECTOR3 vMonsterPos =m_pTransform->GetPos();
	
	D3DXVECTOR3 vMonsterDir_Fowrd = m_pTransform->GetDir();
	D3DXVECTOR3 vMonsterDir_Left = m_pTransform->GetDir();
	
	D3DXVECTOR3 vMonsterDir_Right = m_pTransform->GetDir();
	D3DXVECTOR3 vMonsterDir = vTempPos - vMonsterPos;
	
	//좌우 방향벡터로 내적을 구하고  그것으로 도는 방향을 결정하는 값을 구해서 그쪽으로 돌게만드는게 핵심 
	float m_Range =D3DXVec3Length(&(vMonsterPos - vTempPos));


	D3DXVec3Normalize(&vMonsterDir_Fowrd, &vMonsterDir_Fowrd);
	D3DXVec3Normalize(&vMonsterDir, &vMonsterDir);

	float Monster_Dot = D3DXVec3Dot(&vMonsterDir_Fowrd, &vMonsterDir);
	D3DXVec3Cross(&vMonsterDir_Right, &vMonsterDir,&vMonsterDir_Left);
	// 양수일때 왼쪽 음수일때 오른쪽 이다. y 값이 반영되면 된다. 

	cout <<D3DXToDegree( vMonsterDir_Right.y )<< endl;
	bool m_Check;
	m_Check = true;
	if (m_Range < 5)
	{
		float fMoveSpeed = 1.f * m_pTimeMgr->GetDelta();
		
		//if (vMonsterDir_Right.y > 0)
		{
		if (m_Check) {
			float fMoveSpeed = 1.f * m_pTimeMgr->GetDelta();
			//m_pTransform->SetAngle(D3DXToDegree(vMonsterDir_Right.y), ENGINE::ANGLE_Y);
			m_pTransform->MoveAngle(ENGINE::ANGLE_Y, D3DXToDegree(vMonsterDir_Right.y));
	    	}
		}
		m_Check = false;
		
		m_pTransform->SetDir(vMonsterDir);
		m_pTransform->MovePos(fMoveSpeed);
	}
	
	//정면을 보면서 추격하게 만드는게 필요 


}

CMonster * CMonster::Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _Target)
{

	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CMonster* pInstance = new CMonster(pGraphicDev);
	
	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}
	pInstance->Set_Target(_Target);

	return pInstance;
}
