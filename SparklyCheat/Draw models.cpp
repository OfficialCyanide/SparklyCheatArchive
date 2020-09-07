#include "Draw models.h"
#include "Mathack.h"

static VMTManager* hook;

inline void NormalFlags(IMaterial* mat)
{
	mat->SetMaterialVarFlag(MaterialVarFlags_t::MATERIAL_VAR_WIREFRAME, false);
	mat->SetMaterialVarFlag(MaterialVarFlags_t::MATERIAL_VAR_NO_DRAW, false);
	mat->SetMaterialVarFlag(MaterialVarFlags_t::MATERIAL_VAR_FLAT, false);
	mat->SetMaterialVarFlag(MaterialVarFlags_t::MATERIAL_VAR_IGNOREZ, false);
}

void WINAPI Hooked_DrawModelExecute(const DrawModelState_t &state,
	const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld)
{
	try
	{
		hook = &VMTManager::GetHook(gInts.ModelRender);
		Handler.SetAction("Donator.Mathack()");
		gMat.DrawModel(state, pInfo, pCustomBoneToWorld);
		
		if (restoreMats.size())
		{
			for (size_t i = 0; i < restoreMats.size(); i++)
			{
				if (restoreMats[i] == nullptr)
					continue;

				IMaterial* mats[MAXSTUDIOSKINS];
				gInts.ModelInfo->GetModelMaterials((model_t*)pInfo.pModel, restoreMats[i]->numtextures, mats);

				for (int a = 0; a < restoreMats[i]->numtextures; a++)
				{
					IMaterial *matHands = mats[a];
					if (!matHands)
						continue;

					NormalFlags(matHands);
				}
			}
			restoreMats.clear();
		}

		Original_DrawModelExecute(state, pInfo, pCustomBoneToWorld);
	}
	catch (...)
	{
		//MessageBox(NULL, string("Failed " + Handler.GetAction()).c_str(), "ERROR", MB_OK);
	}
}

void Original_DrawModelExecute(const DrawModelState_t &state,
	const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld)
{
	return hook->GetMethod<void(__thiscall *)(CModelRender*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4*)>(19)(gInts.ModelRender, state, pInfo, pCustomBoneToWorld);
}