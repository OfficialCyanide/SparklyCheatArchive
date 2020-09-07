#pragma once
#include "Entity.h"

struct s_resolve
{
	char guid[33];
	float pitch;
};

class Autism
{
public:
	void command(CUserCmd *pCommand, CPlayer* pLocal);
	void speedHack(CUserCmd *pCommand);

	float nospread = false;
	float speedAmount = false;
	string key = "None";

	float resolver = false;
	float animations = false;
	string pForce = "Auto";
	string yForce = "None";

	float nopush = false;
	float thirdperson = false;
	float realangles = true;

	float antiaim = false;
	string pitch = "Fake up";
	float pAngle = 0;

	float spin = 1;
	string yaw = "Edge";
	float yAngle = 0;

	Vector3 lastAngles = Vector3(0, 0, 0);
};

extern Autism gAutist;