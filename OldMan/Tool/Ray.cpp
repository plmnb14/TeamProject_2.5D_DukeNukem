#include "stdafx.h"
#include "Ray.h"
#include "Trasform.h"

#include "ToolView.h"
#include "MainFrm.h"

//임시
#include "ToolTerrain.h"

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
	D3DXMatrixIdentity(&matView);
	D3DXMatrixIdentity(&matInverseView);
	ENGINE::GetGraphicDev()->GetDevice()->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matInverseView, 0, &matView);

	D3DXVec3TransformCoord(&r.m_vOrigin, &r.m_vOrigin, &matInverseView);
	D3DXVec3TransformNormal(&r.m_vDirection, &r.m_vDirection, &matInverseView);
	D3DXVec3Normalize(&r.m_vDirection, &r.m_vDirection);
	r.m_eRaySpace = RAY_WORLD;

	return r;
}

D3DXVECTOR3 CRay::GetDirection()
{
	POINT pt = {};
	GetCursorPos(&pt);
	::ScreenToClient(g_hWnd, &pt);
	D3DXVECTOR3 v3 = D3DXVECTOR3((float)pt.x, (float)pt.y, 1.f);
	CRay _Ray = CRay::RayAtWorldSpace((int)v3.x, (int)v3.y);

	return _Ray.m_vDirection;
}

D3DXVECTOR3 CRay::GetMousePos()
{
	POINT pt = {};
	GetCursorPos(&pt);
	::ScreenToClient(g_hWnd, &pt);

	return D3DXVECTOR3((float)pt.x, (float)pt.y, 0.f);
}

bool CRay::IsPicked(CToolTerrain* _pTerrainCube)
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

bool CRay::IsPicked(D3DXVECTOR3& _v0, D3DXVECTOR3& _v1, D3DXVECTOR3& _v2, D3DXVECTOR3& _vPickedPos, D3DXMATRIX _matWorld)
{
	POINT pt = {};
	GetCursorPos(&pt);
	::ScreenToClient(g_hWnd, &pt);
	D3DXVECTOR3 v3 = D3DXVECTOR3((float)pt.x, (float)pt.y, 1.f);
	*this = RayAtWorldSpace((int)v3.x, (int)v3.y);

	D3DXVECTOR3 v0, v1, v2;
	D3DXVec3TransformCoord(&v0, &_v0, &_matWorld);
	D3DXVec3TransformCoord(&v1, &_v1, &_matWorld);
	D3DXVec3TransformCoord(&v2, &_v2, &_matWorld);

	float u = 0.f, v = 0.f, t = 0.f;
	BOOL b;
	b = D3DXIntersectTri(&v0, &v1, &v2, &m_vOrigin, &m_vDirection, &u, &v, &t);
	_vPickedPos = m_vOrigin + (t * m_vDirection);

	// warning C4800: 'BOOL': 'true' 또는 'false'로 bool 값을 강제하고 있습니다(성능 경고).
	// 3항 연산자로 해결
	return b ? true : false;
}
