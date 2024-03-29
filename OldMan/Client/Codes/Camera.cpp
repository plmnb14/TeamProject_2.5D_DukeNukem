#include "stdafx.h"
#include "Camera.h"
#include "Camera_Component.h"
#include "Trasform.h"


CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_eCameraMode(LAND_MODE), m_eCameraViewPoint(THIRD_PERSON),
	m_pGraphicDev(pGraphicDev), m_pSubject(ENGINE::GetCameraSubject()),
	m_pTarget(nullptr), m_pCCamera_Component(nullptr),
	m_fMax_PlayerRoll_Angle(0), m_fCamShake_Y(0),
	m_fEyeHeight(0) , m_fZoom_Min(0) , m_fZoom_Max(0),
	m_fX_Angle(0) , m_fY_Angle(0), m_fZ_Angle(0),
	m_fX_OriginXAngle(0) , m_fX_OriginYAngle(0),
	m_pTimeMgr(ENGINE::GetTimeMgr()), m_pKeyMgr(ENGINE::GetKeyMgr())
{
	m_pGraphicDev->AddRef();
}


CCamera::~CCamera()
{
	Release();
}

void CCamera::Starfe(float _Speed)
{
	if (m_eCameraViewPoint == THIRD_PERSON)
		return;

	switch (m_eCameraMode)
	{
	case LAND_MODE:
	{
		m_pCCamera_Component->Add_EyePos(D3DXVECTOR3(m_pCCamera_Component->Get_Right().x, 0.f, m_pCCamera_Component->Get_Right().z) * _Speed);
		m_pCCamera_Component->Add_LookAt(D3DXVECTOR3(m_pCCamera_Component->Get_Right().x, 0.f, m_pCCamera_Component->Get_Right().z) * _Speed);

		break;
	}
	case FLY_MODE:
	{
		m_pCCamera_Component->Add_EyePos(m_pCCamera_Component->Get_Right() * _Speed);
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
		m_pCCamera_Component->Add_EyePos(m_pCCamera_Component->Get_Look() * _Speed);
		break;
	}
	}
}

void CCamera::Walk(float _Speed)
{
	if (m_eCameraViewPoint == THIRD_PERSON)
		return;

	switch (m_eCameraMode)
	{
	case LAND_MODE:
	{
		D3DXVECTOR3 vDir, vWorldUp;
		vWorldUp = { 0.f , 1.f ,0.f };

		D3DXVec3Cross(&vDir, &m_pCCamera_Component->Get_Right(), &vWorldUp);

		m_pCCamera_Component->Add_EyePos(D3DXVECTOR3(vDir.x, 0.f, vDir.z) * _Speed);
		m_pCCamera_Component->Add_LookAt(D3DXVECTOR3(vDir.x, 0.f, vDir.z) * _Speed);

		break;
	}
	case FLY_MODE:
	{
		m_pCCamera_Component->Add_EyePos(m_pCCamera_Component->Get_Look() * _Speed);

		break;
	}
	}
}

void CCamera::Pitch(float _Angle)
{
	D3DXMATRIX T;
	D3DXVECTOR3 tmpUp, tmpLook;

	D3DXMatrixRotationAxis(&T, &m_pCCamera_Component->Get_Right(), D3DXToRadian(_Angle));

	D3DXVec3TransformCoord(&tmpUp, &m_pCCamera_Component->Get_Up(), &T);
	D3DXVec3TransformCoord(&tmpLook, &m_pCCamera_Component->Get_Look(), &T);

	m_pCCamera_Component->Set_Up(tmpUp);
	m_pCCamera_Component->Set_Look(tmpLook);
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

		vView = m_pCCamera_Component->Get_LookAt() - m_pCCamera_Component->Get_EyePos();

		D3DXMatrixRotationAxis(&T, &vWorldUp, D3DXToRadian(_Angle));
		D3DXVec3TransformCoord(&vView, &vView, &T);

		m_pCCamera_Component->Set_LookAt(m_pCCamera_Component->Get_EyePos() + vView);
		break;
	}
	case FLY_MODE:
	{
		D3DXMatrixRotationAxis(&T, &m_pCCamera_Component->Get_Up(), D3DXToRadian(_Angle));
		break;
	}
	case SPYCAM_MODE:
	{
		D3DXMatrixRotationAxis(&T, &m_pCCamera_Component->Get_Up(), D3DXToRadian(_Angle));
		break;
	}
	}


	D3DXVECTOR3 vTmpRight, vTempLook;

	D3DXVec3TransformCoord(&vTmpRight, &m_pCCamera_Component->Get_Right(), &T);
	D3DXVec3TransformCoord(&vTempLook, &m_pCCamera_Component->Get_Look(), &T);

	m_pCCamera_Component->Set_Right(vTmpRight);
	m_pCCamera_Component->Set_Look(vTempLook);
}

