#include "Hitscan.h"
#include "Aim.h"
#include "Entity stuff.h"
#include "CDrawManager.h"
#include "Ray tracing.h"
#include <string>

hitscan gHitscan;

static Vector3 offset[7] =
{
	Vector3(0, 0, 0),
	Vector3(3, 0, 0),
	Vector3(-3, 0, 0),
	Vector3(0, 3, 0),
	Vector3(0, -3, 0),
	Vector3(0, 0, 3),
	Vector3(0, 0, -3)
};

bool hitscan::bestHit(CBaseEntity* target, Vector3 &hitPos)
{
	CBaseEntity* you = GetBaseEntity(me);
	Vector3 localEyepos = ((CPlayer*)you)->EyePos();
	ent_id entType = target->Type();

	CPlayer* player;
	if (target->ToPlayer(player))
	{
		vector<int> bestHit = bestHitbox(gAim.priority);
		Vector3 hitboxPos;
		for (int i = 0; i < bestHit.size(); i++)
		{
			hitboxPos = player->HitboxPos(bestHit[i]);
			if (getProjSpeed(gEntStuff.gActiveWeapon(you)) == -1)
			{
				int amount = gAim.multipoint ? 7 : 1;
				for (int a = 0; a < amount; a++)
				{
					if (canHit(you, player, localEyepos, hitboxPos + offset[a]))
					{
						hitPos = hitboxPos + offset[a];
						return true;
					}
				}
			}
			else
			{
				Vector3 prediction = getPredPos(hitboxPos, player);
				if (predVischeck(you, localEyepos, prediction))
				{
					hitPos = prediction;
					return true;
				}
			}
		}

		if (!gAim.noScan)
		{
			for (int i = 0; i < 18; i++)
			{
				bool skip = false;
				// See if we already did a vischeck at i for MAX EFFICIENCY
				for (int a = 0; a < bestHit.size(); a++)
				{
					if (bestHit[a] == i)
					{
						skip = true;
						break;
					}
				}
				if (skip) continue;

				hitboxPos = player->HitboxPos(i);
				if (getProjSpeed(gEntStuff.gActiveWeapon(you)) == -1)
				{
					int amount = gAim.multipoint ? 7 : 1;
					for (int a = 0; a < amount; a++)
					{
						if (canHit(you, player, localEyepos, hitboxPos + offset[a]))
						{
							hitPos = hitboxPos + offset[a];
							return true;
						}
					}
				}
				else
				{
					Vector3 prediction = getPredPos(hitboxPos, target);
					if (predVischeck(you, localEyepos, prediction))
					{
						hitPos = prediction;
						return true;
					}
				}
			}
		}
	}
	else if (
		entType == ent_id::CObjectDispenser ||
		entType == ent_id::CObjectSentrygun ||
		entType == ent_id::CObjectTeleporter)
	{
		Vector3 targetPos = target->GetWorldCenter();
		if (canHit(you, target, localEyepos, targetPos))
		{
			hitPos = targetPos;
			return true;
		}
	}

	return false;
}

vector<int> hitscan::bestHitbox(string priority)
{
	vector<int> head = { 0 };
	vector<int> body = { 1, 2, 3, 4, 5 };
	vector<int> feet = { 14, 17 };

	if (priority == "Torso")
		return body;
	else if (priority == "Feet")
		return feet;
	else if (priority == "Auto")
	{
		CBaseEntity* you = GetBaseEntity(me);
		switch (*MakePtr(int*, you, netPlayer.m_iClass))
		{
		case TF2_Scout:		return body;
		case TF2_Soldier:	return feet;
		case TF2_Pyro:		return body;
		case TF2_Demoman:	return body;
		case TF2_Heavy:		return body;
		case TF2_Engineer:	return body;
		case TF2_Spy:		return body;
		}
	}

	return head;
}

bool hitscan::canHit(void* you, void* target, Vector3 vStart, Vector3 vEnd, trace_t *result)
{
	trace_t Trace;
	Ray_t Ray;
	CTraceFilter Filter;

	Filter.pSkip = you;

	Ray.Init(vStart, vEnd);
	gInts.EngineTrace->TraceRay(Ray, MASK_SHOT, &Filter, &Trace);

	if (result != nullptr)
		*result = Trace;

	return (Trace.m_pEnt == target);
}

bool hitscan::predVischeck(void* you, Vector3 vStart, Vector3 vEnd, trace_t *result)
{
	trace_t Trace;
	Ray_t Ray;
	CTraceFilter Filter;

	Filter.pSkip = you;
	Ray.Init(vStart, vEnd);

	gInts.EngineTrace->TraceRay(Ray, MASK_SHOT, &Filter, &Trace);

	if (result != nullptr)
		*result = Trace;

	return (!Trace.DidHit() || Trace.m_pEnt->Type() == ent_id::CTFPlayer);
}

Vector3 hitscan::getPredPos(Vector3 startPos, CBaseEntity* target)
{
	static ConVar* sv_gravity = gInts.Cvar->FindVar("sv_gravity");
	CPlayer* you = (CPlayer*)GetBaseEntity(me);
	Vector3 eyePos = you->EyePos();
	float flDist = distTo(startPos, eyePos);
	float wpnSpeed = getProjSpeed(gEntStuff.gActiveWeapon(GetBaseEntity(me)));
	float flTime = flDist / wpnSpeed;
	Vector3 speed = gEntStuff.EstimateAbsVelocity(target);
	int gravity = sv_gravity->GetInt();

	float finalZ;
	int flags = *MakePtr(int*, target, netPlayer.m_fFlags);
	if (flags & FL_ONGROUND)
		finalZ = speed.z * flTime;
	else
		finalZ = 0.5 * -gravity * pow(flTime, 2) + speed.z * flTime;

	Vector3 finalPos = Vector3(	startPos.x + (speed.x * flTime),
								startPos.y + (speed.y * flTime),
								startPos.z + finalZ);

	for (int i = 0; i < 4; i++)
	{
		flDist = distTo(finalPos, eyePos);
		flTime = flDist / wpnSpeed;
		if (flags & FL_ONGROUND)
			finalZ = speed.z * flTime;
		else
			finalZ = 0.5 * -gravity * pow(flTime, 2) + speed.z * flTime;

		Vector3 newPos( startPos.x + (speed.x * flTime),
						startPos.y + (speed.y * flTime),
						startPos.z + finalZ);

		if (!gHitscan.predVischeck(you, finalPos, newPos))
			return newPos;

		finalPos = newPos;
	}
	return finalPos;
}

int hitscan::getProjSpeed(CBaseEntity* weapon)
{
	if (weapon == NULL)
		return -1;
	string name = weapon->GetClientClass()->chName;
	if (name == "CTFRocketLauncher_DirectHit")
		return 1980;
	else if (	name == "CTFRocketLauncher_AirStrike" ||
				name == "CTFParticleCannon" ||
				name == "CTFRocketLauncher")
		return 1100;
	else if (name == "CTFRaygun")
		return 840;
	return -1; // Return this so we know if the weapon isn't supported
}

float hitscan::distTo(Vector3 start, Vector3 end)
{
	Vector3 vDist = start - end;
	return sqrt(pow(abs(vDist.x), 2) + pow(abs(vDist.y), 2) + pow(abs(vDist.z), 2));
}