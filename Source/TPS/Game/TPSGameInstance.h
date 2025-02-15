// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "/My_Projects/TPS/Source/TPS/FunctionLibrary/Type.h"
#include "Engine/DataTable.h"
#include "Game/WeaponDefault.h"
#include "TPSGameInstance.generated.h"


/**
 *  ������ ����� ����� ��������������, ����� ����� �����-���� ����������
 *  �� ������
 *  �� ��������� ����� ����� ��������
 */
UCLASS()
class TPS_API UTPSGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// ����������, �� ������� ����� �������� ������ ������ 
	// � ����� ���������� �� ����� ������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " WeaponSetting ")
		UDataTable* WeaponInfoTable = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " WeaponSetting ")
		UDataTable* DropItemInfoTable = nullptr;
	UFUNCTION(BlueprintCallable)
		bool GetWeaponInfoByName(FName NameWeapon, FWeaponInfo& OutInfo);
	UFUNCTION(BlueprintCallable) // ������ & - ��������� �� �������� ������ � BP
		bool GetDropItemInfoByWeaponName(FName NameItem, FDropItem& OutInfo);
	UFUNCTION(BlueprintCallable)
		bool GetDropItemInfoByName(FName NameItem, FDropItem& OutInfo);
};
