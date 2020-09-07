#include "Misc.h"
#include "CDrawManagerEx.h"
#include "Entity stuff.h"
#include "commonColors.h"
#include "inetchannel.h"

#include "Aim.h"
#include "Menu.h"
#include "Hitscan.h"
#include "Esp.h"
#include "Config.h"
#include "Ray tracing.h"

#include <vector>
#include <time.h>

Misc gMisc;

void Misc::command(CUserCmd* pCommand, CPlayer* pLocal)
{
	if (pLocal->Lifestate() != LIFE_ALIVE)
		return;

	if (bhop)
	{
		static bool bJumpReleased;
		if (pCommand->buttons & IN_JUMP)
		{
			if (!bJumpReleased)
			{
				if (!(pLocal->Flags() & FL_ONGROUND))
					pCommand->buttons &= ~IN_JUMP;
			}
			else
				bJumpReleased = false;
		}
		else if (!bJumpReleased)
			bJumpReleased = true;
	}

	*MakePtr(bool*, pLocal, netPlayer.m_bDrawViewmodel) = (bool)viewmodel;

	bool removecond = false;

	removecond = ((condkey == "Shift" && gMenu.keyDown(VK_LSHIFT)) ||
		(condkey == "Mouse1" && gMenu.keyDown(1)) ||
		(condkey == "Mouse2" && gMenu.keyDown(2)) ||
		(condkey == "Mouse3" && gMenu.keyDown(4)) ||
		(condkey == "F" && gMenu.keyDown('F')) ||
		(condkey == "Alt" && gMenu.keyDown(18)) || // Alt key
		(condkey == "R" && gMenu.keyDown('R')));


	static int lastweapon = 0;
	if ((lastweapon != pCommand->weaponselect && wepSwitch) || removecond)
	{
		INetChannel* ch = (INetChannel*)gInts.Engine->GetNetChannelInfo();
		int* m_nOutSequenceNr = (int*)((unsigned)ch + 0x8);
		*m_nOutSequenceNr += removecond ? condAmount : 500;
	}
	
	/*static float lastTime = gGlobals->curtime;

	int maxSpams = chatSpams.size();
	static size_t spamIndex = 0;

	if (chatspam && lastTime + 0.5f < gGlobals->curtime)
	{
		if (spamIndex >= maxSpams)
			spamIndex = 0;
		else spamIndex++;

		char cmd[256];
		strcpy(cmd, chatSpams[spamIndex]);
		gInts.Engine->ClientCmd_Unrestricted(cmd);

		lastTime = gGlobals->curtime;
	}*/
}

