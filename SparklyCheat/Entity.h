#pragma once
#include "SDK.h"
#include "studio.h"
#include "Entity ID.h"

class CPlayer; // Pre-defining for use in CBaseEntity;

#define me gInts.Engine->GetLocalPlayer()
#define GetBaseEntity gInts.EntList->GetClientEntity

class CBaseEntity
{
public:
	// - Gets the origin position (usually the bottom) of the entity
	Vector3& GetAbsOrigin()
	{
		typedef Vector3& (__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 9)(this);
	}
	Vector3& GetAbsAngles()
	{
		typedef Vector3& (__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 10)(this);
	}
	// - Gets the center position of the entity
	Vector3 GetWorldCenter()
	{
		Vector3 vMin, vMax, output = this->GetAbsOrigin();
		this->GetRenderBounds(vMin, vMax);
		output.z += (vMin.z + vMax.z) / 2;

		return output;
	}
	DWORD* GetModel()
	{
		PVOID pRenderable = (PVOID)(this + 0x4);
		typedef DWORD* (__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pRenderable, 9)(pRenderable);
	}
	bool SetupBones(matrix3x4 *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
	{
		PVOID pRenderable = (PVOID)(this + 0x4);
		typedef bool(__thiscall* OriginalFn)(PVOID, matrix3x4*, int, int, float);
		return getvfunc<OriginalFn>(pRenderable, 16)(pRenderable, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
	}
	ClientClass* GetClientClass()
	{
		PVOID pNetworkable = (PVOID)(this + 0x8);
		typedef ClientClass* (__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pNetworkable, 2)(pNetworkable);
	}
	bool IsDormant()
	{
		PVOID pNetworkable = (PVOID)(this + 0x8);
		typedef bool(__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pNetworkable, 8)(pNetworkable);
	}
	int GetIndex()
	{
		PVOID pNetworkable = (PVOID)(this + 0x8);
		typedef int(__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pNetworkable, 9)(pNetworkable);
	}
	void GetRenderBounds(Vector3& mins, Vector3& maxs)
	{
		PVOID pRenderable = (PVOID)(this + 0x4);
		typedef void(__thiscall* OriginalFn)(PVOID, Vector3&, Vector3&);
		getvfunc<OriginalFn>(pRenderable, 20)(pRenderable, mins, maxs);
	}
	// - Gets the entity type
	ent_id Type()
	{
		return (ent_id)this->GetClientClass()->iClassID;
	}
	// - Gets the entity's team
	// - Ex: if (entity->Team() == 2) string teamName = "BLU";
	byte Team()
	{
		return *(byte*)(this + netPlayer.m_iTeamNum);
	}
	// - Returns true if this entity is a player,
	//   and sets it to the specified player pointer
	bool ToPlayer(CPlayer* &player)
	{
		if (this != nullptr && this->Type() == ent_id::CTFPlayer)
		{
			player = (CPlayer*)this;
			return true;
		}
		return false;
	}
};

class CAttributeList;
class CCombatWeapon : public CBaseEntity
{
public:
	// - Gets the weapon's type/model ID
	int ItemID()
	{
		return *pItemID();
	}
	// - Gets a pointer to the weapon's type/model ID
	int* pItemID()
	{
		return (int*)(this + netWeapon.m_iItemDefinitionIndex);
	}
	CAttributeList* AttributeList()
	{
		return (CAttributeList*)(this + netWeapon.m_AttributeList);
	}
};

class CPlayer : public CBaseEntity
{
public:
	// - Gets the player's health
	int Health()
	{
		return *(int*)(this + netPlayer.m_iHealth);
	}
	// - Gets the player's max health
	int MaxHealth()
	{
		typedef int(__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 107)(this);
	}
	// - Gets the player's flags
	// - Ex: player->Flags() & FL_ONGROUND
	int Flags()
	{
		return *(int*)(this + netPlayer.m_fFlags);
	}
	// - Gets the player's conditions
	// - Ex: player->Conditions() & tf_cond::zoomed
	int Conditions()
	{
		return *(int*)(this + netPlayer.m_Shared + netPlayer.m_nPlayerCond);
	}
	// - Gets the player's FOV time
	float FovTime()
	{
		return *(float*)(this + netPlayer.m_flFOVTime);
	}
	// - Gets the player's active weapon
	CCombatWeapon* ActiveWeapon()
	{
		return (CCombatWeapon*)gInts.EntList->GetClientEntityFromHandle(*(size_t*)(this + netPlayer.m_hActiveWeapon));
	}
	// - Gets the player's spectate target
	CPlayer* ObserverTarget()
	{
		return (CPlayer*)gInts.EntList->GetClientEntityFromHandle(*(size_t*)(this + netPlayer.m_hObserverTarget));
	}
	// - Gets an entity array of the weapons you own
	size_t* MyWeapons()
	{
		return (size_t*)(this + netPlayer.m_hMyWeapons);
	}
	size_t* MyWearables()
	{
		return (size_t*)(this + netPlayer.m_hMyWearables);
	}
	// - Gets your weapon selection index
	int WeaponSelection()
	{
		size_t* hWeapons = this->MyWeapons();
		int a = 0;
		for (int i = 0; hWeapons[i]; i++)
		{
			CCombatWeapon* pWeapon = (CCombatWeapon*)gInts.EntList->GetClientEntityFromHandle(hWeapons[i]);
			if (!pWeapon)
				continue;

			CCombatWeapon* cWep = this->ActiveWeapon();
			if (pWeapon == cWep)
				return a;

			a++;
		}
	}
	// - Gets the player's attribute list
	CAttributeList* AttributeList()
	{
		return (CAttributeList*)(this + netPlayer.m_AttributeList);
	}
	// - Gets the player's class
	tf_classes Class()
	{
		return (tf_classes) *(int*)(this + netPlayer.m_iClass);
	}
	// - Gets the player's state of living
	source_lifestates Lifestate()
	{
		return (source_lifestates) *(byte*)(this + netPlayer.m_lifeState);
	}
	// - Gets the player's info
	// - Ex: string name = player->Info().name
	player_info_s Info()
	{
		player_info_s pInfo;
		gInts.Engine->GetPlayerInfo(GetIndex(), &pInfo);
		return pInfo;
	}
	// - Gets the player's first person camera position
	Vector3 EyePos()
	{
		Vector3 localOrigin = *reinterpret_cast<Vector3*>(this + netPlayer.m_vecOrigin);
		return *reinterpret_cast<Vector3*>(this + netPlayer.m_vecViewOffset) + localOrigin;
	}
	// - Gets a pointer to the player's viewangles
	Vector3* Viewangles()
	{
		return (Vector3*)(this + netPlayer.m_angEyeAngles);
	}
	// - Gets the center position of i hitbox
	// - i range: Min = 0, Max = 17
	Vector3 HitboxPos(int i)
	{
		if (i < 0 || i > 17) i = 0; // Quick sanity check

		DWORD *model = this->GetModel();
		if (!model)
			return Vector3();

		studiohdr_t *hdr = gInts.ModelInfo->GetStudiomodel(model);
		if (!hdr)
			return Vector3();

		matrix3x4 matrix[128];
		if (!this->SetupBones(matrix, 128, 0x100, 0))
			return Vector3();

		int HitboxSetIndex = *(int *)((DWORD)hdr + 0xB0);
		if (!HitboxSetIndex)
			return Vector3();

		mstudiohitboxset_t *pSet = (mstudiohitboxset_t *)(((PBYTE)hdr) + HitboxSetIndex);
		mstudiobbox_t* box = pSet->pHitbox(i);
		if (!box)
			return Vector3();

		Vector3 vCenter = (box->bbmin + box->bbmax) * 0.5f;

		Vector3 vHitbox;
		VectorTransform(vCenter, matrix[box->bone], vHitbox);

		return vHitbox;
	}
};