void CCamera::Roll(float _Angle)
{
	D3DXMATRIX T;

	switch (m_eCameraMode)
	{
	case LAND_MODE:
	{
		D3DXVECTOR3  vView;

		vView = m_pCCamera_Component->Get_LookAt() - m_pCCamera_Component->Get_EyePos();

		D3DXMatrixRotationAxis(&T, &m_pCCamera_Component->Get_Look(), D3DXToRadian(_Angle));
		D3DXVec3TransformCoord(&vView, &vView, &T);

		m_pCCamera_Component->Set_LookAt(m_pCCamera_Component->Get_EyePos() + vView);
		break;
	}
	case FLY_MODE:
	{
		D3DXMatrixRotationAxis(&T, &m_pCCamera_Component->Get_Look(), D3DXToRadian(_Angle));
		break;
	}
	}

	D3DXVECTOR3 vTmpRight, vTmpUp;

	D3DXVec3TransformCoord(&vTmpRight, &m_pCCamera_Component->Get_Right(), &T);
	D3DXVec3TransformCoord(&vTmpUp, &m_pCCamera_Component->Get_Up(), &T);

	m_pCCamera_Component->Set_Right(vTmpRight);
	m_pCCamera_Component->Set_Up(vTmpUp);
}

void CCamera::SetUp_ViewMatrix(D3DXMATRIX * _ViewMatrix)
{
	D3DXVECTOR3 tmpLook = { 0,0,1 };
	D3DXVECTOR3 tmpRight = { 1,0,0 };
	D3DXVECTOR3 tmpUp = { 0,1,0 };

	tmpLook = m_pCCamera_Component->Get_LookAt() - m_pCCamera_Component->Get_EyePos();
	D3DXVec3Normalize(&tmpLook, &tmpLook);

	D3DXVec3Cross(&tmpRight, &tmpUp, &tmpLook);
	D3DXVec3Normalize(&tmpRight, &tmpRight);

	D3DXVec3Cross(&tmpUp, &tmpLook, &tmpRight);
	D3DXVec3Normalize(&tmpUp, &tmpUp);

	float x = -D3DXVec3Dot(&m_pCCamera_Component->Get_EyePos(), &tmpRight);
	float y = -D3DXVec3Dot(&m_pCCamera_Component->Get_EyePos(), &tmpUp);
	float z = -D3DXVec3Dot(&m_pCCamera_Component->Get_EyePos(), &tmpLook);

	m_pCCamera_Component->Set_Right(tmpRight);
	m_pCCamera_Component->Set_Up(tmpUp);
	m_pCCamera_Component->Set_Look(tmpLook);

	_ViewMatrix->_11 = m_pCCamera_Component->Get_Right().x;
	_ViewMatrix->_12 = m_pCCamera_Component->Get_Up().x;
	_ViewMatrix->_13 = m_pCCamera_Component->Get_Look().x;
	_ViewMatrix->_14 = 0.f;

	_ViewMatrix->_21 = m_pCCamera_Component->Get_Right().y;
	_ViewMatrix->_22 = m_pCCamera_Component->Get_Up().y;
	_ViewMatrix->_23 = m_pCCamera_Component->Get_Look().y;
	_ViewMatrix->_24 = 0.f;

	_ViewMatrix->_31 = m_pCCamera_Component->Get_Right().z;
	_ViewMatrix->_32 = m_pCCamera_Component->Get_Up().z;
	_ViewMatrix->_33 = m_pCCamera_Component->Get_Look().z;
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
		SetUp_MouseRotate();
		
		POINT pt;
		pt.x = WINCX / 2;
		pt.y = WINCY / 2;
		
		ClientToScreen(g_hWnd, &pt);
		SetCursorPos(pt.x, pt.y);
		
		D3DXVECTOR3 vTemp_TargetPos = dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();
		m_pCCamera_Component->Set_EyePos({ vTemp_TargetPos.x, vTemp_TargetPos.y + 3,vTemp_TargetPos.z });

		break;
	}
	case THIRD_PERSON:
	{

		D3DXVECTOR3 vTempDir = -dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetDir();
		D3DXVECTOR3 vTempPos = dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();

		D3DXVec3Normalize(&vTempDir, &vTempDir);

		m_pCCamera_Component->Set_EyePos({ vTempPos.x + (vTempDir.x * m_pCCamera_Component->Get_Distance()),
			vTempPos.y + (vTempDir.y * m_pCCamera_Component->Get_Distance() * 0.5f) + 1 + m_pCCamera_Component->Get_Distance() * 0.5f,
			vTempPos.z + (vTempDir.z * m_pCCamera_Component->Get_Distance())
		});

		m_pCCamera_Component->Set_LookAt({ vTempPos.x, vTempPos.y + 1.6f, vTempPos.z });
		m_pCCamera_Component->Set_Up({ 0, 1, 0 });

		break;

	}
	}

}

