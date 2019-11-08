#include "stdafx.h"
#include "Camera.h"
#include "Camera_Component.h"
#include "Trasform.h"


CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_eCameraMode(LAND_MODE), m_eCameraViewPoint(THIRD_PERSON),
	m_pGraphicDev(pGraphicDev),
	m_pTarget(nullptr), m_pCCamera_Component(nullptr),
	m_fMax_PlayerRoll_Angle(0), m_fCamShake_Y(0)
{
	ZeroMemory(&m_tCameraInfo, sizeof(ENGINE::CAM_INFO));
	m_pGraphicDev->AddRef();
}


CCamera::~CCamera()
{
	Release();
}

void CCamera::Starfe(float _Speed)
{
	switch (m_eCameraMode)
	{
	case LAND_MODE:
	{
		m_tCameraInfo.vEyePos += D3DXVECTOR3(m_tCameraInfo.vRight.x, 0.f, m_tCameraInfo.vRight.z) * _Speed;
		m_tCameraInfo.vLookAt += D3DXVECTOR3(m_tCameraInfo.vRight.x, 0.f, m_tCameraInfo.vRight.z) * _Speed;

		break;
	}
	case FLY_MODE:
	{
		m_tCameraInfo.vEyePos += m_tCameraInfo.vRight * _Speed;
		break;
	}
	}
}

void CCamera::Fly(float _Speed)
{
	switch (m_eCameraMode)
	{
	case FLY_MODE:
	{
		m_tCameraInfo.vEyePos += m_tCameraInfo.vLook * _Speed;
		break;
	}
	}
}

void CCamera::Walk(float _Speed)
{
	switch (m_eCameraMode)
	{
	case LAND_MODE:
	{
		D3DXVECTOR3 vDir, vWorldUp;
		vWorldUp = { 0.f , 1.f ,0.f };

		D3DXVec3Cross(&vDir, &m_tCameraInfo.vRight, &vWorldUp);

		m_tCameraInfo.vEyePos += D3DXVECTOR3(vDir.x, 0.f, vDir.z) * _Speed;
		m_tCameraInfo.vLookAt += D3DXVECTOR3(vDir.x, 0.f, vDir.z) * _Speed;

		break;
	}
	case FLY_MODE:
	{
		m_tCameraInfo.vEyePos += m_tCameraInfo.vLook * _Speed;

		break;
	}
	}
}

void CCamera::Pitch(float _Angle)
{
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &m_tCameraInfo.vRight, D3DXToRadian(_Angle));

	D3DXVec3TransformCoord(&m_tCameraInfo.vUp, &m_tCameraInfo.vUp, &T);
	D3DXVec3TransformCoord(&m_tCameraInfo.vLook, &m_tCameraInfo.vLook, &T);
}

void CCamera::Yaw(float _Angle)
{
	if (m_eCameraViewPoint == THIRD_PERSON)
		return;

	D3DXVECTOR3  vView, vWorldUp;
	D3DXMATRIX T;

	switch (m_eCameraMode)
	{
	case LAND_MODE:
	{
		vWorldUp = { 0,1,0 };

		//m_tCameraInfo.vEyePos.z = 0.f;

		vView = m_tCameraInfo.vLookAt - m_tCameraInfo.vEyePos;

		D3DXMatrixRotationAxis(&T, &vWorldUp, D3DXToRadian(_Angle));
		D3DXVec3TransformCoord(&vView, &vView, &T);

		m_tCameraInfo.vLookAt = m_tCameraInfo.vEyePos + vView;
		break;
	}
	case FLY_MODE:
	{
		D3DXMatrixRotationAxis(&T, &m_tCameraInfo.vUp, D3DXToRadian(_Angle));
		break;
	}
	case SPYCAM_MODE:
	{
		D3DXMatrixRotationAxis(&T, &m_tCameraInfo.vUp, D3DXToRadian(_Angle));
		break;
	}
	}

	D3DXVec3TransformCoord(&m_tCameraInfo.vRight, &m_tCameraInfo.vRight, &T);
	D3DXVec3TransformCoord(&m_tCameraInfo.vLook, &m_tCameraInfo.vLook, &T);
}

