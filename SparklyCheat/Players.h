#pragma once
#include "Entity.h"
#include "Menu.h"

struct ListInfo
{
	player_info_s	pInfo;
	DWORD			color = COLORCODE(0, 0, 0, 255);
	int				team = 0;
	int				index = -1;
	int				priority = 1;
};

class Players
{
public:
	float	list = false;
	float	spectators = false;
		  
	float	radar = false;
	float	transparency = 75;
	float	pSize = 6;
	float	size = 150;
	float	maxDist = 2000;
		  
	float	team = false;
	float	drawHealth = false;

	void	drawRadar();
	void	drawPlayerlist();
	void	drawSpecList(CPlayer* pLocal);
	void	processEntity(CBaseEntity* current, CPlayer* pLocal, int entIndex);
	Window	RadarWin, PlayerWin, SpecWin;

	ListInfo priorities[32];
	int		pAmount = 0;

	DWORD pColors[3] = { COLORCODE(90, 235, 60, 255), COLORCODE(140, 140, 140, 255), COLORCODE(240, 230, 60, 255) };
private:
	const char* pString[3] = { "Friend", "Normal", "Rage" };
	void handleValue(int &value, float min, float max, float step);
};
extern Players gPlayers;