void Misc::draw(CBaseEntity* current, CPlayer* pLocal, int entIndex)
{
	if (!gInts.Engine->IsInGame())
		return;

	CPlayer* player;
	if (!current->ToPlayer(player))
		return;

	Vector3 worldPos = player->GetAbsOrigin(); // Get the entity's world positon
	int id = player->GetClientClass()->iClassID; // Get the entity's type

	if (player->Lifestate() != LIFE_ALIVE) // If the player isn't alive, then skip
		return;
	CBaseEntity* you = GetBaseEntity(me);
	int flags = player->Flags();
	bool shouldDraw = player->Team() != you->Team() || gEsp.drawTeam;

	if ((paths && shouldDraw) || (local && entIndex == me))
	{
		Vector3 velocity = gEntStuff.EstimateAbsVelocity(player) / 10;
		Vector3 predPos[101];
		int max = (flags & FL_ONGROUND) ? 21 : 101;
		for (int i = 0; i < max; i++)
			predPos[i] = predPosAt(i * 0.05, player);

		DWORD lineColor;
		for (int i = 0; i < max - 1; i++)
		{
			trace_t result; // Check to see if we're about to hit a surface
			if (!gHitscan.predVischeck(you, predPos[i], predPos[i + 1], &result)) 
			{
				// Draw a circle to show our landing position/angle
				if (!(flags & FL_ONGROUND))
					drawWorldCircle(result.endpos, result.plane.normal, 50, 20, COLORCODE(100, 140, 255, 255));
				break;
			}

			if (i < 21) // Make a gradiant from green to red
				lineColor = gDrawEx.FromHSB((100.f - (i * 5))/255, 200.f/255, 1, 255);

			Vector3 screenPos[3];
			if (!gDraw.WorldToScreen(predPos[i], screenPos[0]))
				continue;
			if (!gDraw.WorldToScreen(predPos[i + 1], screenPos[1]))
				continue;

			gDrawEx.DrawLine(
				screenPos[0].x, screenPos[0].y,
				screenPos[1].x, screenPos[1].y,
				lineColor);

			Vector2 extraLine = Vector2(predPos[i + 1].x, predPos[i + 1].y);
			extraLine += vectorAngle(Vector2(velocity.x, velocity.y).toAngle(), 10);
			extraLine = rotate_vec2d(Vector2(predPos[i + 1].x, predPos[i + 1].y), DEG2RAD1(90), extraLine);

			if (!gDraw.WorldToScreen(Vector3(extraLine.x, extraLine.y, predPos[i + 1].z), screenPos[2]))
				continue;

			gDrawEx.DrawLine(
				screenPos[1].x, screenPos[1].y,
				screenPos[2].x, screenPos[2].y,
				lineColor);
		}
	}

	if (trace && shouldDraw)
	{
		static const int traceLength = 257;
		static Vector3 traces[32][traceLength];

		for (int i = 0; i < traceLength - 1; i++)
			traces[entIndex][i] = traces[entIndex][i + 1];
		traces[entIndex][traceLength - 1] = worldPos;

		Vector3 screenPos[traceLength];
		gDraw.WorldToScreen(traces[entIndex][0], screenPos[0]);
		for (int i = 0; i < traceLength - 1; i++)
		{
			if (!gDraw.WorldToScreen(traces[entIndex][i + 1], screenPos[i + 1]))
				continue;
			gDrawEx.DrawLine(
				screenPos[i].x, screenPos[i].y, screenPos[i + 1].x, screenPos[i + 1].y,
				COLORCODE(50, 100, 255, 255));
		}
	}

	if (hitpos && entIndex != me && shouldDraw)
	{
		for (int a = 0; a < 18; a++)
		{
			Vector3 screenPos;

			int amount = gAim.multipoint ? 7 : 1;
			for (int i = 0; i < amount; i++)
			{
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

				if (gDraw.WorldToScreen(player->HitboxPos(a) + offset[i], screenPos))
					gDraw.DrawString(screenPos.x, screenPos.y, colors.white, to_string(a).c_str());
			}
		}
	}
}

void Misc::drawDebug()
{
	if (greenscreen)
	{
		static Vector3 wpos;
		if (setscreen)
		{
			CBaseEntity* cLocal = GetBaseEntity(me);
			CPlayer* you;
			if (!cLocal->ToPlayer(you))
				return;

			Vector3 eyePos = you->EyePos(), viewAngles;
			gInts.Engine->GetViewAngles(viewAngles);
			Vector3 forward;
			AngleVectors(viewAngles, &forward);
			forward = forward * 8192.0f + eyePos;

			Ray_t ray;
			trace_t trace;
			ray.Init(eyePos, forward);

			CTraceFilter filter;
			filter.pSkip = you;

			gInts.EngineTrace->TraceRay(ray, MASK_ALL, &filter, &trace);
			wpos = trace.endpos;
			wpos.z += hscreen / 2;
		}

		Vector3 corners[4] =
		{
			Vector3(wpos.x - (wscreen / 2), wpos.y, wpos.z + (hscreen / 2)),
			Vector3(wpos.x + (wscreen / 2), wpos.y, wpos.z + (hscreen / 2)),
			Vector3(wpos.x + (wscreen / 2), wpos.y, wpos.z - (hscreen / 2)),
			Vector3(wpos.x - (wscreen / 2), wpos.y, wpos.z - (hscreen / 2))
		};

		for (int i = 0; i < 4; i++)
		{
			if (rotscreen)
			{
				Vector2 cornerxy(corners[i].x, corners[i].y);
				cornerxy = rotate_vec2d({ wpos.x, wpos.y }, DEG2RAD1(rotscreen), cornerxy);
				corners[i].x = cornerxy.x, corners[i].y = cornerxy.y;
			}
			corners[i].x += xscreen, corners[i].y += yscreen, corners[i].z += zscreen;
		}

		Vertex_t verts[4];
		for (int i = 0; i < 4; i++)
		{
			Vector3 spos;
			if (!gDraw.WorldToScreen(corners[i], spos))
				return;
			verts[i] = Vertex_t(Vector2(spos.x, spos.y));
		}

		const byte screenColor[4] = { RED(colorscreen), GREEN(colorscreen), BLUE(colorscreen), ALPHA(colorscreen) };
		gInts.Surface->DrawSetTextureRGBA(Textures.greenScreen, screenColor, 1, 1);
		gDrawEx.DrawPolygon(4, verts, Textures.greenScreen);
	}
}

