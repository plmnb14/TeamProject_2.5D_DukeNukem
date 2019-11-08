#pragma once

#ifndef __EXPORT_SYSTEM_H__

#include "GraphicDev.h"
#include "TimeMgr.h"

BEGIN(ENGINE)

inline CGraphicDev* GetGraphicDev();
inline CTimeMgr* GetTimeMgr();

#include "Export_System.inl"

END

#define __EXPORT_SYSTEM_H__
#endif