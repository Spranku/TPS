// Fill out your copyright notice in the Description page of Project Settings.


#include "TPS_IGameActor.h"

// Add default functionality here for any ITPS_IGameActor functions that are not pure virtual.

EPhysicalSurface ITPS_IGameActor::GetSurfaceType()
{
	return EPhysicalSurface::SurfaceType_Default;
}

TArray<UTPS_StateEffect*> ITPS_IGameActor::GetAllCurrentEffects()
{
	TArray<UTPS_StateEffect*> Effect;

	return Effect;
}

//void ITPS_IGameActor::RemoveEffect(UTPS_StateEffect* RemoveEffect)
//{
//}
//
//void ITPS_IGameActor::AddEffect(UTPS_StateEffect* newEffect)
//{
//}
