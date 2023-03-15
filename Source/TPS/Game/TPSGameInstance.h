// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "/My_Projects/TPS/Source/TPS/FunctionLibrary/Type.h"
#include "Engine/DataTable.h"
#include "Game/WeaponDefault.h"
#include "TPSGameInstance.generated.h"


/**
 *  ƒанный класс будет использоватьс€, чтобы брать какую-либо информацию
 *  из таблиц
 *  ќн сохран€ет успех между уровн€ми
 */
UCLASS()
class TPS_API UTPSGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// ѕеременна€, из которой будем получать нужное оружие 
	// и брать информацию по этому оружию
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " WeaponSetting ")
		UDataTable* WeaponInfoTable = nullptr;
	UFUNCTION(BlueprintCallable)
		bool GetWeaponInfoByName(FName NameWeapon, FWeaponInfo& OutInfo);
};
