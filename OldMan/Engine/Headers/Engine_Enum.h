#pragma once

#ifndef __ENGINE_ENUM_H__

namespace ENGINE
{
	enum RESOURCE_TYPE
	{
		RESOURCE_STATIC,
		RESOURCE_DYNAMIC,
		RESOURCE_END
	};

	enum TEXTRUE_TYPE
	{
		TEX_NORMAL,
		TEX_CUBE,
		TEX_END
	};

	enum ANGLE_TYPE
	{
		ANGLE_X,
		ANGLE_Y,
		ANGLE_Z,
		ANGLE_END
	};

	enum OBJECT_TYPE
	{
		TERRAIN,
		WALL,
		PLAYER,
		MONSTER,
		BULLET,
		PROPS,
		VFX,
		CAMERA,
		UI
	};
}

#define __ENGINE_ENUM_H__
#endif