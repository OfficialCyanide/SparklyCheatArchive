#include "SDK.h"
#include "Panels.h"
#include "Entity stuff.h"
#include "CDrawManagerEx.h"
#include "Fonts.h"
#include "Config.h"
#include "Entity.h"

#include "Menu.h"
#include "Esp.h"
#include "Misc.h"
#include "Players.h"
#include "Ray tracing.h"
#include "Hitscan.h"
#include "HvH.h"

#include "Mathack.h"
CScreenSize gScreen;
//===================================================================================
void __fastcall Hooked_PaintTraverse( PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce )
{
	try
	{
		Handler.SetAction("Hooked_PaintTraverse()");

		string panelName = gInts.Panels->GetName(vguiPanel);

		if (panelName[0] == 's' && panelName[6] == 'C' && panelName[12] == 'D')
			gMisc.drawDebug(); // Look for staticClientDLLPanel, the bottom most panel

		//static unsigned HudScopeCharge;
		//if (panelName[0] == 'H' && panelName)

		if ((gMisc.scope == "No scope" || gMisc.scope == "Both") && 
			panelName[8] != 'C' && panelName[0] == 'H' && panelName[3] == 'S' && panelName[7] == 'e')
			return; // Look for HudScope

		VMTManager& hook = VMTManager::GetHook(pPanels); // Get a pointer to the instance of your VMTManager with the function GetHook.
		hook.GetMethod<void(__thiscall*)(PVOID, unsigned int, bool, bool)>(gOffsets.iPaintTraverseOffset)(pPanels, vguiPanel, forceRepaint, allowForce); //Call the original.

		static unsigned int panel;
		if (panel == NULL)
		{
			//if (panelName.find("FocusOverlayPanel") != string::npos)
			if (panelName[0] == 'F' && panelName[5] == 'O' && panelName[12] == 'P')
			{
				panel = vguiPanel;
				Intro();
			}
		}

		if (panel == vguiPanel) //If we're on FocusOverlayPanel, call our drawing code.
		{
			gInts.Panels->SetTopmostPopup(vguiPanel, true);

			int iNewWidth, iNewHeight; // Check for a new resolution, and only adjust if it's above 0 to prevent bugs
			gInts.Engine->GetScreenSize(iNewWidth, iNewHeight);
			if (!iNewWidth || !iNewHeight)
				return;

			if (gScreen.width != iNewWidth || gScreen.height != iNewHeight)
			{
				gDraw.Initialize();
				fonts.init();
			}

			if(gInts.Engine->IsDrawingLoadingImage() || GetAsyncKeyState(VK_F12) || gInts.Engine->IsTakingScreenshot())
				return; // Don't draw when loading or taking screenshots

			//==================================================================================================

			if (gConfig.welcome)
			{
				gInts.Panels->SetMouseInputEnabled(vguiPanel, true);
				gMenu.getInput();
				int x = gScreen.width / 2 - 275, y = gScreen.height / 2 - 150;

				if (gMenu.menuToggle)
					gConfig.welcome = false;

				gDraw.DrawRect(0, 0, gScreen.width, gScreen.height, COLORCODE(0, 0, 0, 80));
				for (int i = 0; i < 3; i++)
					gDraw.OutlineRect(x - 10 - i, y - 10 - i, 570 + (i * 2), 295 + (i * 2), colors.bluTeam);
				gDraw.DrawRect(x, y, 550, 275, COLORCODE(30, 30, 30, 180));

				int strw, strh;
				gInts.Surface->GetTextSize(fonts.verdana_large, L"Welcome to", strw, strh);
				gDrawEx.DrawString(x + 275 - (strw/2), y + (strh / 2) + 20, COLORCODE(180, 180, 180, 255), fonts.verdana_large, "Welcome to");
				gMenu.WaterMark("Sparkly Cheat", x + 145, y + (strh / 2) + 100);

				gInts.Surface->GetTextSize(fonts.verdana_medium, L"Press [Insert] or [F11]!", strw, strh);
				gDraw.DrawRect(x + 275 - (strw / 2) - 15, y + 250, strw + 30, 50, COLORCODE(0, 0, 0, 255));
				gDrawEx.DrawString(x + 275 - (strw / 2), y + 275 - (strh / 2), colors.bluTeam, fonts.verdana_medium, "Press [Insert] or [F11]!");

				return;
			}

			if (gMenu.menuToggle && gMenu.wmShow)
			{
				gMenu.WaterMark(gMenu.wmText.c_str(), 20, 20);
				gDrawEx.DrawString(5, 5, colors.light, fonts.outlinedFont,
					string("Version " + (string)SPARKLY_VERSION).c_str());
#ifndef DONATOR
				gDrawEx.DrawString(gScreen.width / 3, 5, colors.light, fonts.outlinedFont,
					"If you paid for this version, you got scammed");
#endif
				gDrawEx.DrawString(gScreen.width / 3, 18, colors.light, fonts.outlinedFont,
					"Only download the cheat from here: Discord.io/SparklyCheat");
			}

			CBaseEntity* bLocal = GetBaseEntity(me);
			gMisc.main();
			Handler.SetAction("Radar.Draw()");
			gPlayers.drawRadar();

			CPlayer* pLocal = NULL;
			if (bLocal->ToPlayer(pLocal))
			{
				for (int i = 1; i < gInts.EntList->GetHighestEntityIndex(); i++)
				{
					CBaseEntity* current = GetBaseEntity(i);
					if (current == NULL || current->IsDormant())
						continue;
					Handler.SetAction("Esp.Draw()");
					gEsp.processEntity(current, pLocal, i);

					Handler.SetAction("Misc.Draw()");
					gMisc.draw(current, pLocal, i);

					Handler.SetAction("Radar.DrawPlayer()");
					gPlayers.processEntity(current, pLocal, i);
				}
			}
			// Free the mouse if our menu is on
			gInts.Panels->SetMouseInputEnabled(vguiPanel, gMenu.menuToggle ? true : false);
			Handler.SetAction("Playerlist.Draw()");
			gPlayers.drawPlayerlist();

			/*Handler.SetAction("Spectatorlist.Draw()");
			gPlayers.drawSpecList(pLocal);

			if (gMat.armModels.size())
			{
				for (size_t i = 0; i < gMat.armModels.size(); i++)
				{
					gDraw.DrawString(100, 100 + (i*15), colors.white, gMat.armModels[i].name.c_str());
					for (size_t a = 0; a < gMat.armModels[i].matNames.size(); i++)
						gDraw.DrawString(120, 100 + (i * 15) + ((a + 1) * 15), colors.white, gMat.armModels[i].matNames[a].c_str());
				}
				gMat.armModels.clear();
			}*/

			Handler.SetAction("Menu.Draw()");
			gMenu.paint(); // Make sure to draw the menu above everything else
		}
	}
	catch(...)
	{
		MessageBox(NULL, Handler.GetError().c_str(), "ERROR", MB_OK);
	}
}
//===================================================================================
#include <time.h>
void Intro( void )
{
	try
	{
		gMenu.init();
		gConfig.init();

		netEntity.find();
		netPlayer.find();
		netWeapon.find();
		netObject.find();
		gGlobals = gPlayerInfoManager->GetGlobalVars();

		gDraw.Initialize();
		Textures.init();
		fonts.init();
		gMisc.init();

		gInts.Cvar->ConsolePrintf("Sparkly Cheat injected!\n");

		const char* sound;
		srand(time(NULL));

		switch (rand() % 4)
		{
		case 0: sound = "vo/items/wheatley_sapper/wheatley_sapper_intro03.mp3"; break;
		case 1: sound = "vo/items/wheatley_sapper/wheatley_sapper_hacking37.mp3"; break;
		case 2: sound = "vo/items/wheatley_sapper/wheatley_sapper_hacked27.mp3"; break;
		case 3: sound = "vo/items/wheatley_sapper/wheatley_sapper_hacking34.mp3"; break;
		}
		gInts.Surface->PlaySound(sound);
	}
	catch(...)
	{
		MessageBox(NULL, "Failed Intro()\nMaybe your config is broken? Try deleting it and restart the cheat.", "Error", MB_OK);
	}
}