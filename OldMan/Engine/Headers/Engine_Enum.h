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

	enum RADIUS_TYPE
	{
		RADIUS_X,
		RADIUS_Y,
		RADIUS_Z,
		RADIUS_END
	};

	enum ACCEL_TYPE
	{
		ACCEL_X,
		ACCEL_Y,
		ACCEL_Z,
		ACCEL_END
	};

	enum LENGTH_TYPE
	{
		LENGTH_X,
		LENGTH_Y,
		LENGTH_Z,
		LENGTH_END
	};

	enum BULLET_TYPE
	{
		HITSCAN,
		PROJECTILE
	};

	enum WEAPON_TAG
	{
		NO_WEAPON, MELLE, REVOLVER, RIFLE, SHOTGUN, LUNCHER, SMG,MONSTER_REVOLVER 

	};

	enum OBJECT_TYPE
	{
		TERRAIN,
		WALL,
		PLAYER,
		MONSTER,
		BULLET_PLAYER,
		BULLET_MONSTER,
		PROPS,
		WEAPON,
		PICKUP,
		EQUIPMENT,
		TRIGGER,
		VFX,
		CAMERA,
		UI
	};

	enum TERRAIN_TYPE 
	{ 
		TERRAIN_CUBE,
		TERRAIN_WALL,
		TERRAIN_RECT,
		TERRAIN_END
	};

	enum COLLISION_TYPE
	{
		COLLISION_AABB,
		COLLISION_SHPERE,
		COLLISION_PLANE,
		COLLISION_LINE,
		COLLISION_CAPSHULE,
		COLLISION_END
	};

}

#define __ENGINE_ENUM_H__
#endif