void CCamera::Roll(float _Angle)
{
	D3DXMATRIX T;

	switch (m_eCameraMode)
	{
	case LAND_MODE:
	{
		if (fabs(_Angle) <= m_fMax_PlayerRoll_Angle)
			D3DXMatrixRotationAxis(&T, &m_tCameraInfo.vLook, D3DXToRadian(_Angle));

		break;
	}
	case FLY_MODE:
	{
		D3DXMatrixRotationAxis(&T, &m_tCameraInfo.vLook, D3DXToRadian(_Angle));
		break;
	}
	}

	D3DXVec3TransformCoord(&m_tCameraInfo.vRight, &m_tCameraInfo.vRight, &T);
	D3DXVec3TransformCoord(&m_tCameraInfo.vUp, &m_tCameraInfo.vUp, &T);
}

void CCamera::GetViewMatrix(D3DXMATRIX * _ViewMatrix)
{
	//D3DXVECTOR3 tmpLook = { 0,0,1 };
	//D3DXVECTOR3 tmpRight = { 1,0,0 };
	//D3DXVECTOR3 tmpUp = { 0,1,0 };
	//
	//m_pCCamera_Component->Set_Look(m_pCCamera_Component->Get_LookAt() - m_pCCamera_Component->Get_EyePos());
	//
	//D3DXVec3Normalize(&tmpLook, &m_pCCamera_Component->Get_Look());
	//
	//D3DXVec3Cross(&tmpRight, &m_pCCamera_Component->Get_Up(), &tmpLook);
	//D3DXVec3Normalize(&tmpRight, &m_pCCamera_Component->Get_Right());
	//
	//D3DXVec3Cross(&tmpUp, &tmpLook, &m_pCCamera_Component->Get_Right());
	//D3DXVec3Normalize(&tmpUp, &m_pCCamera_Component->Get_Up());
	//
	//float x = -D3DXVec3Dot(&m_pCCamera_Component->Get_EyePos(), &tmpRight);
	//float y = -D3DXVec3Dot(&m_pCCamera_Component->Get_EyePos(), &tmpUp);
	//float z = -D3DXVec3Dot(&m_pCCamera_Component->Get_EyePos(), &tmpLook);
	//
	//m_pCCamera_Component->Set_Right(tmpRight);
	//m_pCCamera_Component->Set_Up(tmpUp);
	//m_pCCamera_Component->Set_Look(tmpLook);
	//
	//_ViewMatrix->_11 = m_pCCamera_Component->Get_Right().x;
	//_ViewMatrix->_12 = m_pCCamera_Component->Get_Up().x;
	//_ViewMatrix->_13 = m_pCCamera_Component->Get_Look().x;
	//_ViewMatrix->_14 = 0.f;
	//
	//_ViewMatrix->_21 = m_pCCamera_Component->Get_Right().y;
	//_ViewMatrix->_22 = m_pCCamera_Component->Get_Up().y;
	//_ViewMatrix->_23 = m_pCCamera_Component->Get_Look().y;
	//_ViewMatrix->_24 = 0.f;
	//
	//_ViewMatrix->_31 = m_pCCamera_Component->Get_Right().z;
	//_ViewMatrix->_32 = m_pCCamera_Component->Get_Up().z;
	//_ViewMatrix->_33 = m_pCCamera_Component->Get_Look().z;
	//_ViewMatrix->_34 = 0.f;
	//
	//_ViewMatrix->_41 = x;
	//_ViewMatrix->_42 = y;
	//_ViewMatrix->_43 = z;
	//_ViewMatrix->_44 = 1.f;

	m_tCameraInfo.vLook = m_tCameraInfo.vLookAt - m_tCameraInfo.vEyePos;
	D3DXVec3Normalize(&m_tCameraInfo.vLook, &m_tCameraInfo.vLook);

	D3DXVec3Cross(&m_tCameraInfo.vRight, &m_tCameraInfo.vUp, &m_tCameraInfo.vLook);
	D3DXVec3Normalize(&m_tCameraInfo.vRight, &m_tCameraInfo.vRight);

	D3DXVec3Cross(&m_tCameraInfo.vUp, &m_tCameraInfo.vLook, &m_tCameraInfo.vRight);
	D3DXVec3Normalize(&m_tCameraInfo.vUp, &m_tCameraInfo.vUp);

	float x = -D3DXVec3Dot(&m_tCameraInfo.vEyePos, &m_tCameraInfo.vRight);
	float y = -D3DXVec3Dot(&m_tCameraInfo.vEyePos, &m_tCameraInfo.vUp);
	float z = -D3DXVec3Dot(&m_tCameraInfo.vEyePos, &m_tCameraInfo.vLook);

	_ViewMatrix->_11 = m_tCameraInfo.vRight.x;
	_ViewMatrix->_12 = m_tCameraInfo.vUp.x;
	_ViewMatrix->_13 = m_tCameraInfo.vLook.x;
	_ViewMatrix->_14 = 0.f;

	_ViewMatrix->_21 = m_tCameraInfo.vRight.y;
	_ViewMatrix->_22 = m_tCameraInfo.vUp.y;
	_ViewMatrix->_23 = m_tCameraInfo.vLook.y;
	_ViewMatrix->_24 = 0.f;

	_ViewMatrix->_31 = m_tCameraInfo.vRight.z;
	_ViewMatrix->_32 = m_tCameraInfo.vUp.z;
	_ViewMatrix->_33 = m_tCameraInfo.vLook.z;
	_ViewMatrix->_34 = 0.f;

	_ViewMatrix->_41 = x;
	_ViewMatrix->_42 = y;
	_ViewMatrix->_43 = z;
	_ViewMatrix->_44 = 1.f;
}

