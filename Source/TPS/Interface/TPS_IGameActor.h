// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include <TPS/Public/Game/TPS_StateEffect.h>
#include "TPS_IGameActor.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTPS_IGameActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TPS_API ITPS_IGameActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/*UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Event")
		void AviableEffects_BP();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Event")
		bool AviableForEffects();*/

	virtual EPhysicalSurface GetSurfaceType();

	// Ф-я возвращает массив эффектов того или иного объекта
	virtual TArray<UTPS_StateEffect*> GetAllCurrentEffects();
	// Ф-я удаления эффекта
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void RemoveEffect(UTPS_StateEffect* RemoveEffect);
	// Ф-я добавления эффекта
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void AddEffect(UTPS_StateEffect* newEffect);
};
