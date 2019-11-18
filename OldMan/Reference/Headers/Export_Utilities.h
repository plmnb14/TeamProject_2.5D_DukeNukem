#pragma once

#ifndef __EXPORT_UTILITIES_H__

#include "Management.h"
#include "CameraSubject.h"
#include "PlayerSubject.h"

BEGIN(ENGINE)

inline CManagement* GetManagement();
inline CCameraSubject* GetCameraSubject();
inline CPlayerSubject* GetPlayerSubject();

#include "Export_Utilities.inl"

END

#define __EXPORT_UTILITIES_H__
#endif