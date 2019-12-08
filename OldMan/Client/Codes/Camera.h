#pragma once

#ifndef __CAMERA_H__

#include "GameObject.h"

namespace ENGINE
{
	class CTimeMgr;
	class CKeyMgr;
	class CCameraSubject;
	class CCam;
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
	void Set_AimZoom(float _ZoomValue);
	void AimZoom();

public:
	void KeyInput();
	void CamShakeAngle();
	void CamShakePos();
	void Set_CamShakePos(D3DXVECTOR3 _MovePos) { m_vCamShakePos = _MovePos; m_vMaxCamShakePos = _MovePos; }
	D3DXVECTOR3 Get_CamShakePos() { return m_vMaxCamShakePos; };
	void Set_CamYPos(float _y) { m_fCam_PosY = _y; };




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
	D3DXVECTOR3			m_vCamShakePos;
	D3DXVECTOR3			m_vMaxCamShakePos;

private:
	ENGINE::CCam*	m_pCCam;
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
	bool m_bCamPosUp;
	bool m_bCamPosDown;
	bool m_bCamPosRight;
	bool m_bCamPosLeft;
	bool m_bCamPosFront;
	bool m_bCamPosBack;

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
	float m_fFoot_to_Head;


private:
	float m_fMax_PlayerRoll_Angle;
	float m_fCamShake_Y;
	float m_fCamShake_X;
	float m_fCam_PosY;

	private:
		float m_fFov;
};

#define __CAMERA_H__
#endif

