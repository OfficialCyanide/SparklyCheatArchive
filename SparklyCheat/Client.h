#pragma once

#include "SDK.h"

bool __fastcall Hooked_CreateMove(PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd* pCommand);
void __fastcall Hooked_CHLCreateMove(void* ecx, void* edx, int sequence_number, float input_sample_time, bool active);