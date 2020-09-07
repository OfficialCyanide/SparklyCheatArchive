#pragma once
#include "Entity.h"
#include <vector>

class hitscan
{
public:
	//bool shouldTarget(CBaseEntity* entity);
	bool bestHit(CBaseEntity* target, Vector3 &hitPos);
	int getProjSpeed(CBaseEntity* weapon);
	float distTo(Vector3 start, Vector3 end);

	bool predVischeck(void* you, Vector3 vStart, Vector3 vEnd, trace_t *result = nullptr);
	bool canHit(void* you, void* target, Vector3 vStart, Vector3 vEnd, trace_t *result = nullptr);
private:
	vector<int> bestHitbox(string priority);
	Vector3 getPredPos(Vector3 startPos, CBaseEntity* target);
};
extern hitscan gHitscan;