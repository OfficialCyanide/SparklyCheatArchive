#pragma once
#include "SDK.h"
#include "Entity.h"

class Trigger
{
public:
	float autoshoot = false;

	string key = "None";
	string priority = "Head";

	float backstab = false;

	float airblast = false;
	float aimproj = false;

	float ignoreInvis = false;
	float ignoreDisguise = false;
	float ignoreUber = true;
	float ignoreBonk = true;

	float aimSentry = true;
	float aimDispenser = true;
	float aimTeleport = true;
	
	void command(CUserCmd* pCommand, CPlayer* pLocal);
private:
	bool shouldTarget(CBaseEntity* entity);
};
extern Trigger gTrigger;