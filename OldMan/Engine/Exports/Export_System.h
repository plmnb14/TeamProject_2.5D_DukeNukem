#pragma once

#ifndef __EXPORT_SYSTEM_H__

#include "GraphicDev.h"
#include "TimeMgr.h"
#include "KeyMgr.h"
#include "FrameMgr.h"
#include "TextureMgr.h"

BEGIN(ENGINE)

inline CGraphicDev* GetGraphicDev();
inline CTimeMgr* GetTimeMgr();
inline CKeyMgr* GetKeyMgr();
inline CFrameMgr* GetFrameMgr();
inline CTextureMgr* GetTextureMgr();

#include "Export_System.inl"

END

#define __EXPORT_SYSTEM_H__
#endif