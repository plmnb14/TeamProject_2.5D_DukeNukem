#include "stdafx.h"
#include "Ray.h"
#include "Trasform.h"
//юс╫ц
#include "ToolTerrainCube.h"

CRay::CRay()
	:m_vOrigin(0, 0, 0), m_vDirection(0, 0, 0)
{
}


CRay::~CRay()
{
}

CRay CRay::RayAtViewSpace(int _iScreecX, int _iScreecY)
{
	D3DVIEWPORT9 vp;
	ENGINE::GetGraphicDev()->GetDevice()->GetViewport(&vp);

	D3DXMATRIX matProj;

	ENGINE::GetGraphicDev()->GetDevice()->GetTransform(D3DTS_PROJECTION, &matProj);

	CRay r;
	r.m_vDirection.x = (((_iScreecX * 2.f) / vp.Width) - 1.f) / matProj._11;
	r.m_vDirection.y = ((-(_iScreecY * 2.f) / vp.Height) + 1.f) / matProj._22;
	r.m_vDirection.z = 1.f;
	r.m_eRaySpace = RAY_VIEW;

	return r;
}

CRay CRay::RayAtWorldSpace(int _iScreecX, int _iScreecY)
{
	CRay r = CRay::RayAtViewSpace(_iScreecX, _iScreecY);

	D3DXMATRIX matView, matInverseView;
	ENGINE::GetGraphicDev()->GetDevice()->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matInverseView, 0, &matView);

	D3DXVec3TransformCoord(&r.m_vOrigin, &r.m_vOrigin, &matInverseView);
	D3DXVec3TransformNormal(&r.m_vDirection, &r.m_vDirection, &matInverseView);
	D3DXVec3Normalize(&r.m_vDirection, &r.m_vDirection);
	r.m_eRaySpace = RAY_WORLD;

	return r;
}

bool CRay::IsPicked(CToolTerrainCube* _pTerrainCube)
{
	CRay r = (*this);

	D3DXMATRIX matInverseWorld;
	D3DXMatrixIdentity(&matInverseWorld);
	matInverseWorld._41 = dynamic_cast<ENGINE::CTransform*>(_pTerrainCube->Get_Component(L"Transform"))->GetPos().x;
	matInverseWorld._42 = dynamic_cast<ENGINE::CTransform*>(_pTerrainCube->Get_Component(L"Transform"))->GetPos().y;
	matInverseWorld._43 = dynamic_cast<ENGINE::CTransform*>(_pTerrainCube->Get_Component(L"Transform"))->GetPos().z;

	D3DXVec3TransformCoord(&r.m_vOrigin, &r.m_vOrigin, &matInverseWorld);
	D3DXVec3TransformNormal(&r.m_vDirection, &r.m_vDirection, &matInverseWorld);

	float vv = D3DXVec3Dot(&r.m_vDirection, &r.m_vDirection);
	float qv = D3DXVec3Dot(&r.m_vOrigin, &r.m_vDirection);
	float qq = D3DXVec3Dot(&r.m_vOrigin, &r.m_vOrigin);
	float rr = dynamic_cast<ENGINE::CTransform*>(_pTerrainCube->Get_Component(L"Transform"))->GetSize().x;

	return qv * qv - vv * (qq - rr) >= 0;
}

bool CRay::IsPicked(D3DXVECTOR3& _v0, D3DXVECTOR3& _v1, D3DXVECTOR3& _v2, D3DXVECTOR3& _vPickedPos)
{
	float u, v, t;
	bool b = D3DXIntersectTri(&_v0, &_v1, &_v2, &m_vOrigin, &m_vDirection, &u, &v, &t);
	_vPickedPos = m_vOrigin + (t * m_vDirection);

	return b;
}
