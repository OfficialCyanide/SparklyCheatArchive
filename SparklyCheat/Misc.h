#pragma once
#include "Entity.h"
#include <vector>

class Misc
{
public:
	float paths = false;
	float local = false;
	float trace = false;
	float hitpos = false;
	float debug = false;
	float cheatBp = false;
	float killAllCvars = false;

	float bhop = false;
	float fov = 0;
	float viewmodel = true;
	string scope = "None";

	string condkey = "None";
	float condAmount = 500;
	float wepSwitch = false;

	float skinChanger = false;
	string rifle = "Default";
	string rocket = "Default";
	string uwep = "None";
	string utaunt = "None";
	float australium = false;

	float greenscreen = false;
	DWORD colorscreen = COLORCODE(0, 255, 0, 255);
	float setscreen = false;
	float xscreen = 0;
	float yscreen = 0;
	float zscreen = 0;
	float wscreen = 200;
	float hscreen = 200;
	float rotscreen = 0;

	/*float chatspam = false;
	vector<char*> chatSpams =
	{
		"say Discord.io/SparklyCheat",
		"say Sparkly Cheat! Comes with better prediction than Lithium!",
		"say Sparkly Cheat has better slow aim than Null Core!",
		"say Sparkly Cheat is way cheaper than Aimware!",
		"say Completely free and 100% worth it! Discord.io/SparklyCheat"
	};*/

	void command(CUserCmd* pCommand, CPlayer* pLocal);
	void draw(CBaseEntity* current, CPlayer* pLocal, int entIndex);
	void main();
	void drawDebug();
	void init();
private:
	bool allCvarsKilled = false;
	void killCvar(ConCommandBase* target);
	Vector3 predPosAt(float flTime, CBaseEntity* target);
	void drawWorldCircle(Vector3 &pos, Vector3 &normal, float radius, int segments, DWORD dwColor);

	int cheatFlags;
};
extern Misc gMisc;