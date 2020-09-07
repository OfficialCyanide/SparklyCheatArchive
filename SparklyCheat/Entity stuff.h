#pragma once
#include "SDK.h"

class EntityStuff
{
public:
	Vector3 EstimateAbsVelocity(CBaseEntity *ent);
	CBaseEntity* gActiveWeapon(CBaseEntity* player);
};
extern EntityStuff gEntStuff;