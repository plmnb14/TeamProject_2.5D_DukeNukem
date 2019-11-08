#pragma once

#ifndef __EXPORT_UTILITIES_H__

#include "Management.h"

BEGIN(ENGINE)

inline CManagement* GetManagement();

#include "Export_Utilities.inl"

END

#define __EXPORT_UTILITIES_H__
#endif