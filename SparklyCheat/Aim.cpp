#include "Aim.h"
#include "Esp.h"
#include "Hitscan.h"
#include <time.h>

#include "Entity stuff.h"
#include "Trigger.h"
#include "Players.h"
#include "Entity.h"

aim gAim;

void aim::command(CUserCmd* pCommand, CPlayer* pLocal)
{
	target = -1;

	if (!enabled)
		return;
	if (pLocal->Lifestate() != LIFE_ALIVE)
		return;

	tf_classes localClass = pLocal->Class();
	if (scopedOnly && localClass == TF2_Sniper && !(pLocal->Conditions() & TFCond_Zoomed))
		if (pLocal->ActiveWeapon() == gInts.EntList->GetClientEntityFromHandle(pLocal->MyWeapons()[0]))
			return;

	bool keyDown =
		(key == "None" ||
		(key == "Shift" && gMenu.keyDown(VK_LSHIFT)) ||
		(key == "Mouse1" && gMenu.keyDown(1)) ||
		(key == "Mouse2" && gMenu.keyDown(2)) ||
		(key == "Mouse3" && gMenu.keyDown(4)) ||
		(key == "F" && gMenu.keyDown('F')) ||
		(key == "Alt" && gMenu.keyDown(18)) // Alt key
	);
	if (!keyDown) return;

	Vector3 clientAngles;
	gInts.Engine->GetViewAngles(clientAngles);

	Vector3 vAim;
	Vector3 worldDist(0, 0, 0);
	Vector3 distToTarget(420, 420, 0);
	bool firstTarget = true, rage = false;

	if (wrangler)
	{
		Vector3 TargetPos = pLocal->GetWorldCenter();
		Vector3 SentryPos(0, 0, 0);
		for (int i = 1; i <= gInts.EntList->GetHighestEntityIndex(); i++)
		{
			if (i == me) // If it's us, then skip
				continue;

			CBaseEntity* current = GetBaseEntity(i); // Get the entity at i index
			if (current == NULL || current->IsDormant()) // If it's dormant or blank, then skip
				continue;
			if (current->Team() != pLocal->Team())
				continue;

			if (current->Type() != ent_id::CObjectSentrygun)
				continue;

			size_t m_hBuilder = *MakePtr(size_t*, current, netObject.m_hBuilder);
			CBaseEntity* owner = gInts.EntList->GetClientEntityFromHandle(m_hBuilder);
			if (owner && owner->GetIndex() == pLocal->GetIndex())
			{
				SentryPos = current->GetWorldCenter();
				break;
			}
		}

		Vector3 direction = TargetPos - SentryPos, cAim;
		VectorAngles(direction, cAim);
		ClampAngle(cAim);

		if (silent && (pCommand->buttons & IN_ATTACK))
			silentMovementFix(pCommand, cAim);

		if (!silent || (silent && (pCommand->buttons & IN_ATTACK)))
			pCommand->viewangles = cAim;

		return;
	}

	for (int i = 1; i < gInts.EntList->GetHighestEntityIndex(); i++)
	{
		if (i == me) // If it's us, then skip
			continue;

		CBaseEntity* current = GetBaseEntity(i); // Get the entity at i index
		if (current == NULL || current->IsDormant()) // If it's dormant or blank, then skip
			continue;
		if (current->Team() == pLocal->Team())
			continue;
		if (current->Type() == ent_id::CTFPlayer)
			if (((CPlayer*)current)->Lifestate() != LIFE_ALIVE) // If the player isn't alive, then skip
				continue;

		// Check the playerlist for rage / friendlies
		int priority = 1;
		for (int a = 0; a < gPlayers.pAmount; a++)
		{
			if (i != gPlayers.priorities[a].index)
				continue;

			priority = gPlayers.priorities[a].priority;
			break;
		}
		if (!priority) continue;

		if (!shouldTarget(current))
			continue;

		Vector3 targetPos;
		if (!gHitscan.bestHit(current, targetPos))
			continue;
		
		Vector3 vDirection = targetPos - pLocal->EyePos();
		Vector3 cWorldDist = vDirection;

		Vector3 cAim;
		VectorAngles(vDirection, cAim);
		ClampAngle(cAim);
		Vector3 distToCurrent = cAim - clientAngles;


		// The max degrees between any two points on a circle is 180 if you want the shortest path
		if (distToCurrent.y > 180)
			distToCurrent.y -= 360;
		else if (distToCurrent.y < -180)
			distToCurrent.y += 360;

		if (abs(distToCurrent.y) > fov)
			continue;
		if (abs(distToCurrent.x) > fov)
			continue;

		if (gAim.method == "FOV")
		{
			if (priority == 2 && !rage)
			{
				target = i, rage = true;
				distToTarget = distToCurrent;
				vAim = cAim;
				continue;
			}
			else if (rage && priority < 2)
				continue;
			if (abs(distToCurrent.x) + abs(distToCurrent.y) < abs(distToTarget.y) + abs(distToTarget.x))
			{
				target = i;
				distToTarget = distToCurrent;
				vAim = cAim;
			}
		}
		else if (gAim.method == "Distance")
		{
			if (priority == 2 && !rage)
			{
				target = i, rage = true;
				distToTarget = distToCurrent;
				vAim = cAim;
				continue;
			}
			else if (rage && priority < 2)
				continue;

			float cDist = abs(cWorldDist.x) + abs(cWorldDist.y) + abs(cWorldDist.z);
			float lDist = abs(worldDist.x) + abs(worldDist.y) + abs(worldDist.z);

			if (firstTarget || cDist < lDist)
			{
				firstTarget = false;

				target = i;
				worldDist = cWorldDist;
				vAim = cAim;
			}
		}
	}

	if (target == -1)
		return;

	ClampAngle(vAim);
	if (slowAim)
	{
		distToTarget /= slowAim + 1;

		Vector3 newAngle = Vector3(distToTarget.x, distToTarget.y, 0); ClampAngle(newAngle);
		Vector3 finalAngle = pCommand->viewangles + newAngle;
		vAim = finalAngle;

		silentMovementFix(pCommand, vAim);
		pCommand->viewangles = finalAngle;
	}
	else
	{
		if (silent && (pCommand->buttons & IN_ATTACK))
			silentMovementFix(pCommand, vAim);

		if (!silent || (silent && (pCommand->buttons & IN_ATTACK)))
			pCommand->viewangles = vAim;
	}

	int tickbase = *MakePtr(int*, pLocal, netPlayer.m_nTickBase);
	float flChargedTime = (tickbase * gGlobals->interval_per_tick) - pLocal->FovTime();

	if (autoShoot)
	{
		if ((pLocal->Conditions() & tf_cond::TFCond_Zoomed) && flChargedTime < 0.2)
			return;

		silentMovementFix(pCommand, vAim);
		pCommand->viewangles = vAim;
		pCommand->buttons |= IN_ATTACK;
	}
}

