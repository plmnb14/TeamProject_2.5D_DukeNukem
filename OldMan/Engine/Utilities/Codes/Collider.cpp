#include "Collider.h"

USING(ENGINE)

CCollider::CCollider()
	: m_pTarget(nullptr) , m_eCollisionType(ENGINE::COLLISION_AABB),
	m_pVtx(nullptr)
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

void CCollider::Set_CollisionVertex(ENGINE::CTransform* pTarget, ENGINE::VTX_TEX* pTerrainVtx)
{
	m_pVtx = pTerrainVtx;
}

void CCollider::Set_PlaneVtx(float* _Angle, D3DXMATRIX _World)
{
	D3DXVECTOR3 tmpPos = m_tBoxCollider.vCenterPos;
	D3DXVECTOR3 tmpRad = m_tBoxCollider.vRadius;
	D3DXVECTOR3 tmpPlaneVtx[4] = {};

	tmpPlaneVtx[0] = { -tmpRad.x, tmpRad.y , -tmpRad.z };
	tmpPlaneVtx[1] = { tmpRad.x, tmpRad.y , tmpRad.z };
	tmpPlaneVtx[2] = { tmpRad.x, -tmpRad.y , tmpRad.z };
	tmpPlaneVtx[3] = { -tmpRad.x, -tmpRad.y , -tmpRad.z };

	//m_vPlaneVtx[0] = { tmpPos.x - tmpRad.x, tmpPos.y + tmpRad.y , tmpPos.z };
	//m_vPlaneVtx[1] = { tmpPos.x + tmpRad.x, tmpPos.y + tmpRad.y , tmpPos.z };
	//m_vPlaneVtx[2] = { tmpPos.x + tmpRad.x, tmpPos.y - tmpRad.y , tmpPos.z };
	//m_vPlaneVtx[3] = { tmpPos.x - tmpRad.x, tmpPos.y - tmpRad.y , tmpPos.z };

	//m_vPlaneVtx[0].x = (cosf(_Angle[1]) * m_vPlaneVtx[0].x - sinf(_Angle[1]) * m_vPlaneVtx[0].z);
	//m_vPlaneVtx[0].z = (sinf(_Angle[1]) * m_vPlaneVtx[0].x + cosf(_Angle[1]) * m_vPlaneVtx[0].z);
	//
	//m_vPlaneVtx[3].x = (cosf(_Angle[1]) * m_vPlaneVtx[3].x - sinf(_Angle[1]) * m_vPlaneVtx[3].z);
	//m_vPlaneVtx[3].z = (sinf(_Angle[1]) * m_vPlaneVtx[3].x + cosf(_Angle[1]) * m_vPlaneVtx[3].z);

	for (int i = 0; i < 4; i++)
	{
		D3DXVec3TransformCoord(&m_vPlaneVtx[i], &tmpPlaneVtx[i], &_World);
	}
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
