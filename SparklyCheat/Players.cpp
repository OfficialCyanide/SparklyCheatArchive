#include "Players.h"
#include "CDrawManagerEx.h"
#include "commonColors.h"
#include "Entity stuff.h"
#include "Esp.h"
#include "Aim.h"
#include "Fonts.h"
#include "Entity.h"

#include <algorithm>
#include <ctime>

Players gPlayers;

void Players::drawRadar()
{
	if (!radar)
		return;
	
	if (gMenu.menuToggle)
	{
		RadarWin.w = size, RadarWin.h = size;
		gMenu.drawWindow(RadarWin);
	}
	else
	{
		DWORD color = colors.bluTeam;
		gDraw.OutlineRect(RadarWin.x - 1, RadarWin.y - 1, size + 2, size + 2, color);
	}

	gDraw.DrawRect(RadarWin.x, RadarWin.y, size, size, COLORCODE(20, 20, 20, 255 - (int)transparency));

	int radius = size / 2;
	gDraw.DrawRect(RadarWin.x + radius - (radius / 2), RadarWin.y + (size / 2), radius, 1, colors.gray);
	gDraw.DrawRect(RadarWin.x + radius, RadarWin.y + radius - (radius / 2), 1, radius, colors.gray);
}

void Players::drawPlayerlist()
{
	if (!gMenu.menuToggle || !list)
		return;

	int h = 3;
	int w = 100;

	// Creating the playerlist and getting the window's size
	pAmount = 0;
	for (int i = 1; i <= gInts.Engine->GetMaxClients(); i++)
	{
		if (i == me)
			continue;
		CBaseEntity* player = GetBaseEntity(i);
		if (player == NULL)
			continue;

		gInts.Engine->GetPlayerInfo(i, &priorities[pAmount].pInfo);
		priorities[pAmount].color = gEsp.teamColor(player);
		priorities[pAmount].team = *MakePtr(byte*, player, netPlayer.m_iTeamNum);
		priorities[pAmount].index = i;
		h += 14, pAmount++;

		int width, height;
		gInts.Surface->GetTextSize(fonts.smallFont, fonts.to_wchar(priorities[pAmount].pInfo.name), width, height);
		if (width + 55 > w) w = width + 55;

	}

	PlayerWin.h = h, PlayerWin.w = w;
	gMenu.drawWindow(PlayerWin); // Draw the window

	int y = PlayerWin.y + 3;
	for (int team = 1; team <= 3; team++) // Sort the player list by team
	{
		for (int i = 0; i < pAmount; i++) // Drawing all the players on pAmount list
		{
			if (priorities[i].team == team)
			{
				if (gMenu.mouseOver(PlayerWin.x + PlayerWin.w - 40, y, 40, 13))
					handleValue(priorities[i].priority, 0, 2, 1);

				gDrawEx.DrawString(PlayerWin.x + 3, y, priorities[i].color, fonts.smallFont, priorities[i].pInfo.name);
				gDrawEx.DrawString(PlayerWin.x + PlayerWin.w - 40, y, pColors[priorities[i].priority], fonts.smallFont, pString[priorities[i].priority]);
				y += 14;
			}
		}
	}
}

void Players::drawSpecList(CPlayer* pLocal)
{
	if (!gMenu.menuToggle || !spectators)
		return;

	int h = 3;
	int w = 100;

	// Creating the playerlist and getting the window's size
	CPlayer* spectators[32];
	pAmount = 0;
	if (pLocal != NULL)
		for (int i = 1; i <= gInts.Engine->GetMaxClients(); i++)
		{
			if (i == me)
				continue;
			CBaseEntity* current = GetBaseEntity(i);
			CPlayer* player;
			if (!current->ToPlayer(player) || (player->Lifestate() == LIFE_ALIVE && player->Team() > 1) || !player->ObserverTarget() || player->ObserverTarget() != pLocal)
				continue;

			spectators[pAmount] = player;
			h += 14, pAmount++;
			int width, height;
			player_info_s pInfo;
			gInts.Engine->GetPlayerInfo(i, &pInfo);
			gInts.Surface->GetTextSize(fonts.smallFont, fonts.to_wchar(pInfo.name), width, height);
			if (width + 6 > w)
				w = width + 6;
		}

	SpecWin.h = h, SpecWin.w = w;
	gMenu.drawWindow(SpecWin); // Draw the window

	int y = SpecWin.y + 3;
	for (int team = 1; team <= 3; team++) // Sort the player list by team
		for (int i = 0; i < pAmount; i++) // Drawing all the players on pAmount list
			if (spectators[i]->Team() == team)
			{
				gDrawEx.DrawString(SpecWin.x + 3, y, gEsp.teamColor(spectators[i]), fonts.smallFont, spectators[i]->Info().name);
				y += 14;
			}
}

