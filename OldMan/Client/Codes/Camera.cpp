#include "stdafx.h"
#include "Camera.h"
#include "Camera_Component.h"
#include "Trasform.h"
#include "Condition.h"
#include "Player.h"


CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: ENGINE::CGameObject(pGraphicDev),
	m_eCameraMode(LAND_MODE), m_eCameraViewPoint(FIRST_PERSON),
	m_pGraphicDev(pGraphicDev), m_pSubject(ENGINE::GetCameraSubject()),
	m_pTarget(nullptr), m_pCCamera_Component(nullptr),
	m_fMax_PlayerRoll_Angle(0), m_fCamShake_Y(0),
	m_fEyeHeight(0) , m_fZoom_Min(0) , m_fZoom_Max(0),
	m_fX_Angle(0) , m_fY_Angle(0), m_fZ_Angle(0),
	m_fX_OriginXAngle(0) , m_fX_OriginYAngle(0),
	m_fVertical_Move(0) , m_fHorizontal_Move(0),
	m_fReverse_Vertical(0) , m_fReverse_Horizontal(0),
	m_bLeft(false), m_bCamPosUp(false), m_bCamPosDown(true),
	m_vMaxCamShakePos({0,0,0}), m_bCamPosRight(true) , m_bCamPosLeft(false),
	m_bCamPosFront(true), m_bCamPosBack(false), m_fCam_PosY(0), m_fFov(0),
	m_pTimeMgr(ENGINE::GetTimeMgr()), m_pKeyMgr(ENGINE::GetKeyMgr())
{
	D3DXMatrixIdentity(&m_MatView);
	m_pGraphicDev->AddRef();
}


CCamera::~CCamera()
{
	Release();
}

void CCamera::Starfe(float _Speed)
{
	int a = 0;

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
		m_pCCamera_Component->Add_LookAt(m_pCCamera_Component->Get_Right() * _Speed);
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
		m_pCCamera_Component->Add_LookAt(m_pCCamera_Component->Get_Look() * _Speed);

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
		m_fX_Angle = _Angle;
		m_fX_OriginXAngle += m_fX_Angle;

		if (m_fX_OriginXAngle <= -360 || m_fX_OriginXAngle >= 360)
			m_fX_OriginXAngle = 0;

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
	D3DXVECTOR3 vRight = m_pCCamera_Component->Get_Right();
	D3DXVECTOR3 vUp = m_pCCamera_Component->Get_Up();
	D3DXVECTOR3 vLook = m_pCCamera_Component->Get_Look();

	if (m_eCameraViewPoint == THIRD_PERSON)
	{
		vLook = m_pCCamera_Component->Get_LookAt() - m_pCCamera_Component->Get_EyePos();
		D3DXVec3Normalize(&vLook, &vLook);

		D3DXVec3Cross(&vRight, &vUp, &vLook);
		D3DXVec3Normalize(&vRight, &vRight);

		D3DXVec3Cross(&vUp, &vLook, &vRight);
		D3DXVec3Normalize(&vUp, &vUp);
	}
	
	D3DXMatrixIdentity(_ViewMatrix);

	float x = -D3DXVec3Dot(&m_pCCamera_Component->Get_EyePos(), &vRight);
	float y = -D3DXVec3Dot(&m_pCCamera_Component->Get_EyePos(), &vUp);
	float z = -D3DXVec3Dot(&m_pCCamera_Component->Get_EyePos(), &vLook);

	m_pCCamera_Component->Set_Right(vRight);
	m_pCCamera_Component->Set_Up(vUp);
	m_pCCamera_Component->Set_Look(vLook);

	//

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
		switch (m_eCameraMode)
		{
		case LAND_MODE:
		{
			D3DXVECTOR3 vTemp_TargetPos = dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetPos();
			m_pCCamera_Component->Set_EyePos({ vTemp_TargetPos.x + (m_vCamShakePos.x * m_pCCamera_Component->Get_Right().z * m_pCCamera_Component->Get_Look().z),
												vTemp_TargetPos.y + 2.5f + m_vCamShakePos.y + m_fCam_PosY,
												vTemp_TargetPos.z + (m_vCamShakePos.z * m_pCCamera_Component->Get_Right().x * m_pCCamera_Component->Get_Look().x) });
			m_pCCamera_Component->Set_LookAt({ vTemp_TargetPos.x, vTemp_TargetPos.y + 2.5f ,vTemp_TargetPos.z + 1 });

			CamShakePos();
		}

		}

		SetUp_MouseRotate();
		
		POINT pt;
		pt.x = WINCX / 2;
		pt.y = WINCY / 2;
		
		ClientToScreen(g_hWnd, &pt);
		SetCursorPos(pt.x, pt.y);

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

		m_pCCamera_Component->Set_LookAt({ vTempPos.x, vTempPos.y + 1.3f, vTempPos.z });
		m_pCCamera_Component->Set_Up({ 0, 1, 0 });

		break;

	}
	}

}

