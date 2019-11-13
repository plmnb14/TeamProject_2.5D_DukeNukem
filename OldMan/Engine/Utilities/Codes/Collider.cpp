#include "Collider.h"

USING(ENGINE)

CCollider::CCollider()
	: m_pTarget(nullptr)
{
	ZeroMemory(&m_tBoxCollider, sizeof(BOXCOL));
}

CCollider::~CCollider()
{
}

void CCollider::LateUpdate()
{
	Set_CenterPos();
}

void CCollider::SetUp_Box()
{
	m_tBoxCollider.vMinPos = m_tBoxCollider.vCenterPos;
	m_tBoxCollider.vMaxPos = m_tBoxCollider.vCenterPos;

	m_tBoxCollider.vMinPos += { -m_tBoxCollider.fRadius[ENGINE::RADIUS_X] , -m_tBoxCollider.fRadius[ENGINE::RADIUS_Y] , -m_tBoxCollider.fRadius[ENGINE::RADIUS_Z] };
	m_tBoxCollider.vMaxPos += { m_tBoxCollider.fRadius[ENGINE::RADIUS_X] , m_tBoxCollider.fRadius[ENGINE::RADIUS_Y] , m_tBoxCollider.fRadius[ENGINE::RADIUS_Z] };
}

void CCollider::Set_Radius(float* _Radius)
{
	m_tBoxCollider.fRadius[ENGINE::RADIUS_X] = _Radius[0];
	m_tBoxCollider.fRadius[ENGINE::RADIUS_Y] = _Radius[1];
	m_tBoxCollider.fRadius[ENGINE::RADIUS_Z] = _Radius[2];
}

void CCollider::Set_Length(float * _Length)
{
	m_tBoxCollider.flength[ENGINE::LENGTH_X] = _Length[0];
	m_tBoxCollider.flength[ENGINE::LENGTH_Y] = _Length[1];
	m_tBoxCollider.flength[ENGINE::LENGTH_Z] = _Length[2];
}

void CCollider::Set_CenterPos()
{
	m_tBoxCollider.vCenterPos = { m_tBoxCollider.vUnderPos.x ,
								  m_tBoxCollider.vUnderPos.y - m_tBoxCollider.fRadius[ENGINE::RADIUS_Y],
								  m_tBoxCollider.vUnderPos.z };
}

void CCollider::Set_UnderPos(D3DXVECTOR3 _UnderPos)
{
	m_tBoxCollider.vUnderPos = _UnderPos;
}

bool CCollider::Check_AABB(ENGINE::BOXCOL* _TargetCollider)
{
	if (m_tBoxCollider.vMinPos.x <= _TargetCollider->vMaxPos.x && m_tBoxCollider.vMaxPos.x >= _TargetCollider->vMinPos.x &&
		m_tBoxCollider.vMinPos.y <= _TargetCollider->vMaxPos.y && m_tBoxCollider.vMaxPos.y >= _TargetCollider->vMinPos.y &&
		m_tBoxCollider.vMinPos.z <= _TargetCollider->vMaxPos.z && m_tBoxCollider.vMaxPos.z >= _TargetCollider->vMinPos.z)
	{
		//m_tBoxCollider.flength[0] = m_tBoxCollider.vMinPos.x - _TargetCollider->vMaxPos.x;
		//m_tBoxCollider.flength[1] = m_tBoxCollider.vMinPos.y - _TargetCollider->vMaxPos.y;
		//m_tBoxCollider.flength[2] = m_tBoxCollider.vMinPos.z - _TargetCollider->vMaxPos.z;

		m_tBoxCollider.flength[0] = _TargetCollider->vMaxPos.x - m_tBoxCollider.vMinPos.x;
		m_tBoxCollider.flength[1] = _TargetCollider->vMaxPos.y - m_tBoxCollider.vMinPos.y;
		m_tBoxCollider.flength[2] = _TargetCollider->vMaxPos.z - m_tBoxCollider.vMinPos.z;

		m_tBoxCollider.bIsCollision = true;
		return true;
	}

	else
	{
		m_tBoxCollider.flength[0] = 0;
		m_tBoxCollider.flength[1] = 0;
		m_tBoxCollider.flength[2] = 0;

		m_tBoxCollider.bIsCollision = false;
		return false;
	}
}

CCollider* CCollider::Create()
{
	return new CCollider();
}
