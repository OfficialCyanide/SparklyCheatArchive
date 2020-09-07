#include "FrameStageNotify.h"
#include "Entity.h"
#include "Misc.h"
#include "HvH.h"
#include "Misc.h"
#include "Aim.h"
#include "Vector.h"

#ifdef _DEBUG
#define GAYLOL
#undef _DEBUG
#endif

#include "utlvector.h"

#ifdef GAYLOL
#define _DEBUG 1
#endif

#include <string>

class CAttribute
{
public:
	void *pad;
	uint16_t m_iAttributeDefinitionIndex;
	float m_flValue;
	unsigned int uintPad;

	inline CAttribute(uint16_t iAttributeDefinitionIndex, float flValue)
	{
		m_iAttributeDefinitionIndex = iAttributeDefinitionIndex;
		m_flValue = flValue;
	}
};

class CAttributeList
{
public:
	void *pad;
	CUtlVector<CAttribute, CUtlMemory<CAttribute>> m_Attributes;

	inline bool IsEmpty()
	{
		return m_Attributes.Size() == 0;
	}

	inline void AddAttribute(int iIndex, float flValue)
	{
		for (int i = 0; i < m_Attributes.Count(); i++)
		{
			CAttribute* at = &m_Attributes[i];
			if (at->m_iAttributeDefinitionIndex == iIndex)
			{
				if (at->m_flValue != flValue)
					at->m_flValue = flValue;
				return;
			}
		}

		//15 = MAX_ATTRIBUTES
		if (m_Attributes.Count() > 14)
			return;

		CAttribute attr(iIndex, flValue);
		m_Attributes.AddToTail(attr);
	}

	inline void RemoveAttribute(int iIndex)
	{
		for (int i = 0; i < m_Attributes.Count(); i++)
		{
			if (m_Attributes[i].m_iAttributeDefinitionIndex == iIndex)
				return m_Attributes.FastRemove(i);
		}
	}
};

enum attrib_ids
{
	//Apply these for australium item
	AT_is_australium_item = 2027,
	AT_loot_rarity = 2022,
	AT_item_style_override = 542,

	//This one and item style override 0 to golden pan
	AT_ancient_powers = 150,

	AT_uses_stattrack_module = 719,

	AT_is_festive = 2053,

	//Only ks sheen is visible
	AT_sheen = 2014,

	AT_unusual_effect = 134,
	AT_particle_effect = 370,

	AT_weapon_allow_inspect = 731,
};

enum unusual_effects
{
	WE_hot = 701,
	WE_isotope = 702,
	WE_cool = 703,
	WE_energyorb = 704,

	HE_sparkle = 4,
	HE_buringflames = 13,
	HE_lanterns = 37,
	HE_terrorwat = 57,
	HE_memoryleak = 65,
	UT_72 = 3004,
	UT_holygrail = 3003,
	UT_fireworks_blue = 3002,
	UT_fireworks_red = 3001,
};

