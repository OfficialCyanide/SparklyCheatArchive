#pragma once
#include "SDK.h"
#include "Menu.h"
#include "Entity.h"
#include <vector>

class Esp
{
public:
	float enabled = false;

	float drawPlayer = true;
	float drawTeam = false;
	float drawName = false;
	float drawClass = true;
	float drawBones = false;

	string invisible = "Highlight";
	string drawHealth = "Bar (Left)";

	float health = false;
	float ammo = false;
	float projectiles = false;

	float sentry = true;
	float dispenser = true;
	float teleports = true;
	float doors = false;
	DWORD objectColor = COLORCODE(255, 255, 255, 255);

	float guids = false;

	float customColor = false;
	DWORD playerColor = COLORCODE(50, 100, 255, 255);

	string font = "Small";
	string boxStyle = "2D";
	string textAlign = "Center";
	float boxSize = 2;
	float drunk = false;

	void processEntity(CBaseEntity* entity, CPlayer* pLocal, int entIndex);
	DWORD teamColor(CBaseEntity* player);
	void healthBar(int x, int y, int w, int h, CPlayer* player, bool vertical);

private:
	string itemType(const char* modelName);
	string itemSize(const char* modelName);

	void screenBounds(Vector3 *worldpos, int amount, Vector2 &xy, Vector2 &wh);
	void drawBox(int x, int y, int w, int h, DWORD color);
	void drunkBox(int x, int y, int w, int h, Vector2 topDiff, DWORD color);
};

extern Esp gEsp;