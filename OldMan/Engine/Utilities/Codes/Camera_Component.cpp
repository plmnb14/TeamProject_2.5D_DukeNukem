#include "Camera_Component.h"

USING(ENGINE)

CCamera_Component::CCamera_Component()
{
	ZeroMemory(&m_tCamInfo, sizeof(ENGINE::CAM_INFO));
}

CCamera_Component::~CCamera_Component()
{
}

const D3DXVECTOR3 & CCamera_Component::Get_EyePos() const
{
	return m_tCamInfo.vEyePos;
}

const D3DXVECTOR3 & CCamera_Component::Get_LookAt() const
{
	return m_tCamInfo.vLookAt;
}

const D3DXVECTOR3 & CCamera_Component::Get_Right() const
{
	return m_tCamInfo.vRight;
}

const D3DXVECTOR3 & CCamera_Component::Get_Up() const
{
	return m_tCamInfo.vUp;
}

const D3DXVECTOR3 & CCamera_Component::Get_Look() const
{
	return m_tCamInfo.vLook;
}

const int CCamera_Component::Get_Index() const
{
	return 0;
}

const float CCamera_Component::Get_Distance() const
{
	return m_tCamInfo.fDistance;
}

const bool CCamera_Component::Get_MainCamera() const
{
	return false;
}

void CCamera_Component::Set_EyePos(const D3DXVECTOR3 & _vEyePos)
{
	m_tCamInfo.vEyePos = _vEyePos;
}

void CCamera_Component::Set_LookAt(const D3DXVECTOR3 & _vLookAt)
{
	m_tCamInfo.vLookAt = _vLookAt;
}

void CCamera_Component::Set_Right(const D3DXVECTOR3 & _vRight)
{
	m_tCamInfo.vRight = _vRight;
}

void CCamera_Component::Set_Up(const D3DXVECTOR3 & _vUp)
{
	m_tCamInfo.vUp = _vUp;
}

void CCamera_Component::Set_Look(const D3DXVECTOR3 & _vLook)
{
	m_tCamInfo.vLook = _vLook;
}

void CCamera_Component::Set_Index(int _Index)
{
	m_tCamInfo.iIndex = _Index;
}

void CCamera_Component::Set_Distance(float _Distance)
{
	m_tCamInfo.fDistance = _Distance;
}

void CCamera_Component::set_MainCamera(bool _IsMainCamera)
{
	m_tCamInfo.bMainCamera = _IsMainCamera;
}

void CCamera_Component::Add_Distance(float _Distance)
{
	m_tCamInfo.fDistance += _Distance;
}

void CCamera_Component::LateUpdate()
{
}

CCamera_Component* CCamera_Component::Create()
{
	return new CCamera_Component();
}
