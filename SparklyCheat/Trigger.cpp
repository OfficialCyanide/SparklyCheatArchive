#include "Trigger.h"
#include "Esp.h"
#include "Hitscan.h"
#include "Entity stuff.h"
#include "Aim.h"
#include "Ray tracing.h"
#include "inetchannel.h"

Trigger gTrigger;

void Trigger::command(CUserCmd* pCommand, CPlayer* pLocal)
{
	if (pLocal->Lifestate() != LIFE_ALIVE)
		return;

	if (airblast && pLocal->Class() == TF2_Pyro)
	{
		for (int index = 0; index < gInts.EntList->GetHighestEntityIndex(); index++)
		{
			CBaseEntity* current = GetBaseEntity(index);
			if (current == NULL || current->IsDormant())
				continue;

			int id = current->GetClientClass()->iClassID;
			if (id == (int)ent_id::CTFProjectile_Rocket ||
				id == (int)ent_id::CTFGrenadePipebombProjectile ||
				id == (int)ent_id::CTFCleaver ||
				id == (int)ent_id::CTFProjectile_Jar ||
				id == (int)ent_id::CTFProjectile_Flare ||
				id == (int)ent_id::CTFProjectile_Arrow ||
				id == (int)ent_id::CTFProjectile_SentryRocket ||
				id == (int)ent_id::CTFProjectile_JarMilk ||
				id == (int)ent_id::CTFProjectile_HealingBolt ||
				id == (int)ent_id::CTFProjectile_EnergyBall)
			{
				if (current->Team() == pLocal->Team())
					continue;

				INetChannelInfo* ch = (INetChannelInfo*)gInts.Engine->GetNetChannelInfo();
				float latency = ch->GetLatency(FLOW_OUTGOING);
				Vector3 vel = gEntStuff.EstimateAbsVelocity(current);
				
				Vector3 pos = current->GetWorldCenter() + vel * latency;
				Vector3 myPos = pLocal->EyePos();
				
				Vector3 aimVector = pos - myPos, angles;
				VectorAngles(aimVector, angles);
				ClampAngle(angles);

				Vector3 clientAngles; gInts.Engine->GetViewAngles(clientAngles);
				Vector3 aDiff = angles - clientAngles;

				if (aDiff.y > 180)
					aDiff.y -= 360;
				else if (aDiff.y < -180)
					aDiff.y += 360;
				
				if (!aimproj && (abs(aDiff.x) > 90 || abs(aDiff.y) > 90))
					continue; // Make sure the projectile isn't out of FOV

				if (!gHitscan.predVischeck(pLocal, myPos, pos))
					continue;

				float range = 250;
				float dist = gHitscan.distTo(pos, myPos);
				if (dist <= range)
				{
					pCommand->buttons |= IN_ATTACK2;
					if (aimproj)
						pCommand->viewangles = angles;
				}
			}
		}
	}

	if (backstab)
	{
		CBaseEntity* weapon = gEntStuff.gActiveWeapon(pLocal);
		if (weapon->GetClientClass()->iClassID == (int)ent_id::CTFKnife)
		{ 
			if (*MakePtr(bool*, weapon, netPlayer.m_bReadyToBackstab))
			pCommand->buttons |= IN_ATTACK;
		}
	}

	if (!autoshoot)
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

	Ray_t ray;
	trace_t trace;
	Vector3 eyePos = pLocal->EyePos();

	Vector3 viewAngles;
	gInts.Engine->GetViewAngles(viewAngles);

	Vector3 forward;
	AngleVectors(viewAngles, &forward);
	forward = forward * 8192.0f + eyePos;

	ray.Init(eyePos, forward);

	CTraceFilter filter;
	filter.pSkip = pLocal;

	gInts.EngineTrace->TraceRay(ray, /*MASK_AIMBOT | CONTENTS_HITBOX*/ 0x46004003, &filter, &trace);

	if (!trace.m_pEnt ||
		trace.m_pEnt->IsDormant() ||
		(pLocal->Team() == trace.m_pEnt->Team()) ||
		!shouldTarget(trace.m_pEnt))
		return;

	CPlayer* player;
	if (trace.m_pEnt->ToPlayer(player))
	{
		if (trace.hitgroup < 1)
			return;

		if (priority == "Head")
		{
			if (trace.hitbox != 0)
				return;
		}
		else if (priority == "Torso")
		{
			bool notTorso = true;
			for (int i = 0; i <= 5; i++)
				if (trace.hitbox == i)
					notTorso = false;

			if (notTorso)
				return;
		}
		else if (priority == "Feet")
		{
			if (trace.hitbox != 17 && trace.hitbox != 14)
				return;
		}

		// Make sure we should target the player
		if (player->Lifestate() != LIFE_ALIVE)
			return;
	}

	pCommand->buttons |= IN_ATTACK;
}

bool Trigger::shouldTarget(CBaseEntity* entity)
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