#pragma once
#ifndef __RAY_H__

class CToolTerrainCube;
class CRay
{
public:
	CRay();
	~CRay();

	enum eRAYSPACE
	{
		RAY_NONE,
		RAY_VIEW,
		RAY_WORLD,
		RAY_LOCAL
	};

public:
	static CRay RayAtViewSpace(int _iScreecX, int _iScreecY);
	static CRay RayAtWorldSpace(int _iScreecX, int _iScreecY);
	bool IsPicked(CToolTerrainCube* _pTerrainCube);
	bool IsPicked(D3DXVECTOR3& _v0, D3DXVECTOR3& _v1, D3DXVECTOR3& _v2, D3DXVECTOR3& _vPickedPos);

public:
	D3DXVECTOR3 m_vOrigin;
	D3DXVECTOR3 m_vDirection;
	eRAYSPACE m_eRaySpace;

};

#define __RAY_H__
#endif