void CCamera::SetUp_FirstPerson_ViewPoint()
{
	if (m_pCCamera_Component->Get_Distance() <= 1.f && m_eCameraViewPoint == THIRD_PERSON)
	{
		D3DXVECTOR3 vTemp_TargetPos = dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();

		m_eCameraViewPoint = FIRST_PERSON;

		m_pCCamera_Component->Set_EyePos({ vTemp_TargetPos.x, vTemp_TargetPos.y + 3,vTemp_TargetPos.z });
		m_pCCamera_Component->Set_LookAt({ vTemp_TargetPos.x , vTemp_TargetPos.y + 3, vTemp_TargetPos.z + 1 });

		Yaw(dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetAngle(ENGINE::ANGLE_Y));
	}

	if (m_pCCamera_Component->Get_Distance() > 1.f)
		m_eCameraViewPoint = THIRD_PERSON;
}

void CCamera::SetUp_Zoom()
{
	if (m_pKeyMgr->KeyPressing(ENGINE::KEY_SPACE))
	{
		if (m_pCCamera_Component->Get_Distance() > m_fZoom_Min)
			m_pCCamera_Component->Add_Distance(-0.1f);

		POINT pt;
		pt.x = WINCX / 2;
		pt.y = WINCY / 2;

		ClientToScreen(g_hWnd, &pt);
		SetCursorPos(pt.x, pt.y);

	}

	if (GetAsyncKeyState('X'))
	{
		if (m_pCCamera_Component->Get_Distance() < m_fZoom_Max)
			m_pCCamera_Component->Add_Distance(0.1f);
	}
}

void CCamera::SetUp_MouseRotate()
{
	POINT tmpPT = m_pKeyMgr->Get_MouseGap(g_hWnd);

	float fAngle = 0.f;
	D3DXMATRIX	matRot;
	D3DXVECTOR3 vDir;

	if (tmpPT.x != 0)
	{
		D3DXVECTOR3 tmpRight, tmpUp, tmpLook;
		D3DXVECTOR3 tmpEyePos;

		m_fX_Angle = tmpPT.x * 0.5f;
		m_fX_OriginXAngle += m_fX_Angle;

		if (m_fX_OriginXAngle <= -360 || m_fX_OriginXAngle >= 360)
			m_fX_OriginXAngle = 0;

		dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->MoveAngle(ENGINE::ANGLE_Y, m_fX_Angle);
		
		D3DXMatrixRotationY(&matRot, D3DXToRadian(m_fX_Angle));
		
		memcpy(&matRot._41, &m_pCCamera_Component->Get_EyePos(), sizeof(D3DXVECTOR3));
		
		vDir = m_pCCamera_Component->Get_LookAt() - m_pCCamera_Component->Get_EyePos();
		
		// x 축 재조정
		D3DXVec3TransformNormal(&tmpRight, &m_pCCamera_Component->Get_Right(), &matRot);
		D3DXVec3Normalize(&tmpRight, &tmpRight);
		m_pCCamera_Component->Set_Right(tmpRight);
		
		// y 축 재조정
		D3DXVec3TransformNormal(&tmpUp, &m_pCCamera_Component->Get_Up(), &matRot);
		D3DXVec3Normalize(&tmpUp, &tmpUp);
		m_pCCamera_Component->Set_Up(tmpUp);
		
		// z 축 재조정
		D3DXVec3TransformNormal(&tmpLook, &m_pCCamera_Component->Get_Look(), &matRot);
		D3DXVec3Normalize(&tmpLook, &tmpLook);
		m_pCCamera_Component->Set_Look(tmpLook);
		
		// 카메라 위치 재조정
		D3DXVec3TransformCoord(&tmpEyePos, &vDir, &matRot);
		m_pCCamera_Component->Set_LookAt(tmpEyePos);
	}

	if (tmpPT.y != 0)
	{
		if (m_pCCamera_Component->Get_Look().z < 0.2f && m_pCCamera_Component->Get_Look().z > 0.f)
		{
			if (m_pCCamera_Component->Get_Look().y < -0.9f)
			{
				if (tmpPT.y > 0)
					return;
			}

			if (m_pCCamera_Component->Get_Look().y > 0.9f)
			{
				if (tmpPT.y < 0)
					return;
			}
		}

		else if (m_pCCamera_Component->Get_Look().z > -0.2f && m_pCCamera_Component->Get_Look().z < 0.f)
		{
			if (m_pCCamera_Component->Get_Look().y < -0.9f)
			{
				if (tmpPT.y > 0)
					return;
			}

			if (m_pCCamera_Component->Get_Look().y > 0.9f)
			{
				if (tmpPT.y < 0)
					return;
			}
		}

		D3DXVECTOR3 tmpRight, tmpUp, tmpLook;
		D3DXVECTOR3 tmpEyePos;

		m_fY_Angle = tmpPT.y * 0.5f;
		m_fX_OriginYAngle = m_pCCamera_Component->Get_Look().y * -90;

		if (m_fX_OriginYAngle <= -90 || m_fX_OriginYAngle >= 90)
			m_fX_OriginYAngle = 0;
		
		D3DXMatrixRotationAxis(&matRot, &m_pCCamera_Component->Get_Right(), D3DXToRadian(m_fY_Angle));
		memcpy(&matRot._41, &m_pCCamera_Component->Get_EyePos(), sizeof(D3DXVECTOR3));
		
		vDir = m_pCCamera_Component->Get_LookAt() - m_pCCamera_Component->Get_EyePos();
		
		// y 축 재조정
		D3DXVec3TransformNormal(&tmpUp, &m_pCCamera_Component->Get_Up(), &matRot);
		D3DXVec3Normalize(&tmpUp, &tmpUp);
		m_pCCamera_Component->Set_Up(tmpUp);
		
		// z 축 재조정
		D3DXVec3TransformNormal(&tmpLook, &m_pCCamera_Component->Get_Look(), &matRot);
		D3DXVec3Normalize(&tmpLook, &tmpLook);
		m_pCCamera_Component->Set_Look(tmpLook);
		
		// 카메라 위치 재조정
		D3DXVec3TransformCoord(&tmpEyePos, &vDir, &matRot);
		m_pCCamera_Component->Set_LookAt(tmpEyePos);
	}
}

