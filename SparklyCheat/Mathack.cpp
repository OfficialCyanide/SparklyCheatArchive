#include "Mathack.h"
#include "Entity.h"
#include "Draw models.h"
#include "Esp.h"
#include "Menu.h"

Mathack gMat;

inline void NormalFlags(IMaterial* mat)
{
	mat->SetMaterialVarFlag(MaterialVarFlags_t::MATERIAL_VAR_WIREFRAME, false);
	mat->SetMaterialVarFlag(MaterialVarFlags_t::MATERIAL_VAR_NO_DRAW, false);
	mat->SetMaterialVarFlag(MaterialVarFlags_t::MATERIAL_VAR_FLAT, false);
	mat->SetMaterialVarFlag(MaterialVarFlags_t::MATERIAL_VAR_IGNOREZ, false);
}

inline void SetFlag(IMaterial* mat, MaterialVarFlags_t flag, bool enabled)
{
	//if (mat->GetMaterialVarFlag(flag) != enabled)
		mat->SetMaterialVarFlag(flag, enabled);
}

inline void ForceMaterial(Color color, IMaterial* material, bool useColor, bool forceMaterial)
{
	if (useColor)
	{
		float blend[3] = { (float)color.r / 255.f, (float)color.g / 255.f, (float)color.b / 255.f };
		float alpha = (float)color.a / 255.f;

		gInts.RenderView->SetBlend(alpha);
		gInts.RenderView->SetColorModulation(blend);
	}

	if (forceMaterial)
		gInts.ModelRender->ForcedMaterialOverride(material);
}

void Mathack::DrawModel(const DrawModelState_t &state,
	const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld)
{
	static IMaterial* newMat = nullptr;
	if (newMat == nullptr)
		newMat = gInts.MatSystem->CreateMaterial(true, false, true);

	ForceMaterial(Color(255, 255, 255), newMat, true, true);

	return;
	string pszModelName = gInts.ModelInfo->GetModelName((DWORD*)pInfo.pModel);
	if (pszModelName.find("arms") != string::npos)
	{
		IMaterial* mats[MAXSTUDIOSKINS];
		studiohdr_t* hdrHands = gInts.ModelInfo->GetStudiomodel((DWORD*)pInfo.pModel);
		gInts.ModelInfo->GetModelMaterials((model_t*)pInfo.pModel, hdrHands->numtextures, mats);

		if (!enabled)
			return restoreMats.push_back(hdrHands);
		else if (hands == "Normal")
			return;

		modelstuff model;
		model.name = hdrHands->name;

		for (int i = 0; i < hdrHands->numtextures; i++)
		{
			IMaterial *matHands = mats[i];
			if (!matHands)
				continue;

			model.matNames.push_back(matHands->GetName());

			if (hands == "Hide")
				SetFlag(matHands, MaterialVarFlags_t::MATERIAL_VAR_NO_DRAW, true);
			else if (hands == "Wireframe")
			{
				NormalFlags(matHands);
				SetFlag(matHands, MaterialVarFlags_t::MATERIAL_VAR_WIREFRAME, true);

				float baseColor[4] = { 5, 5, 5, 255 };
				gInts.RenderView->SetColorModulation(baseColor);
			}
			else if (hands == "Wireframe overlay")
			{
				NormalFlags(matHands);

				float baseColor[4] = { 1, 1, 1, 255 };
				gInts.RenderView->SetColorModulation(baseColor);
				Original_DrawModelExecute(state, pInfo, pCustomBoneToWorld);

				SetFlag(matHands, MaterialVarFlags_t::MATERIAL_VAR_WIREFRAME, true);

				baseColor[0] = 10, baseColor[1] = 10, baseColor[2] = 10;
				gInts.RenderView->SetColorModulation(baseColor);
			}
		}

		//armModels.push_back(model);
	}
}