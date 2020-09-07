#include "SDK.h"
#include "Netvar.h"

EntityVars netEntity;
PlayerVars netPlayer;
WeaponVars netWeapon;
ObjectVars netObject;

int getOffset(char *szClassName, char *szVariable) // i dont care enough to add a proper netvar manager
{
	ClientClass *pClass = gInts.Client->GetAllClasses();

	for (; pClass; pClass = pClass->pNextClass)
	{
		RecvTable *pTable = pClass->Table;

		if (pTable->GetNumProps() <= 1) continue;

		for (int i = 0; i < pTable->GetNumProps(); i++)
		{
			RecvProp *pProp = pTable->GetProp(i);

			if (!pProp) continue;

			if (streql(pTable->GetName(), szClassName) && streql(pProp->GetName(), szVariable))
			{
				return pProp->GetOffset();
			}
			if (pProp->GetDataTable())
			{
				RecvTable *pTable = pProp->GetDataTable();
				for (int i = 0; i < pTable->GetNumProps(); i++)
				{
					RecvProp *pProp = pTable->GetProp(i);

					if (!pProp) continue;

					if (streql(pTable->GetName(), szClassName) && streql(pProp->GetName(), szVariable))
					{
						return pProp->GetOffset();
					}
				}
			}
		}
	}
	return 0;
}

void EntityVars::find()
{
	netvar_tree nv;
	m_vecOrigin = nv.get_offset("DT_BaseEntity", "m_vecOrigin");
	m_angRotation = nv.get_offset("DT_BaseEntity", "m_angRotation");
	m_iTeamNum = nv.get_offset("DT_BaseEntity", "m_iTeamNum");
	m_hOwnerEntity = nv.get_offset("DT_BaseEntity", "m_hOwnerEntity");
}

void PlayerVars::find()
{
	netvar_tree nv;
	m_lifeState = getOffset("DT_BasePlayer", "m_lifeState");
	m_iHealth = getOffset("DT_BasePlayer", "m_iHealth");
	m_iMaxHealth = getOffset("DT_BasePlayer", "m_iMaxHealth");
	m_fFlags = getOffset("DT_BasePlayer", "m_fFlags");
	m_iClass = getOffset("DT_TFPlayer", "m_PlayerClass") + 0x4;
	m_iTeamNum = getOffset("DT_BaseEntity", "m_iTeamNum");

	m_Shared = getOffset("DT_TFPlayer", "m_Shared");
	m_nPlayerCond = getOffset("DT_TFPlayerShared", "m_nPlayerCond");

	m_Collision = getOffset("DT_BaseEntity", "m_Collision");

	m_hActiveWeapon = getOffset("DT_BaseCombatCharacter", "m_hActiveWeapon");
	m_iWeaponState = getOffset("DT_WeaponMinigun", "m_iWeaponState");

	m_vecViewOffset = getOffset("DT_LocalPlayerExclusive", "m_vecViewOffset[0]");
	m_vecOrigin = getOffset("DT_BaseEntity", "m_vecOrigin");
	m_vecVelocity = getOffset("DT_BaseEntity", "m_vecVelocity[0]");

	m_nForceTauntCam = getOffset("DT_TFPlayer", "m_nForceTauntCam");

	deadflag = nv.get_offset("DT_BasePlayer", "pl", "deadflag");

	m_bReadyToBackstab = getOffset("DT_TFWeaponKnife", "m_bReadyToBackstab");

	m_iFOV = getOffset("DT_BasePlayer", "m_iFOV");
	m_hMyWeapons = getOffset("DT_BaseCombatCharacter", "m_hMyWeapons");
	m_hMyWearables = nv.get_offset("DT_BasePlayer", "m_hMyWearables");
	m_angEyeAngles = nv.get_offset("DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[0]");
	m_bDrawViewmodel = 0xF04; //nv.get_offset("DT_BasePlayer", "localdata", "m_bDrawViewmodel");
	m_nTickBase = nv.get_offset("DT_BasePlayer", "localdata", "m_nTickBase");
	m_flFOVTime = nv.get_offset("DT_BasePlayer", "m_flFOVTime");
	m_flFOVRate = 0xE5C; //nv.get_offset("DT_BasePlayer", "localdata", "m_flFOVRate");
	m_hObserverTarget = nv.get_offset("DT_BasePlayer", "m_hObserverTarget");
	m_AttributeList = nv.get_offset("DT_BasePlayer", "m_AttributeList");

	m_bViewingCYOAPDA = nv.get_offset("DT_TFPlayer", "m_bViewingCYOAPDA");
}

void WeaponVars::find()
{
	netvar_tree nv;
	m_AttributeManager = nv.get_offset("DT_EconEntity", "m_AttributeManager");
	m_iItemDefinitionIndex = nv.get_offset("DT_EconEntity", "m_AttributeManager", "m_Item", "m_iItemDefinitionIndex");
	m_AttributeList = nv.get_offset("DT_EconEntity", "m_AttributeManager", "m_Item", "m_AttributeList");
	m_flObservedCritChance = nv.get_offset("DT_TFWeaponBase", "LocalActiveTFWeaponData", "m_flObservedCritChance");
	m_flNextPrimaryAttack = nv.get_offset("DT_BaseCombatWeapon", "LocalActiveWeaponData", "m_flNextPrimaryAttack");
}

void ObjectVars::find()
{
	m_iHealth = getOffset("DT_BaseObject", "m_iHealth");
	m_hBuilder = getOffset("DT_BaseObject", "m_hBuilder");
	m_iMaxHealth = getOffset("DT_BaseObject", "m_iMaxHealth");

	m_bBuilding = getOffset("DT_BaseObject", "m_bBuilding");
	m_iUpgradeLevel = getOffset("DT_BaseObject", "m_iUpgradeLevel");
	m_iUpgradeMetal = getOffset("DT_BaseObject", "m_iUpgradeMetal");
	m_iUpgradeMetalRequired = getOffset("DT_BaseObject", "m_iUpgradeMetalRequired");
}