D3DXVECTOR3 CCamera::Get_Pos()
{
	return m_pCCamera_Component->Get_EyePos();
}

D3DXVECTOR3 CCamera::Get_LookAt()
{
	return m_pCCamera_Component->Get_LookAt();
}

D3DXVECTOR3 CCamera::Get_Look()
{
	return m_pCCamera_Component->Get_Look();
}

D3DXVECTOR3 CCamera::Get_Right()
{
	return m_pCCamera_Component->Get_Right();
}

D3DXVECTOR3 CCamera::Get_Up()
{
	return m_pCCamera_Component->Get_Up();
}

int CCamera::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	D3DXMATRIX matView;

	SetUp_Zoom();
	SetUp_FirstPerson_ViewPoint();
	SetUp_ViewPoint(m_eCameraViewPoint);
	SetUp_ViewMatrix(&matView);

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);

	m_pSubject->AddData(D3DTS_VIEW, &matView);

	return NO_EVENT;
}

void CCamera::LateUpdate()
{
}

void CCamera::Render()
{
}

HRESULT CCamera::Initialize()
{
	FAILED_CHECK_RETURN(AddComponent(), E_FAIL);

	m_pCCamera_Component->Set_EyePos({ 0,3,0 });
	m_pCCamera_Component->Set_Right({ 1,0,0 });
	m_pCCamera_Component->Set_Up({ 0,1,0 });
	m_pCCamera_Component->Set_Look({ 0,0,1 });
	m_pCCamera_Component->Set_LookAt({ 0,2,1 });

	m_pCCamera_Component->Set_Distance(5.f);
	m_pCCamera_Component->set_MainCamera(true);


	m_fMax_PlayerRoll_Angle = 25.f;			// 1 인칭 시, 좌우 기울이기 최대 각도
	m_fEyeHeight = 3.f;
	m_fZoom_Min = 1.f;
	m_fZoom_Max = 10.f;


	D3DXMATRIX matView, matProj;

	SetUp_ViewMatrix(&matView);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(70.f), WINCX / (float)WINCY, 1.f, 1000.f);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);

	ShowCursor(false);

	m_pSubject->AddData(D3DTS_VIEW, &matView);
	m_pSubject->AddData(D3DTS_PROJECTION, &matProj);

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