bool aim::shouldTarget(CBaseEntity* entity)
{
	ent_id entType = entity->Type();
	if (entType == ent_id::CTFPlayer)
	{
		int flags = ((CPlayer*)entity)->Conditions();

		if (ignoreInvis && (flags & TFCond_Cloaked))
			return false;
		if (ignoreDisguise && (flags & TFCond_Disguised))
			return false;
		if (ignoreUber && (flags & TFCond_Ubercharged))
			return false;
		if (ignoreBonk && (flags & TFCond_Bonked))
			return false;
	}
	else if (entType == ent_id::CObjectSentrygun)
	{
		if (!aimSentry)	return false;
	}
	else if (entType == ent_id::CObjectDispenser)
	{
		if (!aimDispenser) return false;
	}
	else if (entType == ent_id::CObjectTeleporter)
	{
		if (!aimTeleport) return false;
	}
	else
		return false;

	return true;
}

// Credits to F1ssi0n
void aim::silentMovementFix(CUserCmd *pCommand, Vector3 angles)
{
	Vector3 vecSilent(pCommand->forwardmove, pCommand->sidemove, pCommand->upmove);
	float flSpeed = sqrt(vecSilent.x * vecSilent.x + vecSilent.y * vecSilent.y);
	Vector3 angMove;
	VectorAngles(vecSilent, angMove);
	float flYaw = DEG2RAD1(angles.y - pCommand->viewangles.y + angMove.y);
	pCommand->forwardmove = cos(flYaw) * flSpeed;
	pCommand->sidemove = sin(flYaw) * flSpeed;
}