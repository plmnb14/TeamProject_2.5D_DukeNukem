#pragma once

#ifndef __CAMERA_H__

#include "GameObject.h"

namespace ENGINE
{
	class CCamera_Component;
}

class CCamera : public ENGINE::CGameObject
{

private:
	explicit CCamera(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CCamera();

public:
	enum CameraMode
	{
		LAND_MODE, FLY_MODE, SPYCAM_MODE, END_MODE
	};

	enum CameraViewPoint
	{
		FIRST_PERSON, THIRD_PERSON
	};

public:
	void Starfe(float _Speed);
	void Fly(float _Speed);
	void Walk(float _Speed);

public:
	void Pitch(float _Angle);
	void Yaw(float _Angle);
	void Roll(float _Angle);

public:
	void GetViewMatrix(D3DXMATRIX* _ViewMatrix);
	void SetCameraMode(CameraMode _CameraType);

public:
	void SetUp_ViewPoint(CameraViewPoint _CameraViewPoint);
	void SetUp_FirstPerson_ViewPoint();


public:
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

private:
	virtual HRESULT Initialize() override;
	virtual void Release() override;

private:
	HRESULT AddComponent();

public:
	void Set_Target(CGameObject* _Target) { m_pTarget = _Target; };

public:
	static CCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _Target);

private:
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	CameraMode			m_eCameraMode;
	CameraViewPoint		m_eCameraViewPoint;

private:
	ENGINE::CAM_INFO			m_tCameraInfo;
	ENGINE::CCamera_Component*	m_pCCamera_Component;

private:
	CGameObject*			m_pTarget;	// 추후 Obj Target 으로 변경 예정


private:
	float m_fMax_PlayerRoll_Angle;
	float m_fCamShake_Y;
};

#define __CAMERA_H__
#endif

