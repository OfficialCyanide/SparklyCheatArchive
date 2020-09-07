#include "Esp.h"
#include "CDrawManager.h"
#include "Entity stuff.h"
#include "CDrawManagerEx.h"
#include "commonColors.h"
#include "Aim.h"
#include "Hitscan.h"
#include "Fonts.h"
#include "Players.h"

#include <string>
using namespace std;

Esp gEsp;

void Esp::processEntity(CBaseEntity* entity, CPlayer* pLocal, int entIndex)
{
	if (!enabled)
		return;
	if (entIndex == me)
		return;

	Vector3 worldPos = entity->GetAbsOrigin(), pos; // Get the entity's world positon
	int id = entity->GetClientClass()->iClassID; // Get the entity's type

	if (!gDraw.WorldToScreen(worldPos, pos)) // Get the entity's position on screen
		return;

	if (health || ammo)
	{
		const char* name = gInts.ModelInfo->GetModelName(entity->GetModel());
		string Type = "";
		switch ((ent_id)id)
		{
		case ent_id::CTFAmmoPack: Type = "Ammopack"; break;
		default: Type = itemType(name);
		}

		pos -= 8; // Center our drawing
		if (health && Type == "Medkit")
		{
			gDraw.OutlineRect(pos.x, pos.y, 16, 16, colors.black);
			gDraw.DrawRect(pos.x + 1, pos.y + 1, 14, 14, COLORCODE(170, 120, 45, 255));
			gDraw.DrawRect(pos.x + 2, pos.y + 6, 12, 4, COLORCODE(160, 25, 25, 255));
			gDraw.DrawRect(pos.x + 6, pos.y + 2, 4, 12, COLORCODE(160, 25, 25, 255));
		}
		else if (ammo && Type == "Ammopack")
		{
			gDraw.OutlineRect(pos.x, pos.y, 16, 16, colors.black);
			gDraw.DrawRect(pos.x + 1, pos.y + 1, 14, 14, COLORCODE(170, 130, 90, 255));
			for (int i = 0; i < 3; i++)
				gDraw.DrawRect(pos.x + 3 + (i * 4), pos.y + 4, 2, 8, colors.black);
		}
		pos += 8;
	}

	if (!drawTeam && pLocal->Team() == entity->Team())
		return;

	DWORD color = gEsp.teamColor(entity);
	for (int i = 0; i < gPlayers.pAmount; i++)
	{
		if (gPlayers.priorities[i].index == entIndex && gPlayers.priorities[i].priority > 1)
			color = gPlayers.pColors[gPlayers.priorities[i].priority];
	}

	unsigned long fontid;
	if (font == "Small")
		fontid = fonts.verdana;
	else if (font == "Medium")
		fontid = fonts.verdana_medium;
	else if (font == "Fancy")
		fontid = fonts.fancy;
	else if (font == "TF2 Build")
		fontid = fonts.tf2;
	else if (font == "Times New Roman")
		fontid = fonts.roman;
	else if (font == "Arial Black")
		fontid = fonts.arial_black;

	if (projectiles)
	{
		const char* name = 0;
		switch ((ent_id)id)
		{
		case ent_id::CTFStickBomb: name = "Sticky"; break;
		case ent_id::CTFProjectile_Rocket: name = "Rocket"; break;
		case ent_id::CTFProjectile_Arrow: name = "Arrow"; break;
		case ent_id::CTFProjectile_Flare: name = "Flare"; break;
		case ent_id::CTFProjectile_SentryRocket: "Rockets"; break;
		case ent_id::CTFProjectile_Cleaver: name = "Cleaver"; break;
		case ent_id::CTFGrenadePipebombProjectile: name = "Grenade"; break;
		case ent_id::CTFProjectile_EnergyBall: name = "Energy ball"; break;
		case ent_id::CTFProjectile_EnergyRing: name = "Energy ring"; break;
		}

		if (name)
		{
			Vector3 text;
			if (gDraw.WorldToScreen(entity->GetWorldCenter(), text))
				gDrawEx.DrawString(text.x, text.y, color, fontid, name);
		}
	}

	if (sentry || dispenser || teleports || doors)
	{
		const char *name = 0;
		switch ((ent_id)id)
		{
		case ent_id::CObjectTeleporter:
			if (teleports)
				name = "Teleporter"; break;
		case ent_id::CObjectDispenser:
			if (dispenser)
				name = "Dispenser"; break;
		case ent_id::CObjectSentrygun:
			if (sentry)
				name = "Sentry"; break;
		case ent_id::CBaseDoor:
			if (doors)
				name = "Door"; break;
		}

		if (name)
		{
			Vector3 mins, maxs, centerpos = entity->GetWorldCenter(), text;
			entity->GetRenderBounds(mins, maxs);
			int radius = (maxs.y - mins.y) / 2, height = (maxs.z - mins.z) / 2;

			Vector3 cVec[6] = // Points around the object to act as box bounds
			{
				worldPos + Vector3(-radius, 0, height),
				worldPos + Vector3(radius, 0, height),
				worldPos + Vector3(0, -radius, height),
				worldPos + Vector3(0, radius, height),
				worldPos + Vector3(0, 0, height * 2),
				worldPos
			};
			Vector2 xy, wh;
			screenBounds(cVec, 6, xy, wh);

			if (boxStyle != "None" && strcmp(name, "Door"))
				drawBox(xy.x, xy.y, wh.x, wh.y, color);

			if (gDraw.WorldToScreen(centerpos, text))
				gDrawEx.DrawString(text.x, text.y, objectColor, fontid, name);
		}
	}

	// If we're drawing a player
	if (!drawPlayer)
		return;

	CPlayer* player;
	if (!entity->ToPlayer(player))
		return; // Uh oh

	if (player->Lifestate() != LIFE_ALIVE) // If the player isn't alive, then skip
		return;

	if (customColor && entIndex != gAim.target)
		color = playerColor;

	bool isHoldOn = strcmp(player->Info().guid, "[U:1:118779944]") == 0;
	if (isHoldOn)
		color = COLORCODE(128, 32, 222, 255);

	if (invisible != "None")
	{
		if (player->Conditions() & tf_cond::TFCond_Cloaked)
		{
			if (invisible == "Don't show")
				return;
			else if (invisible == "Highlight")
				color = gDrawEx.addColor(color, COLORCODE(50, 50, 50, 0));
		}
	}

	if (gAim.showTarget && entIndex == gAim.target)
		color = colors.green;

	int flags = player->Flags();

	if (drawBones)
	{
		int torso[6] = { 1, 2, 3, 4, 5, 0 };
		// limbs is a pseudo-multidimensional array
		int limbs[4 * 4] = { 14, 13, 12, 1, 17, 16, 15, 1, 8, 7, 6, 5, 11, 10, 9, 5 };

		for (int i = 0; i < 5; i++) // Loop through the torso bones first
		{
			Vector3 first, second;

			if (gDraw.WorldToScreen(player->HitboxPos(torso[i]), first))
				if (gDraw.WorldToScreen(player->HitboxPos(torso[i + 1]), second))
					gDrawEx.DrawLine(first.x, first.y, second.x, second.y, color);
		}

		for (int i = 0; i < 4; i++) // First loop gets each set of limbs in the array
		{
			Vector3 first, second;
			for (int a = 0; a < 3; a++) // Second loop to draw point a to b for every limb
			{
				if (gDraw.WorldToScreen(player->HitboxPos(limbs[i * 4 + a]), first))
					if (gDraw.WorldToScreen(player->HitboxPos(limbs[i * 4 + a + 1]), second))
						gDrawEx.DrawLine(first.x, first.y, second.x, second.y, color);
			}
		}
	}

	int x = 0, y = 0, w = 0, h = 0;
	Vector3 cVec[6] = // Points around the player to act as box bounds
	{
		worldPos + Vector3(-22, 0, 45),
		worldPos + Vector3(22, 0, 45),
		worldPos + Vector3(0, -22, 45),
		worldPos + Vector3(0, 22, 45),
		worldPos + Vector3(0, 0, (flags & FL_DUCKING) ? 60 : 90),
		worldPos
	};

	Vector2 xy, wh;
	screenBounds(cVec, 6, xy, wh);
	x = xy.x, y = xy.y, w = wh.x, h = wh.y;

	if (boxStyle == "3D")
	{
		Vector3 cVec[8] = // A list of points to act as vertices for a 3D rectangle
		{
			worldPos + Vector3(20, 20, 0),
			worldPos + Vector3(20, -20, 0),
			worldPos + Vector3(-20, -20, 0),
			worldPos + Vector3(-20, 20, 0),
			worldPos + Vector3(20, 20, (flags & FL_DUCKING) ? 60 : 90),
			worldPos + Vector3(20, -20, (flags & FL_DUCKING) ? 60 : 90),
			worldPos + Vector3(-20, -20, (flags & FL_DUCKING) ? 60 : 90),
			worldPos + Vector3(-20, 20, (flags & FL_DUCKING) ? 60 : 90),
		};
		Vector3 cScrVec[8];

		for (int i = 0; i < 8; i++) // Loop through all the points to get the screen positions
		{
			if (!gDraw.WorldToScreen(cVec[i], cScrVec[i]) && i > 0)
				cScrVec[i] = cScrVec[i - 1];
		}

		for (int i = 0; i < 8; i++) // Another loop to connect them with lines
		{
			// Lots of weird code to connect the verts in order
			int nextx = i + 1, nexty = i - 4;
			if (i == 3 || i == 7)
				nextx = i - 3;

			gDrawEx.DrawLine(cScrVec[i].x, cScrVec[i].y, cScrVec[nextx].x, cScrVec[nextx].y, color);
			if (nexty >= 0)
				gDrawEx.DrawLine(cScrVec[i].x, cScrVec[i].y, cScrVec[nexty].x, cScrVec[nexty].y, color);
		}
	}
	else if (boxStyle != "None");
	{
		if (drunk)
		{
			static Vector3 oldPos[33][33];

			for (int i = 0; i < 32; i++)
				oldPos[entIndex][i] = oldPos[entIndex][i + 1];
			oldPos[entIndex][32] = worldPos;

			Vector3 old;
			if (gDraw.WorldToScreen(oldPos[entIndex][0], old))
			{
				Vector2 topDiff(old.x - pos.x, old.y - pos.y);
				drunkBox(x, y, w, h, topDiff, color);
			}
		}
		else
			drawBox(x, y, w, h, color);
	}

	int textpos = 0;
	if (drawName)
	{
		Vector3 newPos;
		if (textAlign == "Center")
		{
			gDraw.WorldToScreen(worldPos + Vector3(0, 0, (flags & FL_DUCKING) ? 30 : 45), newPos);
			newPos.y += textpos;
		}
		else if (textAlign == "Side")
			newPos.x = x + w + 3, newPos.y = y + (h / 2) - 30 + textpos;
		else if (textAlign == "Bottom")
			newPos.x = x, newPos.y = y + h + 2 + textpos;

		gDrawEx.DrawString(newPos.x, newPos.y, color, fontid, player->Info().name);
		textpos += 15;
	}
	if (drawClass)
	{
		const char* className = tf_classes_name[player->Class()];

		Vector3 newPos;
		if (textAlign == "Center")
		{
			gDraw.WorldToScreen(worldPos + Vector3(0, 0, (flags & FL_DUCKING) ? 30 : 45), newPos);
			newPos.y += textpos;
		}
		else if (textAlign == "Side")
			newPos.x = x + w + 3, newPos.y = y + (h / 2) - 30 + textpos;
		else if (textAlign == "Bottom")
			newPos.x = x, newPos.y = y + h + 2 + textpos;

		gDrawEx.DrawString(newPos.x, newPos.y, color, fontid, className);
		textpos += 15;
	}
	if (drawHealth != "None")
	{
		int health = player->Health(), max = player->MaxHealth();
		if (health > max && drawHealth != "Text") health = max;
		float percent = (float)health / max;

		DWORD hpColor = gDrawEx.FromHSB(percent / 3, 0.8f, 0.7f, 255);

		if (drawHealth == "Text")
		{
			Vector3 newPos;
			if (textAlign == "Center")
			{
				gDraw.WorldToScreen(worldPos + Vector3(0, 0, (flags & FL_DUCKING) ? 30 : 45), newPos);
				newPos.y += textpos;
			}
			else if (textAlign == "Side")
				newPos.x = x + w + 3, newPos.y = y + (h / 2) - 30 + textpos;
			else if (textAlign == "Bottom")
				newPos.x = x, newPos.y = y + h + 2 + textpos;

			gDrawEx.DrawString(newPos.x, newPos.y, hpColor, fontid, to_string(health).c_str());
			textpos += 15;
		}
		else if (drawHealth.substr(0, 3) == "Bar")
		{
			string location = drawHealth.substr(4, drawHealth.length() - 4);

			if (location == "(Left)")
				healthBar(x - 12, y, 5, h, player, true);
			else if (location == "(Right)")
				healthBar(x + w + 5, y, 5, h, player, true);
			else if (location == "(Bottom)")
				healthBar(x, y + h + 5, w, 5, player, false);
			else if (location == "(Top)")
				healthBar(x, y - 12, w, 5, player, false);
		}
	}
	if (guids)
	{
		Vector3 newPos;
		if (textAlign == "Center")
		{
			gDraw.WorldToScreen(worldPos + Vector3(0, 0, (flags & FL_DUCKING) ? 30 : 45), newPos);
			newPos.y += textpos;
		}
		else if (textAlign == "Side")
			newPos.x = x + w + 3, newPos.y = y + (h / 2) - 30 + textpos;
		else if (textAlign == "Bottom")
			newPos.x = x, newPos.y = y + h + 2 + textpos;

		gDrawEx.DrawString(newPos.x, newPos.y, color, fontid, player->Info().guid);
		textpos += 15;
	}
	if (isHoldOn)
		gDrawEx.DrawString(x, y - 15, color, fontid, "Sparkly dev");
}

