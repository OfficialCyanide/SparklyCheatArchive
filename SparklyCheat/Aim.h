#pragma once
#include "Entity.h"

class aim
{
public:
	void command(CUserCmd* pCommand, CPlayer* pLocal);

	float enabled = false;

	string key = "None";
	string method = "FOV";

	string priority = "Head";
	float noScan = false;

	float autoShoot = false;
	float silent = true;
	float slowAim = 0;

	float fov = 45;
	float scopedOnly = false;
	float showTarget = true;
	float multipoint = false;

	float ignoreInvis = false;
	float ignoreDisguise = false;
	float ignoreUber = true;
	float ignoreBonk = true;

	float aimSentry = true;
	float aimDispenser = true;
	float aimTeleport = true;
	float wrangler = false;

	int target = -1;

	void silentMovementFix(CUserCmd *pCommand, Vector3 angles);
private:
	bool shouldTarget(CBaseEntity* entity);
};
extern aim gAim;