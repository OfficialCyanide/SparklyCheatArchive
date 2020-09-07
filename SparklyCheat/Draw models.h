#pragma once
#include "Entity.h"
#include "Material.h"
#include <vector>

static vector<studiohdr_t*> restoreMats;

void WINAPI Hooked_DrawModelExecute(const DrawModelState_t &state,
	const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld = NULL);

void Original_DrawModelExecute(const DrawModelState_t &state,
	const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld = NULL);