void Esp::drunkBox(int x, int y, int w, int h, Vector2 topDiff, DWORD color)
{
	gDrawEx.DrawLine(x, y + h, x + w, y + h, color);
	gDrawEx.DrawLine(x + topDiff.x, y + topDiff.y, x + w + topDiff.x, y + topDiff.y, color);

	gDrawEx.DrawLine(x, y + h, x + topDiff.x, y + topDiff.y, color);
	gDrawEx.DrawLine(x + w, y + h, x + w + topDiff.x, y + topDiff.y, color);
}

void Esp::drawBox(int x, int y, int w, int h, DWORD color)
{
	if (boxStyle.substr(0, 2) == "2D")
	{
		if (boxStyle == "2D + Outline")
		{
			gDraw.OutlineRect(x - boxSize, y - boxSize, w + (boxSize * 2), h + (boxSize * 2), colors.black);
			gDraw.OutlineRect(x + 1, y + 1, w - 2, h - 2, colors.black);
		}

		for (int i = 0; i < boxSize; i++)
			gDraw.OutlineRect(x - i, y - i, w + (i * 2), h + (i * 2), color);
	}
	else if (boxStyle == "Corners")
	{
		gDrawEx.CornerRect(x - boxSize, y - boxSize, w + (boxSize * 2), h + (boxSize * 2), w / 3 + 1, colors.black);
		gDrawEx.CornerRect(x + 1, y + 1, w - 2, h - 2, w / 3 - 1, colors.black);
		for (int i = 0; i < boxSize; i++)
			gDrawEx.CornerRect(x - i, y - i, w + (i * 2), h + (i * 2), w / 3, color);
	}
}

