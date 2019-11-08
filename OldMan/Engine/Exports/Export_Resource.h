#pragma once

#ifndef __EXPORT_RESOURCE_H__

#include "ResourceMgr.h"

BEGIN(ENGINE)

inline CResourceMgr* GetResourceMgr();

#include "Export_Resource.inl"

END

#define __EXPORT_UTILITIES_H__
#endif