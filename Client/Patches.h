#pragma once
#include "UE.h"
#include "client.h"

class Patches {
public:
	UObject* GAL_Func = nullptr;
	FVector* GetLocation(UObject* Target) {
		FVector Ret;
		if (GAL_Func == nullptr) {
			GAL_Func = FindObject("Function /Script/Engine.Actor.K2_GetActorLocation");
		}
		ProcessEventOG(Target, GAL_Func, &Ret);
		return new FVector(Ret);
	}
};
