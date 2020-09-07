#include "SDK.h"
#include "Client.h"
#include "Panels.h"
#include "Config.h"
#include "Draw models.h"
#include "FrameStageNotify.h"
#include "Menu.h"
#include <string>

CHandler Handler;

CGlobals* gGlobals;
IPlayerInfoManager* gPlayerInfoManager;
CInput* gInput;

COffsets gOffsets;
CInterfaces gInts;

CreateInterface_t EngineFactory = NULL;
CreateInterface_t ClientFactory = NULL;
CreateInterface_t VGUIFactory = NULL;
CreateInterface_t VGUI2Factory = NULL;

DWORD WINAPI dwMainThread( LPVOID lpArguments )
{
	try
	{
		Handler.SetAction("Cheat.Init()");
		if (gInts.Client == NULL) //Prevent repeat callings.
		{
			//Gottammove those factorys up.
			//Grab the factorys from their resptive module's EAT.
			ClientFactory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("client.dll"), "CreateInterface");
			gInts.Client = (CHLClient*)ClientFactory("VClient017", NULL);

			gInts.EntList = (CEntList*)ClientFactory("VClientEntityList003", NULL);

			EngineFactory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("engine.dll"), "CreateInterface");
			gInts.Engine = (EngineClient*)EngineFactory("VEngineClient013", NULL);

			//const char* version = gInts.Engine->GetProductVersionString();
			//DWORD iVersion = stoi(version);

			//bool old = iVersion != 4280740;

			/*if (old)
			{
				MessageBox(NULL,
					"This is an old version of Sparkly Cheat!\nJoin https://www.Discord.io/SparklyCheat and download the newest update",
					string("New update: " + (string)version).c_str(), MB_OK);
				exit(0);
			}*/

			VGUIFactory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("vguimatsurface.dll"), "CreateInterface");
			gInts.Surface = (ISurface*)VGUIFactory("VGUI_Surface030", NULL);

			gInts.ModelInfo = (IVModelInfo*)EngineFactory("VModelInfoClient006", NULL);
			gInts.EngineTrace = (IEngineTrace*)EngineFactory("EngineTraceClient003", NULL);

			CreateInterfaceFn CVarFactory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "CreateInterface"));
			gInts.Cvar = static_cast<ICvar*>(CVarFactory("VEngineCvar004", nullptr));

			CreateInterfaceFn ServerFactory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA("server.dll"), "CreateInterface"));
			gPlayerInfoManager = (IPlayerInfoManager*)ServerFactory("PlayerInfoManager002", nullptr);

			gInts.ModelRender = (CModelRender*)EngineFactory("VEngineModel016", nullptr);

			gInts.RenderView = (CRenderView*)EngineFactory("VEngineRenderView014", nullptr);

			CreateInterfaceFn MaterialSystemFactory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(gSignatures.GetModuleHandleSafe("MaterialSystem.dll"), "CreateInterface"));
			gInts.MatSystem = (CMaterialSystem*)MaterialSystemFactory("VMaterialSystem081", nullptr);

			//Setup the Panel hook so we can draw.
			if (!gInts.Panels)
			{
				VGUI2Factory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("vgui2.dll"), "CreateInterface");
				gInts.Panels = (IPanel*)VGUI2Factory("VGUI_Panel009", NULL);

				if (gInts.Panels)
				{
					VMTBaseManager* panelHook = new VMTBaseManager(); //Setup our VMTBaseManager for Panels.
					panelHook->Init(gInts.Panels);
					panelHook->HookMethod(&Hooked_PaintTraverse, gOffsets.iPaintTraverseOffset);
					panelHook->Rehook();
				}
			}

			DWORD dwClientModeAddress = gSignatures.GetClientSignature("8B 0D ? ? ? ? 8B 02 D9 05");
			gInts.ClientMode = **(ClientModeShared***)(dwClientModeAddress + 2);

			DWORD clientStateOffset = gSignatures.GetEngineSignature("6A 01 B9 ? ? ? ? E8 ? ? ? ? 50 B9") + 3;
			gInts.ClientState = *reinterpret_cast<CClientState **>(clientStateOffset);

			VMTBaseManager* clientHook = new VMTBaseManager(); //Setup our VMTBaseManager for ClientMode.
			clientHook->Init(gInts.ClientMode);
			clientHook->HookMethod(&Hooked_CreateMove, gOffsets.iCreateMoveOffset); //ClientMode create move is called inside of CHLClient::CreateMove, and thus no need for hooking WriteUserCmdDelta.
			clientHook->Rehook();

			VMTBaseManager* chlHook = new VMTBaseManager();
			chlHook->Init(gInts.Client);
			chlHook->HookMethod(&Hooked_CHLCreateMove, gOffsets.iCreateMoveOffset);
			chlHook->HookMethod(&Hooked_FrameStageNotify, gOffsets.iFrameStageOffset);
			chlHook->Rehook();

			/*VMTBaseManager* modelHook = new VMTBaseManager();
			modelHook->Init(gInts.ModelRender);
			modelHook->HookMethod(&Hooked_DrawModelExecute, gOffsets.iDrawModelExecuteOffset);
			modelHook->Rehook();*/

			DWORD dwInputPointer = (gSignatures.dwFindPattern((DWORD)chlHook->GetMethod<DWORD>(gOffsets.iCreateMoveOffset), ((DWORD)chlHook->GetMethod<DWORD>(gOffsets.iCreateMoveOffset)) + 0x100, "8B 0D")) + (0x2);
			gInput = **(CInput***)dwInputPointer;
		}
	}
	catch (...)
	{
		MessageBox(NULL, Handler.GetError().c_str(), "ERROR", MB_OK);
	}
		return 0; //The thread has been completed, and we do not need to call anything once we're done. The call to Hooked_PaintTraverse is now our main thread.
}

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	//If you manually map, make sure you setup this function properly.
	if(dwReason == DLL_PROCESS_ATTACH)
	{
		gConfig.hInstance = hInstance;
		CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)dwMainThread, NULL, 0, NULL ); //CreateThread > _BeginThread. (For what we're doing, of course.)
	}
	return true;
}