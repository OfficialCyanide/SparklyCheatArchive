#pragma once
#include "Material.h"
#include <vector>

struct modelstuff
{
	string name = "";
	vector<string> matNames;
};
class Mathack
{
public:
	float enabled = false;

	string chams = "None";
	string visibility = "Behind walls";

	string hands = "Normal";

	vector<modelstuff> armModels;

	void DrawModel(const DrawModelState_t &state,
		const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld);
};
extern Mathack gMat;