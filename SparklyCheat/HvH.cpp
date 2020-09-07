#include "HvH.h"
#include "Aim.h"
#include "Hitscan.h"
#include "Menu.h"
#include "Client.h"
#include "Ray tracing.h"

Autism gAutist;

inline float CalcEdge(CPlayer* pLocal)
{
	trace_t result;
	Vector3 eyePos = pLocal->EyePos();
	float leastDist = -1, edgeYaw = -1;
	for (int i = 0; i < 360; i += 2)
	{
		Vector3 angle(0, i, 0), direction;
		AngleVectors(angle, &direction);

		if (gHitscan.predVischeck(pLocal, eyePos, direction * 90 + eyePos, &result))
			continue;

		Vector3 normal = result.plane.normal;
		if (abs(normal.y - direction.y) <= 2 && normal.x < 60 && normal.x > -60)
		{
			if (leastDist == -1 || Vector3(result.endpos - eyePos).Length() < leastDist)
			{
				edgeYaw = i + 90, leastDist = Vector3(result.endpos - eyePos).Length();

				Vector3 centerEndpos = result.endpos, rightNrm;
				AngleVectors(Vector3(0, i - 90, 0), &rightNrm);

				Vector3 start = rightNrm * 60 + centerEndpos;
				gHitscan.predVischeck(pLocal, start, start + direction * 80, &result);

				if (result.DidHitWorld())
					edgeYaw = i - 90;
			}
		}
	}

	if (leastDist != -1)
		return edgeYaw;
	else
		return -1;
}

void Autism::command(CUserCmd *pCommand, CPlayer* pLocal)
{
	*(bool*)(pLocal + netPlayer.m_nForceTauntCam) = thirdperson;

		static ConVar* avoidPush = gInts.Cvar->FindVar("tf_avoidteammates_pushaway");
		if (avoidPush)
			avoidPush->SetValue(!nopush);

	if (!antiaim ||
		(pCommand->buttons & IN_ATTACK) ||
		((pCommand->buttons & IN_ATTACK2) && pLocal->Class() == TF2_Pyro) ||
		(!gAim.silent && gAim.enabled))
		return;

	Vector3 newAngles = pCommand->viewangles;
	static bool change = false;
	if (pitch == "Fake up")
		newAngles.x = -345;
	else if (pitch == "Fake down")
		newAngles.x = 271;
	else if (pitch == "Up")
		newAngles.x = -89;
	else if (pitch == "Down")
		newAngles.x = 89;
	else if (pitch == "Lisp")
	{
		if (change)
			newAngles.x = 271;
		else
			newAngles.x = -89;
	}
	else if (pitch == "Meme spin")
	{
		static float spin = -132;
		spin += 2;
		if (spin > 0)
			spin = -134;

		newAngles.x = -315 + spin;
	}
	else if (pitch == "Meme (Scoped)")
		newAngles.x = -372;
	else if (pitch == "Meme (Unscoped)")
		newAngles.x = -397;
	else if (pitch == "Bait (Mouse3)")
	{
		//newAngles.x = 3256; Fake center

		int rnd = rand() % 3 + 1;
		switch (rnd)
		{
		case 1: newAngles.x = 271; break;
		case 2: newAngles.x = -89; break;
		case 3: newAngles.x = -87; break;
		}

		if (GetAsyncKeyState(4))
		{
			int rnd2 = rand() % 3 + 1;
			switch (rnd2)
			{
			case 1: newAngles.x = -271; break;
			case 2: newAngles.x = 89; break;
			case 3: newAngles.x = 87; break;
			}
		}
	}

	if (yaw == "Spin")
	{
		static int spin = 0;
		spin += 3;
		if (spin > 180)
			spin = -180;

		newAngles.y += spin;
	}
	else if (yaw == "Edge")
	{
		float edgeYaw = CalcEdge(pLocal);
		if (edgeYaw != -1)
			newAngles.y = edgeYaw;
	}
	else if (yaw == "Fake side")
	{
		if (change)
			newAngles.y += 180;
	}
	else if (yaw == "Jitter (0/90)")
	{
		if (change)
			newAngles.y += 90;
	}
	else if (yaw == "Jitter (0/-90)")
	{
		if (change)
			newAngles.y -= 90;
	}
	else if (yaw == "Jitter spin")
	{
		if (change)
			newAngles.y += 180;

		static int spin = 0;
		spin += 3;
		if (spin > 180)
			spin = -180;

		newAngles.y += spin;
	}
	
	newAngles.y += yAngle;
	newAngles.x += pAngle;

	Vector3 clamped = newAngles;
	ClampAngle(clamped);
	clamped.x = newAngles.x;

	gAim.silentMovementFix(pCommand, clamped);
	pCommand->viewangles = clamped;

	change = !change;
}

void Autism::speedHack(CUserCmd* pCommand)
{
	bool keyDown =
		(key == "None" ||
		(key == "Shift" && gMenu.keyDown(VK_LSHIFT)) ||
		(key == "Mouse1" && gMenu.keyDown(1)) ||
		(key == "Mouse2" && gMenu.keyDown(2)) ||
		(key == "Mouse3" && gMenu.keyDown(4)) ||
		(key == "F" && gMenu.keyDown('F')) ||
		(key == "Alt" && gMenu.keyDown(18)) // Alt key
	);

	static int iSpeedCounter = 0;
	if (speedAmount) // Credits to gir489
		if (iSpeedCounter > 0 && keyDown)
		{
			iSpeedCounter--;
			pCommand->tick_count--;
			_asm
			{
				push eax;
				mov eax, dword ptr ss : [ebp];
				mov eax, [eax];
				lea eax, [eax + 0x4];
				sub[eax], 0x5;
				pop eax;
			}
		}
		else
			iSpeedCounter = speedAmount;
}