void CCamera::SetCameraMode(CameraMode _CameraType)
{
}

void CCamera::SetUp_ViewPoint(CameraViewPoint _CameraViewPoint)
{
	switch (_CameraViewPoint)
	{
	case FIRST_PERSON:
	{
		break;
	}
	case THIRD_PERSON:
	{
		D3DXVECTOR3 vTempDir = -dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetDir();
		D3DXVECTOR3 vTempPos = dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();
		
		D3DXVec3Normalize(&vTempDir, &vTempDir);

		m_tCameraInfo.vEyePos = { vTempPos.x + (vTempDir.x * m_tCameraInfo.fDistance),
			vTempPos.y + (vTempDir.y * m_tCameraInfo.fDistance * 0.5f) + 1 + m_tCameraInfo.fDistance * 0.5f,
			vTempPos.z + (vTempDir.z * m_tCameraInfo.fDistance)
		};

		m_tCameraInfo.vLookAt = { vTempPos.x  , vTempPos.y + 1.6f, vTempPos.z };
		m_tCameraInfo.vUp = { 0,1,0 };
		break;

		//D3DXVec3Normalize(&vTempDir, &vTempDir);
		//
		//m_pCCamera_Component->Set_EyePos({ vTempPos.x + (vTempDir.x * m_pCCamera_Component->Get_Distance()),
		//	vTempPos.y + (vTempDir.y * m_pCCamera_Component->Get_Distance() * 0.5f) + 1 + m_pCCamera_Component->Get_Distance() * 0.5f,
		//	vTempPos.z + (vTempDir.z * m_pCCamera_Component->Get_Distance())
		//});
		//
		//m_pCCamera_Component->Set_LookAt({ vTempPos.x, vTempPos.y + 1.6f, vTempPos.z });
		//m_pCCamera_Component->Set_Up({ 0,1,0 });
		//break;
	}
	}

}

