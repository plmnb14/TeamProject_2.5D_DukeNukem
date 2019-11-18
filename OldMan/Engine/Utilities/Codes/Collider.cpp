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

void CCollider::LateUpdate(D3DXVECTOR3 _Pos)
{
	Set_CenterPos(_Pos);
	Set_UnderPos();
	SetUp_Box();
}

void CCollider::SetUp_Box()
{
	m_tBoxCollider.vMinPos = {m_tBoxCollider.vCenterPos - m_tBoxCollider.vRadius };
	m_tBoxCollider.vMaxPos = {m_tBoxCollider.vCenterPos + m_tBoxCollider.vRadius };
}

void CCollider::Set_Radius(D3DXVECTOR3 _Radius)
{
	m_tBoxCollider.vRadius = _Radius;
}

void CCollider::Set_Length(D3DXVECTOR3 _Length)
{
	m_tBoxCollider.vLength = _Length;
}

void CCollider::Set_CenterPos(D3DXVECTOR3 _CenterPos)
{
	m_tBoxCollider.vCenterPos = _CenterPos;
}

void CCollider::Set_UnderPos()
{
	m_tBoxCollider.vUnderPos = { m_tBoxCollider.vCenterPos.x , 
								 m_tBoxCollider.vCenterPos.y - m_tBoxCollider.vRadius.y ,
								 m_tBoxCollider.vCenterPos.z };
}

void CCollider::Set_Dynamic(bool _Dynamic)
{
	m_tBoxCollider.bIsDynamic = _Dynamic;
}

void CCollider::Set_Trigger(bool _Trigger)
{
	m_tBoxCollider.bIsTrigger = _Trigger;
}

void CCollider::Set_IsCollision(bool _Collision)
{
	m_tBoxCollider.bIsCollision = _Collision;
}

CCollider* CCollider::Create()
{
	return new CCollider();
}
