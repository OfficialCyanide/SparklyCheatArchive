#include "SDK.h"
#include "Client.h"
#include "CDrawManager.h"
#include "Entity.h"
#include <string>

#include "HvH.h"
#include "Misc.h"
#include "Aim.h"
#include "Trigger.h"
#include "Panels.h"

//============================================================================================
bool __fastcall Hooked_CreateMove(PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd* pCommand)
{
	VMTManager& hook = VMTManager::GetHook(ClientMode); //Get a pointer to the instance of your VMTManager with the function GetHook.
	bool bReturn = hook.GetMethod<bool(__thiscall*)(PVOID, float, CUserCmd*)>(gOffsets.iCreateMoveOffset)(ClientMode, input_sample_frametime, pCommand); //Call the original.
	try
	{
		Handler.SetAction("Hooked_CreateMove()");

		if (!pCommand->command_number)
			return bReturn;

		CBaseEntity* pBaseEntity = GetBaseEntity(me); //Grab the local player's entity pointer.
		CPlayer* pLocal;
		if (!pBaseEntity->ToPlayer(pLocal)) // Uh oh
			return bReturn;

		Handler.SetAction("Trigger.Command()");
		gTrigger.command(pCommand, pLocal);

		Handler.SetAction("Misc.Command()");
		gMisc.command(pCommand, pLocal);

		Handler.SetAction("Aimbot.Command()");
		gAim.command(pCommand, pLocal);

		Handler.SetAction("HvH.Command()");
		gAutist.command(pCommand, pLocal);

		bool attacking = (pCommand->buttons & IN_ATTACK) || ((pCommand->buttons & IN_ATTACK2) && pLocal->Class() == TF2_Pyro);
		bool silent = (gAim.enabled && gAim.silent) || (gTrigger.airblast && gTrigger.aimproj);
		if (((!silent && !gAutist.antiaim) && attacking) || (gAim.enabled && !gAim.silent))
			gInts.Engine->SetViewAngles(pCommand->viewangles);

		gAutist.lastAngles = pCommand->viewangles;
	}
	catch(...)
	{
		MessageBox(NULL, Handler.GetError().c_str(), "ERROR", MB_OK);
	}

	return false;
}
//============================================================================================

void __fastcall Hooked_CHLCreateMove(void* ecx, void* edx, int sequence_number, float input_sample_time, bool active)
{
	VMTManager& hook = VMTManager::GetHook(gInts.Client);
	hook.GetMethod<void(__thiscall*)(void*, int, float, bool)>(gOffsets.iCreateMoveOffset)(edx, sequence_number, input_sample_time, active);
	try
	{
		CUserCmd* pCommand = gInput->GetUserCmd(sequence_number);
		gAutist.speedHack(pCommand);

		if (GetAsyncKeyState('F'))
		{
			void* ch = gInts.Engine->GetNetChannelInfo();
			int* m_nOutSequenceNr = (int*)((unsigned)ch + 0x8);
			*m_nOutSequenceNr += 500;
		}
	}
	catch (...)
	{
		MessageBox(NULL, "Failed Hooked_CHLCreateMove()", "ERROR", MB_OK);
	}
}