#include "inetchannel.h"
#include "Entity stuff.h"
#include "Trigger.h"
void __stdcall Hooked_FrameStageNotify(ClientFrameStage_t curStage)
{
	auto &hook = VMTManager::GetHook(gInts.Client);

	try
	{
		Handler.SetAction("Hooked_FrameStageNotify()");
		CBaseEntity* pLocal = GetBaseEntity(me);
		CPlayer* you;
		if (!pLocal || !pLocal->ToPlayer(you) || you->Lifestate() != LIFE_ALIVE)
			return hook.GetMethod<void(__thiscall *)(PVOID, ClientFrameStage_t)>(gOffsets.iFrameStageOffset)(gInts.Client, curStage);

		if (gMisc.scope == "No zoom" || gMisc.scope == "Both" || gMisc.fov)
		{
			static ConVar* fov_desired = gInts.Cvar->FindVar("fov_desired");
			int *fovPtr = MakePtr(int*, pLocal, netPlayer.m_iFOV);
			*fovPtr = gMisc.fov ? gMisc.fov : fov_desired->GetInt();
			*MakePtr(float*, pLocal, netPlayer.m_flFOVRate) = 0;
		}

		if (curStage == FRAME_RENDER_START)
		{
			Handler.SetAction("gMisc.ShowAngles()");
			if (gAutist.thirdperson)
			{
				if (!gAutist.realangles)
				{
					if (gAutist.lastAngles.x > 90)
						gAutist.lastAngles.x = 90;
					else if (gAutist.lastAngles.x < -90)
						gAutist.lastAngles.x = -90;
				}

				*MakePtr(Vector3*, you, netPlayer.deadflag + 0x4) = gAutist.lastAngles;
			}
		}
		else if (curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		{
			Handler.SetAction("HvH.Resolver()");

			if (gAutist.resolver)
				for (int i = 1; i <= gInts.Engine->GetMaxClients(); i++)
				{
					if (i == me)
						continue;
					CBaseEntity* current = GetBaseEntity(i);
					CPlayer* player;
					if (!current->ToPlayer(player) || player->Team() == you->Team())
						continue;

					if (gAutist.animations)
					{
						if (player->Conditions() & TFCond_Taunting)
							*MakePtr(int*, player, netPlayer.m_Shared + netPlayer.m_nPlayerCond) &= ~TFCond_Taunting;

						*MakePtr(bool*, player, netPlayer.m_bViewingCYOAPDA) = false;
					}

					Vector3 angles = *player->Viewangles();
					if (gAutist.pForce == "Auto")
					{
						if (angles.x >= 90)
							angles.x = -89;
						else if (angles.x <= -90)
							angles.x = 89;
					}
					else if (gAutist.pForce == "Up")
						angles.x = -89;
					else if (gAutist.pForce == "Down")
						angles.x = 89;
					else if (gAutist.pForce == "Center")
						angles.x = 0;

					if (gAutist.yForce != "None")
					{
						angles.y = fmod(angles.y + 180.0f, 360.0f);
						if (angles.y < 0) angles.y += 360.0f;
						angles.y -= 180.0f;

						if (gAutist.yForce == "180")
							angles.y = 180;
						else if (gAutist.yForce == "90")
							angles.y = 90;
						else if (gAutist.yForce == "0")
							angles.y = 0;
						else if (gAutist.yForce == "-90")
							angles.y = -90;
					}

					*player->Viewangles() = angles;
				}

			Handler.SetAction("Donator.SkinChanger()");
			if (!gMisc.skinChanger)
				return;

			hook.GetMethod<void(__thiscall *)(PVOID, ClientFrameStage_t)>(gOffsets.iFrameStageOffset)(gInts.Client, curStage);

			size_t* hMyWeapons = you->MyWeapons();
			int index = -1;
			for (int i = 0; hMyWeapons[i]; i++)
			{
				CCombatWeapon* pWeapon = (CCombatWeapon*)gInts.EntList->GetClientEntityFromHandle(hMyWeapons[i]);

				if (!pWeapon)
					continue;

				int* id = pWeapon->pItemID();
				index++;

				//if (index == 2 && gMisc.australium)
					//*id = 1071;

				CAttributeList* list = pWeapon->AttributeList();
				if (!list)
					continue;

				if (gMisc.australium)
				{
					list->RemoveAttribute(AT_is_festive);

					list->AddAttribute(AT_is_australium_item, true);
					list->AddAttribute(AT_loot_rarity, true);
					list->AddAttribute(AT_item_style_override, true);
				}

				if (gMisc.uwep != "None")
				{
					if (gMisc.uwep == "Hot")
						list->AddAttribute(AT_unusual_effect, WE_hot);
					else if (gMisc.uwep == "Cool")
						list->AddAttribute(AT_unusual_effect, WE_cool);
					else if (gMisc.uwep == "Energy orb")
						list->AddAttribute(AT_unusual_effect, WE_energyorb);
					else if (gMisc.uwep == "Isotope")
						list->AddAttribute(AT_unusual_effect, WE_isotope);
				}
				
				switch (*id)
				{
				case WPN_Scattergun:
					*id = WPN_NewScattergun; break;
				case WPN_ScoutPistol:
				case WPN_EngineerPistol:
					*id = WPN_NewPistol; break;
				case WPN_RocketLauncher:
					*id = WPN_NewRocketLauncher; break;
				case WPN_EngineerShotgun:
				case WPN_HeavyShotgun:
				case WPN_PyroShotgun:
				case WPN_SoldierShotgun:
					*id = WPN_NewShotgun; break;
				case WPN_Flamethrower:
					*id = WPN_NewFlamethrower; break;
				case WPN_GrenadeLauncher:
					*id = WPN_NewGrenadeLauncher; break;
				case WPN_StickyLauncher:
					*id = WPN_NewStickyLauncher; break;
				case WPN_Minigun:
					*id = WPN_NewMinigun; break;
				case WPN_SyringeGun:
					*id = WPN_NewSyringeGun; break;
				case WPN_Medigun:
					*id = WPN_NewMedigun; break;
				case WPN_SniperRifle:
					*id = WPN_NewSniperRifle; break;
				case WPN_SMG:
					*id = WPN_NewSMG; break;
				case WPN_Revolver:
					*id = WPN_NewRevolver;
				case WPN_Knife:
					*id = WPN_NewKnife;
				}

				tf_classes player = you->Class();
				if (player == TF2_Soldier && index == 0)
				{
					if (gMisc.rocket == "Festive")
						*id = WPN_FestiveRocketLauncher;
					else if (gMisc.rocket == "Sand cannon")
						*id = 15014;
					else if (gMisc.rocket == "Shell shocker")
						*id = 15052;
				}
				else if (player == TF2_Sniper && index == 0)
				{
					if (gMisc.rifle == "Festive")
						*id = WPN_FestiveSniperRifle;
					else if (gMisc.rifle == "AWP")
						*id = WPN_AWP;
					else if (gMisc.rifle == "Balloonicorn")
						*id = 15111;
					else if (gMisc.rifle == "Night owl")
						*id = 15000;
				}
			}

			CAttributeList* pAttribs = you->AttributeList();
			if (pAttribs && pAttribs->IsEmpty())
			{
				if (gMisc.utaunt != "None")
				{
					if (gMisc.utaunt == "Sparkle")
						pAttribs->AddAttribute(AT_particle_effect, HE_sparkle);
					else if (gMisc.utaunt == "Fire")
						pAttribs->AddAttribute(AT_particle_effect, HE_buringflames);
					else if (gMisc.utaunt == "Lanterns")
						pAttribs->AddAttribute(AT_particle_effect, HE_lanterns);
					else if (gMisc.utaunt == "Electricity")
						pAttribs->AddAttribute(AT_particle_effect, HE_terrorwat);
					else if (gMisc.utaunt == "Pixels")
						pAttribs->AddAttribute(AT_particle_effect, HE_memoryleak);
					else if (gMisc.utaunt == "Disco")
						pAttribs->AddAttribute(AT_particle_effect, UT_72);
					else if (gMisc.utaunt == "Lights")
						pAttribs->AddAttribute(AT_particle_effect, UT_holygrail);
					else if (gMisc.utaunt == "Fireworks")
					{
						byte team = you->Team();
						if (team == BLU_TEAM)
							pAttribs->AddAttribute(AT_particle_effect, UT_fireworks_blue);
						else if (team == RED_TEAM)
							pAttribs->AddAttribute(AT_particle_effect, UT_fireworks_red);
					}
				}
			}

			static string lastrifle = gMisc.rifle, lastrocket = gMisc.rocket, lastu = gMisc.utaunt, lastw = gMisc.uwep;
			static float lasta = gMisc.australium;
			if (lastrifle != gMisc.rifle || lastrocket != gMisc.rocket || lastu != gMisc.utaunt || lastw != gMisc.uwep || lasta != gMisc.australium)
			{
				typedef void(__thiscall *ForceFullUpdateFn)(CClientState *);
				static ForceFullUpdateFn forceFullUpdate = (ForceFullUpdateFn)gSignatures.GetEngineSignature("56 8B F1 83 BE ? ? ? ? ? 74 1D");
				forceFullUpdate(gInts.ClientState);

				lastrifle = gMisc.rifle, lastrocket = gMisc.rocket, lastu = gMisc.utaunt, lastw = gMisc.uwep, lasta = gMisc.australium;
			}
		}
	}
	catch (...)
	{
		MessageBox(NULL, Handler.GetError().c_str(), "ERROR", MB_OK);
	}

	hook.GetMethod<void(__thiscall *)(PVOID, ClientFrameStage_t)>(gOffsets.iFrameStageOffset)(gInts.Client, curStage);
}