void CCamera::SetUp_FirstPerson_ViewPoint()
{
	if (m_eCameraMode == FLY_MODE)
		return;

	if (m_pCCamera_Component->Get_Distance() <= m_fZoom_Min && m_eCameraViewPoint == THIRD_PERSON)
	{
		D3DXVECTOR3 vTempDir = dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->GetDir();

		m_eCameraViewPoint = FIRST_PERSON;
		//m_pCCamera_Component->Set_Look(vTempDir);
	}

	if (m_pCCamera_Component->Get_Distance() > m_fZoom_Min)
		m_eCameraViewPoint = THIRD_PERSON;
}

void CCamera::SetUp_Zoom()
{
	if (m_pKeyMgr->KeyPressing(ENGINE::KEY_Z))
	{
		if (m_pCCamera_Component->Get_Distance() > m_fZoom_Min)
			m_pCCamera_Component->Add_Distance(-0.1f);
	
		POINT pt;
		pt.x = WINCX / 2;
		pt.y = WINCY / 2;
	
		ClientToScreen(g_hWnd, &pt);
		SetCursorPos(pt.x, pt.y);
	}
	
	if (m_pKeyMgr->KeyPressing(ENGINE::KEY_X))
	{
		if (m_pCCamera_Component->Get_Distance() < m_fZoom_Max)
			m_pCCamera_Component->Add_Distance(0.1f);

		POINT pt;
		pt.x = WINCX / 2;
		pt.y = WINCY / 2;

		ClientToScreen(g_hWnd, &pt);
		SetCursorPos(pt.x, pt.y);
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

		m_fX_Angle = tmpPT.x * 0.3f;
		m_fX_OriginXAngle += m_fX_Angle;

		if (m_fX_OriginXAngle <= -360 || m_fX_OriginXAngle >= 360)
			m_fX_OriginXAngle = 0;

		dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->MoveAngle(ENGINE::ANGLE_Y, m_fX_Angle);
		
		D3DXMatrixRotationY(&matRot, D3DXToRadian(m_fX_Angle));
		
		memcpy(&matRot._41, &m_pCCamera_Component->Get_LookAt(), sizeof(D3DXVECTOR3));
		
		vDir = m_pCCamera_Component->Get_EyePos() - m_pCCamera_Component->Get_LookAt();
		
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
		m_pCCamera_Component->Set_EyePos(tmpEyePos);
	}

	if (tmpPT.y != 0)
	{
		if (m_pCCamera_Component->Get_Look().z < 0.2f && m_pCCamera_Component->Get_Look().z > 0.f)
		{
			if (m_pCCamera_Component->Get_Look().y < -0.8f)
			{
				if (tmpPT.y > 0)
					return;
			}

			if (m_pCCamera_Component->Get_Look().y > 0.8f)
			{
				if (tmpPT.y < 0)
					return;
			}
		}

		else if (m_pCCamera_Component->Get_Look().z > -0.2f && m_pCCamera_Component->Get_Look().z < 0.f)
		{
			if (m_pCCamera_Component->Get_Look().y < -0.8f)
			{
				if (tmpPT.y > 0)
					return;
			}

			if (m_pCCamera_Component->Get_Look().y > 0.8f)
			{
				if (tmpPT.y < 0)
					return;
			}
		}

		D3DXVECTOR3 tmpRight, tmpUp, tmpLook;
		D3DXVECTOR3 tmpEyePos;
		
		m_fY_Angle = tmpPT.y * 0.3f;
		m_fX_OriginYAngle += m_fY_Angle;
		
		//dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->MoveAngle(ENGINE::ANGLE_X, m_fY_Angle);

		if (m_fX_OriginYAngle <= -80)
			m_fX_OriginYAngle = -80;

		if (m_fX_OriginYAngle >= 80)
			m_fX_OriginYAngle = 80;
		
		D3DXMatrixRotationAxis(&matRot, &m_pCCamera_Component->Get_Right(), D3DXToRadian(m_fY_Angle));
		memcpy(&matRot._41, &m_pCCamera_Component->Get_LookAt(), sizeof(D3DXVECTOR3));
		
		vDir = m_pCCamera_Component->Get_EyePos() - m_pCCamera_Component->Get_LookAt();
		
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
		m_pCCamera_Component->Set_EyePos(tmpEyePos);
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

void CCamera::SetLookAt(D3DXVECTOR3 _LookAt)
{
	m_pCCamera_Component->Set_LookAt(_LookAt);
}

void CCamera::Set_Vertical(float _Vertical)
{
	int a  = rand() % 2;

	switch (a)
	{
	case 0:
	{
		m_fVertical_Move = -_Vertical;
		m_fReverse_Vertical = _Vertical;

		m_bLeft = true;
		break;
	}
	case 1:
	{
		m_fVertical_Move = _Vertical;
		m_fReverse_Vertical = -_Vertical;

		m_bLeft = false;
		break;
	}
	}
}

void CCamera::AimZoom()
{

}

void CCamera::Set_AimZoom(float _ZoomValue)
{
	m_fFov = _ZoomValue;
}

void CCamera::KeyInput()
{
	if (m_eCameraMode != FLY_MODE)
		return;

	if (m_pKeyMgr->KeyPressing(ENGINE::KEY_W))
	{
		Walk(0.3f);
	}

	if (m_pKeyMgr->KeyPressing(ENGINE::KEY_S))
	{
		Walk(-0.3f);
	}

	if (m_pKeyMgr->KeyPressing(ENGINE::KEY_A))
	{
		Starfe(-0.3f);
	}

	if (m_pKeyMgr->KeyPressing(ENGINE::KEY_D))
	{
		Starfe(0.3f);
	}

	if (m_pKeyMgr->KeyPressing(ENGINE::KEY_LSHIFT))
	{
		m_pCCamera_Component->Add_EyePos({ 0 , 0.3f , 0 });
		m_pCCamera_Component->Add_LookAt({ 0 , 0.3f , 0 });
	}

	if (m_pKeyMgr->KeyPressing(ENGINE::KEY_LCTRL))
	{
		m_pCCamera_Component->Add_EyePos({ 0 , -0.3f , 0 });
		m_pCCamera_Component->Add_LookAt({ 0 , -0.3f , 0 });
	}

	if (GetAsyncKeyState(VK_NUMPAD6))
	{
		Yaw(0.5f);
	}

	if (GetAsyncKeyState(VK_NUMPAD4))
	{
		Yaw(-0.5f);
	}

	if (GetAsyncKeyState(VK_NUMPAD8))
	{
		Pitch(-0.5f);
	}

	if (GetAsyncKeyState(VK_NUMPAD2))
	{
		Pitch(0.5f);
	}
}

void CCamera::CamShakeAngle()
{
	if (m_fHorizontal_Move != 0)
	{
		D3DXMATRIX matRot;
		D3DXVECTOR3 tmpRight, tmpUp, tmpLook;
		D3DXVECTOR3 tmpEyePos , vDir;

		D3DXMatrixRotationAxis(&matRot, &m_pCCamera_Component->Get_Right(), D3DXToRadian(m_fHorizontal_Move));
		memcpy(&matRot._41, &m_pCCamera_Component->Get_LookAt(), sizeof(D3DXVECTOR3));

		vDir = m_pCCamera_Component->Get_EyePos() - m_pCCamera_Component->Get_LookAt();

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
		m_pCCamera_Component->Set_EyePos(tmpEyePos);

		m_fHorizontal_Move += m_fReverse_Horizontal * m_pTimeMgr->GetDelta() * 20;

		if (m_fHorizontal_Move > m_fReverse_Horizontal * 0.9f)
			m_fHorizontal_Move = 0;
	}

	if (m_fVertical_Move != 0)
	{
		D3DXMATRIX matRot;
		D3DXVECTOR3 tmpRight, tmpUp, tmpLook;
		D3DXVECTOR3 tmpEyePos, vDir;

		dynamic_cast<ENGINE::CTransform*>(m_pTarget->Get_Component(L"Transform"))->MoveAngle(ENGINE::ANGLE_Y, m_fVertical_Move);

		D3DXMatrixRotationY(&matRot, D3DXToRadian(m_fVertical_Move));

		memcpy(&matRot._41, &m_pCCamera_Component->Get_LookAt(), sizeof(D3DXVECTOR3));

		vDir = m_pCCamera_Component->Get_EyePos() - m_pCCamera_Component->Get_LookAt();

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
		m_pCCamera_Component->Set_EyePos(tmpEyePos);

		if (m_bLeft)
		{
			m_fVertical_Move += m_fReverse_Vertical * m_pTimeMgr->GetDelta() * 20;
		
			if (m_fVertical_Move > m_fReverse_Vertical * 0.9f)
				m_fVertical_Move = 0;
		}

		else if (!m_bLeft)
		{
			m_fVertical_Move += m_fReverse_Vertical * m_pTimeMgr->GetDelta() * 20;
		
			if (m_fVertical_Move < m_fReverse_Vertical * 0.9f)
				m_fVertical_Move = 0;
		}
	}
}

void CCamera::CamShakePos()
{
	if (m_vMaxCamShakePos.y != 0)
	{
		if (m_bCamPosDown)
		{
			m_vCamShakePos.y -= m_vMaxCamShakePos.y * m_pTimeMgr->GetDelta() * 20;
	
			if (m_vCamShakePos.y <= -m_vMaxCamShakePos.y)
			{
				m_bCamPosDown = false;
				m_bCamPosUp = true;
			}
	
		}
	
		if (m_bCamPosUp)
		{
			m_vCamShakePos.y += m_vMaxCamShakePos.y * m_pTimeMgr->GetDelta() * 20;
	
			if (m_vCamShakePos.y >= m_vMaxCamShakePos.y)
			{
				m_bCamPosDown = true;
				m_bCamPosUp = false;
			}
		}
	}

	if (m_vMaxCamShakePos.x != 0)
	{
		if (m_bCamPosRight)
		{
			m_vCamShakePos.x -= m_vMaxCamShakePos.x * m_pTimeMgr->GetDelta() * 10;

			if (m_vCamShakePos.x <= -m_vMaxCamShakePos.x)
			{
				m_bCamPosLeft = true;
				m_bCamPosRight = false;
			}

		}

		if (m_bCamPosLeft)
		{
			m_vCamShakePos.x += m_vMaxCamShakePos.x * m_pTimeMgr->GetDelta() * 10;

			if (m_vCamShakePos.x >= m_vMaxCamShakePos.x)
			{
				m_bCamPosLeft = false;
				m_bCamPosRight = true;
			}
		}
	}

	if (m_vMaxCamShakePos.z != 0)
	{
		if (m_bCamPosFront)
		{
			m_vCamShakePos.z -= m_vMaxCamShakePos.z * m_pTimeMgr->GetDelta() * 20;

			if (m_vCamShakePos.z <= -m_vMaxCamShakePos.z)
			{
				m_bCamPosBack = true;
				m_bCamPosFront = false;
			}

		}

		if (m_bCamPosBack)
		{
			m_vCamShakePos.z += m_vMaxCamShakePos.z * m_pTimeMgr->GetDelta() * 20;

			if (m_vCamShakePos.z >= m_vMaxCamShakePos.z)
			{
				m_bCamPosBack = false;
				m_bCamPosFront = true;
			}
		}
	}
}

int CCamera::Update()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	CamShakeAngle();
	KeyInput();
	SetUp_Zoom();
	SetUp_FirstPerson_ViewPoint();
	SetUp_ViewPoint(m_eCameraViewPoint);
	SetUp_ViewMatrix(&m_MatView);

	D3DXMATRIX matProj;

	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(m_fFov), WINCX / (float)WINCY, 1.f, 1000.f);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_MatView);

	m_pSubject->AddData(D3DTS_VIEW, &m_MatView);
	m_pSubject->AddData(D3DTS_PROJECTION, &matProj);

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
	m_pCCamera_Component->Set_LookAt({ 0,3,1 });

	m_pCCamera_Component->set_MainCamera(true);

	if(m_eCameraViewPoint == THIRD_PERSON )
		m_pCCamera_Component->Set_Distance(5.5f);

	else if (m_eCameraViewPoint == FIRST_PERSON)
		m_pCCamera_Component->Set_Distance(0.5f);


	m_fMax_PlayerRoll_Angle = 25.f;			// 1 인칭 시, 좌우 기울이기 최대 각도
	m_fEyeHeight = 3.f;
	m_fZoom_Min = 0.5f;
	m_fZoom_Max = 10.f;

	m_fFov = 70.f;

	D3DXMATRIX matProj;

	SetUp_ViewMatrix(&m_MatView);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_MatView);

	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(m_fFov), WINCX / (float)WINCY, 1.f, 1000.f);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);

	ShowCursor(false);

	m_pSubject->AddData(D3DTS_VIEW, &m_MatView);
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