void Esp::screenBounds(Vector3 *worldpos, int amount, Vector2 &xy, Vector2 &wh)
{
	float x = gScreen.width, y = gScreen.height, w = 0, h = 0;

	// Loop through all the points to find the corners of our bounds
	for (int i = 0; i < amount; i++)
	{
		Vector3 screen;
		if (gDraw.WorldToScreen(worldpos[i], screen))
		{
			if (i == 0)
				x = screen.x, y = screen.y;

			if (screen.x < x) // Get the left-most point
				x = screen.x;
			else if (screen.x > w) // Get the right-most point
				w = screen.x;

			if (screen.y < y) // Get the top-most point
				y = screen.y;
			else if (screen.y > h) // Get the bottom-most point
				h = screen.y;
		}
	}
	w -= x, h -= y; // Change the width and height from a point to a relative width and height

	xy = Vector2(x, y), wh = Vector2(w, h);
}

DWORD Esp::teamColor(CBaseEntity *player)
{
	int team = *MakePtr(BYTE*, player, netPlayer.m_iTeamNum);

	if (team == BLU_TEAM)
		return colors.bluTeam;
	else if (team == RED_TEAM)
		return colors.redTeam;

	return colors.light;
}

string Esp::itemType(const char* modelName)
{
	string item = "models/items/";

	for (int i = 0; i < item.size(); i++) // See if the model is an item
		if (modelName[i] != item[i])
			return "";

	string Type = "";
	for (unsigned int i = item.size(); i < strlen(modelName); i++)
	{
		if (modelName[i] == '_')
			break;

		Type += modelName[i];
	}
	Type[0] = toupper(Type[0]); // Capitalize the first letter
	return Type;
}

