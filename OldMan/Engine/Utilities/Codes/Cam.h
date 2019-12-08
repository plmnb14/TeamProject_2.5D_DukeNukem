#pragma once

#ifndef __Cam_H__

#include "Component.h"

BEGIN(ENGINE)

class ENGINE_DLL CCam : public CComponent
{
private:
	explicit CCam();

public:
	virtual ~CCam();

public:
	const D3DXVECTOR3&	Get_EyePos() const;
	const D3DXVECTOR3&	Get_LookAt() const;
	const D3DXVECTOR3&	Get_Right() const;
	const D3DXVECTOR3&	Get_Up() const;
	const D3DXVECTOR3&	Get_Look() const;

public:
	const int		Get_Index() const;
	const float		Get_Distance() const;
	const bool		Get_MainCamera() const;

public:
	void Set_EyePos	(const D3DXVECTOR3& _vEyePos);
	void Set_LookAt	(const D3DXVECTOR3& _vLookAt);
	void Set_Right	(const D3DXVECTOR3& _vRight);
	void Set_Up		(const D3DXVECTOR3& _vUp);
	void Set_Look	(const D3DXVECTOR3& _vLook);

public:
	void	Set_Index(int _Index);
	void	Set_Distance(float _Distance);
	void	set_MainCamera(bool _IsMainCamera);

public:
	void Add_EyePos(const D3DXVECTOR3& _vEyePos);
	void Add_LookAt(const D3DXVECTOR3& _vLookAt);
	void Add_Distance(float _Distance);

public:
	virtual void LateUpdate();

public:
	static CCam* Create();

private:
	ENGINE::CAM_INFO	m_tCamInfo;
};

END

#define __Cam_H__
#endif