void Players::processEntity(CBaseEntity* current, CPlayer* pLocal, int entIndex)
{
	if (!radar)
		return;
	if (!gInts.Engine->IsInGame())
		return;
	if (entIndex == me)
		return;

	CPlayer* player;
	if (!current->ToPlayer(player))
		return;
	
	if (player->Lifestate() != LIFE_ALIVE) // If the player isn't alive, then skip
		return;

	CBaseEntity* us = GetBaseEntity(me);
	if (!team && player->Team() == us->Team())
		return;

	DWORD color = entIndex == gAim.target ? colors.green : gEsp.teamColor(player);
	Vector3 cWorld = player->GetAbsOrigin(), iWorld = us->GetAbsOrigin();
	Vector2 cPos(cWorld.x, cWorld.y), iPos(iWorld.x, iWorld.y);
	Vector2 dist = cPos - iPos;

	if (abs(dist.x) > maxDist || abs(dist.y) > maxDist)
		return;

	int radius = size / 2;
	dist = Vector2(radius * (dist.x / maxDist), radius * (dist.y / maxDist)), dist.y = -dist.y;
	Vector2 center(RadarWin.x + radius, RadarWin.y + radius);

	dist = dist + center;
	Vector3 angles; gInts.Engine->GetViewAngles(angles);
	Vector2 rPos = rotate_vec2d(center, DEG2RAD1(angles.y - 90), dist);

	if (rPos.x < RadarWin.x) // Clamp points within the distance
		rPos.x = RadarWin.x;
	else if (rPos.x > RadarWin.x + size)
		rPos.x = RadarWin.x + size;
	if (rPos.y < RadarWin.y)
		rPos.y = RadarWin.y;
	else if (rPos.y > RadarWin.y + size)
		rPos.y = RadarWin.y + size;
	
	int half = (pSize / 2);
	gDraw.DrawRect(rPos.x - half + 1, rPos.y - half + 1, pSize - 2, pSize - 2, color);
	gDraw.OutlineRect(rPos.x - half, rPos.y - half, pSize, pSize, colors.black);

	if (drawHealth)
	{
		int health = player->Health(), max = player->MaxHealth();
		if (health > max) health = max;
		float percent = (float)health / max;
		DWORD hpColor = gDrawEx.FromHSB(percent / 3, 0.8f, 0.7f, 255);

		gDraw.DrawRect(rPos.x - half, rPos.y + half + 1, percent * pSize, 2, hpColor);
	}
}

void Players::handleValue(int &value, float min, float max, float step)
{
	static clock_t ldown = clock(), rdown = clock();

	if (!gMenu.keyDown(VK_LBUTTON))
		ldown = clock();
	if (!gMenu.keyDown(VK_RBUTTON))
		rdown = clock();

	if (!gMenu.canFocus("Priority list"))
		return;

	if (gMenu.mb == e_mb::LCLICK || (clock() - ldown > 500 && (clock() - ldown) % 5 == 0))
	{
		// If you're about to go below the min, then go back to the max
		if (value - step < min)
			value = max;

		else // Otherwise, keep going down
			value -= step;
	}
	else if (gMenu.mb == e_mb::RCLICK || (clock() - rdown > 500 && (clock() - rdown) % 5 == 0))
	{
		// If you're about to go above the max, then go back to the min
		if (value + step > max)
			value = min;

		else // Otherwise, keep going up
			value += step;
	}
}