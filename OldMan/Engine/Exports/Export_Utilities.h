#pragma once

#ifndef __EXPORT_UTILITIES_H__

#include "Management.h"
#include "CameraSubject.h"

BEGIN(ENGINE)

inline CManagement* GetManagement();
inline CCameraSubject* GetCameraSubject();

#include "Export_Utilities.inl"

END

#define __EXPORT_UTILITIES_H__
#endif