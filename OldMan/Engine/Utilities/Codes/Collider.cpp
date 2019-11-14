#include "Collider.h"

USING(ENGINE)

CCollider::CCollider()
	: m_pTarget(nullptr) , m_eCollisionType(ENGINE::COLLISION_AABB)
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

	m_tBoxCollider.vMinPos += { -m_tBoxCollider.vRadius };
	m_tBoxCollider.vMaxPos += { m_tBoxCollider.vRadius };
}

void CCollider::Set_Radius(D3DXVECTOR3 _Radius)
{
	m_tBoxCollider.vRadius = _Radius;
}

void CCollider::Set_Length(D3DXVECTOR3 _Length)
{
	m_tBoxCollider.vLength = _Length;
}

void CCollider::Set_CenterPos()
{
	m_tBoxCollider.vCenterPos = { m_tBoxCollider.vUnderPos.x ,
								  m_tBoxCollider.vUnderPos.y - m_tBoxCollider.vRadius.y,
								  m_tBoxCollider.vUnderPos.z };
}

void CCollider::Set_UnderPos(D3DXVECTOR3 _UnderPos)
{
	m_tBoxCollider.vUnderPos = _UnderPos;
}

void CCollider::Set_Dynamic(bool _Dynamic)
{
	m_tBoxCollider.bIsDynamic = _Dynamic;
}

void CCollider::Set_Trigger(bool _Trigger)
{
	m_tBoxCollider.bIsTrigger = _Trigger;
}

bool CCollider::Check_AABB(ENGINE::BOXCOL* _DistCollider , ENGINE::BOXCOL* _TargetCollider)
{
	if (_DistCollider->vMinPos.x <= _TargetCollider->vMaxPos.x && _DistCollider->vMaxPos.x >= _TargetCollider->vMinPos.x &&
		_DistCollider->vMinPos.y <= _TargetCollider->vMaxPos.y && _DistCollider->vMaxPos.y >= _TargetCollider->vMinPos.y &&
		_DistCollider->vMinPos.z <= _TargetCollider->vMaxPos.z && _DistCollider->vMaxPos.z >= _TargetCollider->vMinPos.z)
	{
		m_tBoxCollider.bIsCollision = true;
		return true;
	}

	else
	{
		m_tBoxCollider.bIsCollision = false;
		return false;
	}
}

CCollider* CCollider::Create()
{
	return new CCollider();
}