void CCamera::SetUp_FirstPerson_ViewPoint()
{
}

void CCamera::Update()
{
	//if (GetAsyncKeyState('Z'))
	//	m_tCameraInfo.fDistance -= 0.01f;
	//
	//if (GetAsyncKeyState('X'))
	//	m_tCameraInfo.fDistance += 0.01f;

	D3DXMATRIX matView;

	SetUp_ViewPoint(m_eCameraViewPoint);

	GetViewMatrix(&matView);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);

	//if (m_tCameraInfo.fDistance <= 1.1f && m_eCameraViewPoint == THIRD_PERSON)
	//{
	//	D3DXVECTOR3 vTemp_TargetPos = dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();
	//
	//	m_eCameraViewPoint = FIRST_PERSON;
	//	
	//	m_tCameraInfo.vEyePos = { vTemp_TargetPos.x, vTemp_TargetPos.y + 3,vTemp_TargetPos.z };
	//	m_tCameraInfo.vRight = { 1,0,0 };
	//	m_tCameraInfo.vUp = { 0,1,0 };
	//	m_tCameraInfo.vLook = { 0,0,1 };
	//	m_tCameraInfo.vLookAt = { vTemp_TargetPos.x , vTemp_TargetPos.y + 3, vTemp_TargetPos.z + 1 };
	//	
	//	Yaw(dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetAngle(ENGINE::ANGLE_Y));
	//}
	//
	//if (m_tCameraInfo.fDistance > 1.1f)
	//	m_eCameraViewPoint = THIRD_PERSON;

}

void CCamera::LateUpdate()
{
}

void CCamera::Render()
{
}

HRESULT CCamera::Initialize()
{
	//FAILED_CHECK_RETURN(AddComponent(), E_FAIL);
	//
	//m_pCCamera_Component->Set_EyePos({ 0,3,0 });
	//m_pCCamera_Component->Set_Right({ 1,0,0 });
	//m_pCCamera_Component->Set_Up({ 0,1,0 });
	//m_pCCamera_Component->Set_Look({ 0,0,1 });
	//m_pCCamera_Component->Set_LookAt({ 0,2,1 });
	//m_pCCamera_Component->Set_Distance(5.f);

	m_tCameraInfo.vEyePos = { 0,3,0 };	// Camera 의 Pos [ eye 에 해당 ]
	m_tCameraInfo.vRight = { 1,0,0 };	// X 축 기준
	m_tCameraInfo.vUp = { 0,1,0 };	// Y 축 기준
	m_tCameraInfo.vLook = { 0,0,1 };	// Z 축 기준
	m_tCameraInfo.vLookAt = { 0,2,1 };	// 카메라가 바라볼 대상
	m_tCameraInfo.fDistance = 5.f;			// 3 인칭 시, 카메라와 플레이어의 거리
	

	m_fMax_PlayerRoll_Angle = 25.f;			// 1 인칭 시, 좌우 기울이기 최대 각도

	D3DXMATRIX matView, matProj;

	GetViewMatrix(&matView);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(70.f), WINCX / (float)WINCY, 1.f, 1000.f);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);

	return S_OK;
}

void CCamera::Release()
{
	ENGINE::Safe_Release(m_pGraphicDev);
}

HRESULT CCamera::AddComponent()
{
	ENGINE::CComponent* pComponent = nullptr;
	
	pComponent = ENGINE::CCamera_Component::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert({ L"Camera", pComponent });

	m_pCCamera_Component = dynamic_cast<ENGINE::CCamera_Component*>(pComponent);
	NULL_CHECK_RETURN(m_pCCamera_Component, E_FAIL);
	
	return S_OK;
}

CCamera * CCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _Target)
{
	CCamera* pInstance = new CCamera(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		ENGINE::Safe_Delete(pInstance);
		return nullptr;
	}

	pInstance->Set_Target(_Target);

	return pInstance;
}