void Misc::init()
{
	// Saving the flags so we can set them back to normal when bypass is off
	cheatFlags = gInts.Cvar->FindCommandBase("sv_cheats")->m_nFlags;
}

Vector3 Misc::predPosAt(float flTime, CBaseEntity* target)
{
	if (flTime == 0)
		return target->GetAbsOrigin();

	int gravity = gInts.Cvar->FindVar("sv_gravity")->GetInt();
	Vector3 velocity = gEntStuff.EstimateAbsVelocity(target);
	Vector3 startPos = target->GetAbsOrigin();
	float zdrop;
	if (*MakePtr(int*, target, netPlayer.m_fFlags) & FL_ONGROUND)
		zdrop = velocity.z * flTime;
	else
		zdrop = 0.5 * -gravity * pow(flTime, 2) + velocity.z * flTime;

	return Vector3(	startPos.x + (velocity.x * flTime),
					startPos.y + (velocity.y * flTime),
					startPos.z + zdrop);
}

inline DWORD WINAPI killCvars(LPVOID param)
{
	ConCommandBase *base = gInts.Cvar->GetCommands();
	int count = 0;
	while (base)
	{

		if (base->m_nFlags & (int)ConvarFlags::FCVAR_CHEAT)
			base->m_nFlags &= ~(int)ConvarFlags::FCVAR_CHEAT;

		if (base->m_nFlags & (int)ConvarFlags::FCVAR_REPLICATED)
			base->m_nFlags &= ~(int)ConvarFlags::FCVAR_REPLICATED;

		if (base->m_nFlags & (int)ConvarFlags::FCVAR_PROTECTED)
			base->m_nFlags &= ~(int)ConvarFlags::FCVAR_PROTECTED;

		if (base->m_nFlags & (int)ConvarFlags::FCVAR_SPONLY)
			base->m_nFlags &= ~(int)ConvarFlags::FCVAR_SPONLY;

		if (base->m_nFlags & (int)ConvarFlags::FCVAR_HIDDEN)
			base->m_nFlags &= ~(int)ConvarFlags::FCVAR_HIDDEN;

		if (base->m_nFlags & (int)ConvarFlags::FCVAR_DEVELOPMENTONLY)
			base->m_nFlags &= ~(int)ConvarFlags::FCVAR_DEVELOPMENTONLY;

		if (base->m_nFlags & (int)ConvarFlags::FCVAR_NOT_CONNECTED)
			base->m_nFlags &= ~(int)ConvarFlags::FCVAR_NOT_CONNECTED;

		if (ConVar *c = dynamic_cast<ConVar *>(base))
		{
			c->m_bHasMax = false;
			c->m_bHasMin = false;
		}

		base = base->m_pNext;
		count++;
	}

	gInts.Cvar->ConsolePrintf("[Sparkly] %d cvars have been unlocked.\n[Sparkly] Beware of potential SMAC bans!\n", count);

	return 0;
}

