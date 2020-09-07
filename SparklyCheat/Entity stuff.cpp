#include "Entity stuff.h"
#include "Entity.h"

EntityStuff gEntStuff;

Vector3 EntityStuff::EstimateAbsVelocity(CBaseEntity *ent)
{
	typedef void(__thiscall * EstimateAbsVelocityFn)(CBaseEntity *, Vector3 &);

	static DWORD dwFn = gSignatures.GetClientSignature("E8 ? ? ? ? F3 0F 10 4D ? 8D 85 ? ? ? ? F3 0F 10 45 ? F3 0F 59 C9 56 F3 0F 59 C0 F3 0F 58 C8 0F 2F 0D ? ? ? ? 76 07") + 0x1;

	static DWORD dwEstimate = ((*(PDWORD)(dwFn)) + dwFn + 4);

	EstimateAbsVelocityFn vel = (EstimateAbsVelocityFn)dwEstimate;

	Vector3 v;
	vel(ent, v);

	return v;
}

CBaseEntity* EntityStuff::gActiveWeapon(CBaseEntity* player)
{
	DWORD hWeapon = *reinterpret_cast<DWORD*>(player + netPlayer.m_hActiveWeapon);
	hWeapon &= 0xFFF;
	CBaseEntity *pWep = GetBaseEntity(hWeapon);
	
	return pWep;
}