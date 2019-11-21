#pragma once

#ifndef __TRANSFORM_H__

#include "Component.h"

BEGIN(ENGINE)

class ENGINE_DLL CTransform : public CComponent
{
private:
	explicit CTransform(const D3DXVECTOR3& vLook);

public:
	virtual ~CTransform();

public:
	const D3DXVECTOR3& GetPos() const;
	const D3DXVECTOR3& GetDir() const;
	const D3DXVECTOR3& GetSize() const;
	const D3DXMATRIX& GetWorldMatrix() const;
	const float& GetAngle(ANGLE_TYPE eAxis) const;
	float* GetAllAngle();

public:
	void SetPos(const D3DXVECTOR3& vPos);
	void SetDir(const D3DXVECTOR3& vDir);
	void SetSize(const D3DXVECTOR3& vSize);
	void SetWorldMatrix(const D3DXMATRIX& matWorld);
	void SetAngle(const float& fAngle, ANGLE_TYPE eAxis);

public:
	void MovePos(float fSpeed);
	void Move_AdvancedPos(D3DXVECTOR3& _vDir, float _fSpeed);
	void Move_AdvancedPos_Vec3(D3DXVECTOR3 & _vPos);
	void Move_PosY(float fSpeed);
	void MoveAngle(ANGLE_TYPE eAxis, float fSpeed);

public:
	virtual void LateUpdate();

public:
	static CTransform* Create(const D3DXVECTOR3& vLook);

private:
	D3DXVECTOR3 m_vPos;
	D3DXVECTOR3 m_vDir;
	D3DXVECTOR3 m_vLook;
	D3DXVECTOR3 m_vSize;
	D3DXMATRIX m_matScale;
	D3DXMATRIX m_matRot[ANGLE_END];
	D3DXMATRIX m_matTrans;
	D3DXMATRIX m_matWorld;
	float m_fAngle[ANGLE_END];
};

END

#define __TRANSFORM_H__
#endif