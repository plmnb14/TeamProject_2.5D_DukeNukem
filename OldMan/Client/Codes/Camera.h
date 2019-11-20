#pragma once

#ifndef __CAMERA_H__

#include "GameObject.h"

namespace ENGINE
{
	class CTimeMgr;
	class CKeyMgr;
	class CCameraSubject;
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
	void SetUp_ViewMatrix(D3DXMATRIX* _ViewMatrix);
	void SetCameraMode(CameraMode _CameraType);

public:
	void SetUp_ViewPoint(CameraViewPoint _CameraViewPoint);
	void SetUp_FirstPerson_ViewPoint();
	void SetUp_Zoom();
	void SetUp_MouseRotate();


public:
	float Get_YAngle() { return m_fX_OriginYAngle; };
	float Get_XAngle() { return m_fX_OriginXAngle; };
	D3DXVECTOR3 Get_Pos();
	D3DXVECTOR3 Get_LookAt();
	D3DXVECTOR3 Get_Look();
	D3DXVECTOR3 Get_Right();
	D3DXVECTOR3 Get_Up();
	CameraViewPoint Get_ViewPoint() { return m_eCameraViewPoint; }

public:
	void SetLookAt(D3DXVECTOR3 _LookAt);
	void Set_Hotizontal(float _Horizontal) { m_fHorizontal_Move = -_Horizontal; m_fReverse_Horizontal = _Horizontal; };
	void Set_Vertical(float _Vertical);

public:
	void KeyInput();
	void CamShake();



public:
	virtual int Update() override;
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
	D3DXMATRIX			m_MatView;

private:
	ENGINE::CCamera_Component*	m_pCCamera_Component;
	ENGINE::CTimeMgr*			m_pTimeMgr;
	ENGINE::CKeyMgr*			m_pKeyMgr;
	ENGINE::CCameraSubject*		m_pSubject;

private:
	CGameObject*	m_pTarget;	// 추후 Obj Target 으로 변경 예정

private:
	float m_fVertical_Move;
	float m_fHorizontal_Move;
	float m_fReverse_Vertical;
	float m_fReverse_Horizontal;

private:
	bool m_bLeft;

private:
	float m_fEyeHeight;
	float m_fZoom_Max;
	float m_fZoom_Min;
	float m_fZoom_Speed;

private:
	float m_fX_Angle;
	float m_fY_Angle;
	float m_fZ_Angle;

	float m_fX_OriginXAngle;
	float m_fX_OriginYAngle;


private:
	float m_fMax_PlayerRoll_Angle;
	float m_fCamShake_Y;
	float m_fCamShake_X;
};

#define __CAMERA_H__
#endif

