// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include </My_Projects/TPS/Source/TPS/Character/TPSHealthComponent.h>
#include "TPSCharacterHealthComponent.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API UTPSCharacterHealthComponent : public UTPSHealthComponent
{
	GENERATED_BODY()
	
public:

	void ChangeCurrentHealth(float ChangeValue) override;
};