void Misc::killCvar(ConCommandBase* target)
{
	if (target->m_nFlags & (int)ConvarFlags::FCVAR_CHEAT)
		target->m_nFlags &= ~(int)ConvarFlags::FCVAR_CHEAT;

	if (target->m_nFlags & (int)ConvarFlags::FCVAR_REPLICATED)
		target->m_nFlags &= ~(int)ConvarFlags::FCVAR_REPLICATED;

	if (target->m_nFlags & (int)ConvarFlags::FCVAR_PROTECTED)
		target->m_nFlags &= ~(int)ConvarFlags::FCVAR_PROTECTED;

	if (target->m_nFlags & (int)ConvarFlags::FCVAR_SPONLY)
		target->m_nFlags &= ~(int)ConvarFlags::FCVAR_SPONLY;

	if (target->m_nFlags & (int)ConvarFlags::FCVAR_HIDDEN)
		target->m_nFlags &= ~(int)ConvarFlags::FCVAR_HIDDEN;

	if (target->m_nFlags & (int)ConvarFlags::FCVAR_DEVELOPMENTONLY)
		target->m_nFlags &= ~(int)ConvarFlags::FCVAR_DEVELOPMENTONLY;

	if (target->m_nFlags & (int)ConvarFlags::FCVAR_NOT_CONNECTED)
		target->m_nFlags &= ~(int)ConvarFlags::FCVAR_NOT_CONNECTED;

	if (ConVar *c = dynamic_cast<ConVar *>(target))
	{
		c->m_bHasMax = false;
		c->m_bHasMin = false;
	}
}

void Misc::main()
{
	if (gConfig.save_config)
	{
		gConfig.save_config = false;
		gConfig.saveCfg();
	}
	if (gConfig.reload_config)
	{
		gConfig.reload_config = false;
		gConfig.loadCfg();
	}

	if (gConfig.reload_chatspam)
	{
		gConfig.reload_chatspam = false;
		//gConfig.loadChatspam();
	}

	static ConVar* cCheats = gInts.Cvar->FindVar("sv_cheats");
	if (cheatBp)
	{
		killCvar(cCheats);
		cCheats->SetValue(1);
	}
	else
		cCheats->m_nFlags = cheatFlags;

	if (killAllCvars && !allCvarsKilled)
	{
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)killCvars, nullptr, 0, nullptr);
		allCvarsKilled = true;
	}
	else if (!killAllCvars && allCvarsKilled)
	{
		killAllCvars = true;
		gInts.Cvar->ConsolePrintf("[Sparkly] Cvars cannot be un-killed/re-locked!\n");
	}
}

void Misc::drawWorldCircle(Vector3 &pos, Vector3 &normal, float radius, int segments, DWORD dwColor)
{
	vector<Vector3> lines; // A vector of vectors
	float degrees = 360 / segments;
	for (float fl = 0; fl < 360; fl += segments)
	{
		Vector2 xy = Vector2(pos.x, pos.y); // Focus on rotating x and y
		Vector2 line = xy; // Set our point
		line.x += radius; // Extend from origin by (radius) units 
		line = rotate_vec2d(xy, DEG2RAD1(fl), line); // Rotate the point by fl, eventually completing a full circle

		lines.push_back(Vector3(line.x, line.y, pos.z)); // Add the line to our array
	}

	for (int i = 0; i < lines.size(); i++)
	{
		// Make sure we wrap all the way around the circle using our last line
		Vector3 nextVec = (i < lines.size() - 1)? lines[i + 1] : lines[0];
		Vector3 currentVec = lines[i];

		//string text = to_string(normal.x) + ", " + to_string(normal.y) + ", " + to_string(normal.z);
		//gDraw.DrawString(5, 5, colors.white, text.c_str());

		Vector3 cPoint, nPoint; // If the lines aren't on-screen, don't draw them
		if (!gDraw.WorldToScreen(currentVec, cPoint) || !gDraw.WorldToScreen(nextVec, nPoint))
			continue;
		gDrawEx.DrawLine(cPoint.x, cPoint.y, nPoint.x, nPoint.y, dwColor);
	}
}