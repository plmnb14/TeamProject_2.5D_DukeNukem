#include "Cam.h"

USING(ENGINE)

CCam::CCam()
{
	ZeroMemory(&m_tCamInfo, sizeof(ENGINE::CAM_INFO));
}

CCam::~CCam()
{
}

const D3DXVECTOR3 & CCam::Get_EyePos() const
{
	return m_tCamInfo.vEyePos;
}

const D3DXVECTOR3 & CCam::Get_LookAt() const
{
	return m_tCamInfo.vLookAt;
}

const D3DXVECTOR3 & CCam::Get_Right() const
{
	return m_tCamInfo.vRight;
}

const D3DXVECTOR3 & CCam::Get_Up() const
{
	return m_tCamInfo.vUp;
}

const D3DXVECTOR3 & CCam::Get_Look() const
{
	return m_tCamInfo.vLook;
}

const int CCam::Get_Index() const
{
	return m_tCamInfo.iIndex;
}

const float CCam::Get_Distance() const
{
	return m_tCamInfo.fDistance;
}

const bool CCam::Get_MainCamera() const
{
	if (m_tCamInfo.bMainCamera == true)
		return true;

	return false;
}

void CCam::Set_EyePos(const D3DXVECTOR3 & _vEyePos)
{
	m_tCamInfo.vEyePos = _vEyePos;
}

void CCam::Set_LookAt(const D3DXVECTOR3 & _vLookAt)
{
	m_tCamInfo.vLookAt = _vLookAt;
}

void CCam::Set_Right(const D3DXVECTOR3 & _vRight)
{
	m_tCamInfo.vRight = _vRight;
}

void CCam::Set_Up(const D3DXVECTOR3 & _vUp)
{
	m_tCamInfo.vUp = _vUp;
}

void CCam::Set_Look(const D3DXVECTOR3 & _vLook)
{
	m_tCamInfo.vLook = _vLook;
}

void CCam::Set_Index(int _Index)
{
	m_tCamInfo.iIndex = _Index;
}

void CCam::Set_Distance(float _Distance)
{
	m_tCamInfo.fDistance = _Distance;
}

void CCam::set_MainCamera(bool _IsMainCamera)
{
	m_tCamInfo.bMainCamera = _IsMainCamera;
}

void CCam::Add_EyePos(const D3DXVECTOR3 & _vEyePos)
{
	m_tCamInfo.vEyePos += _vEyePos;
}

void CCam::Add_LookAt(const D3DXVECTOR3 & _vLookAt)
{
	m_tCamInfo.vLookAt += _vLookAt;
}

void CCam::Add_Distance(float _Distance)
{
	m_tCamInfo.fDistance += _Distance;
}

void CCam::LateUpdate()
{
}

CCam* CCam::Create()
{
	return new CCam();
}
