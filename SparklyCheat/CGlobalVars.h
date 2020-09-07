#include "SDK.h"

class EntityVars
{
public:
	void find();

	DWORD m_vecOrigin;
	DWORD m_angRotation;
	DWORD m_iTeamNum;
	DWORD m_hOwnerEntity;
};

class PlayerVars
{
public:
	void find();

	DWORD m_lifeState;
	DWORD m_fFlags;
	DWORD m_iHealth;
	DWORD m_iMaxHealth;
	DWORD m_iTeamNum;
	DWORD m_iClass;
	DWORD m_nPlayerCond;
	DWORD m_Shared;
	DWORD m_Collision;
	DWORD m_hActiveWeapon;
	DWORD m_iWeaponState;
	DWORD m_vecViewOffset;
	DWORD m_vecOrigin;
	DWORD m_vecVelocity;
	DWORD m_nForceTauntCam;
	DWORD deadflag;
	DWORD m_bReadyToBackstab;

	DWORD m_iFOV;
	DWORD m_bDrawViewmodel;
	DWORD m_hMyWeapons;
	DWORD m_hMyWearables;
	DWORD m_angEyeAngles;
	DWORD m_nTickBase;
	DWORD m_flFOVTime;
	DWORD m_flFOVRate;
	DWORD m_hObserverTarget;
	DWORD m_iDamageDone;
	DWORD m_AttributeList;

	DWORD m_bViewingCYOAPDA;
};

class WeaponVars
{
public:
	void find();

	DWORD m_iItemDefinitionIndex;
	DWORD m_AttributeManager;
	DWORD m_flObservedCritChance;
	DWORD m_AttributeList;
	DWORD m_flNextPrimaryAttack;
};

class ObjectVars
{
public:
	void find();

	DWORD m_iHealth;
	DWORD m_hBuilder;
	DWORD m_iMaxHealth;
	DWORD m_bBuilding;
	DWORD m_iUpgradeLevel;
	DWORD m_iUpgradeMetal;
	DWORD m_iUpgradeMetalRequired;
};

extern EntityVars netEntity;
extern PlayerVars netPlayer;
extern WeaponVars netWeapon;
extern ObjectVars netObject;