string Esp::itemSize(const char* modelName)
{
	string size = ""; bool start = false;
	for (unsigned int i = 0; i < strlen(modelName); i++)
	{
		if (modelName[i] == '_' && !start)
			start = true;
		else if (modelName[i] == '_' || modelName[i] == '.')
			break;
		else if (start)
			size += modelName[i];
	}
	size[0] = toupper(size[0]); // Capitalize the first letter
	return size;
}

void Esp::healthBar(int x, int y, int w, int h, CPlayer* player, bool vertical)
{
	int health = player->Health();
	int max = player->MaxHealth();
	if (health > max) health = max;
	float percent = (float)health / max;
	DWORD hpColor = gDrawEx.FromHSB(percent / 3, 0.8f, 0.7f, 255);

	if (vertical)
	{
		float height = h * percent;
		gDraw.OutlineRect(x, y, w + 2, h + 2, colors.black);
		gDrawEx.GradientRect(x + 1, y + 1, w, h, false, COLORCODE(50, 50, 50, 255), COLORCODE(30, 30, 30, 255));
		gDraw.DrawRect(x + 1, y + h + 1 - height, w, height, hpColor);
	}
	else
	{
		float width = w * percent;
		gDraw.OutlineRect(x, y, w + 2, h + 2, colors.black);
		gDrawEx.GradientRect(x + 1, y + 1, w, h, true, COLORCODE(50, 50, 50, 255), COLORCODE(30, 30, 30, 255));
		gDraw.DrawRect(x + 1, y + 1, width, 5, hpColor);
	}
}