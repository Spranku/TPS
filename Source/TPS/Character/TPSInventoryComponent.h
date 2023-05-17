// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "/My_Projects/TPS/Source/TPS/FunctionLibrary/Type.h"
#include "TPSInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSwitchWeapon,
	FName,
	WeaponIdName,
	FAdditionalWeaponInfo,
	WeaponAdditionalInfo);
// �������, ���������� ��� ����������� ������ �������
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChange,
	EWeaponType,
	TypeAmmo,
	int32,
	Cout);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_API UTPSInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTPSInventoryComponent();
	// ���������� �������� OnSwitchWeapon ��������, ��� ������ ������ ��������
	FOnSwitchWeapon OnSwitchWeapon;
	// ��������� ��������, �������� ��� ����������� ������ �������
	UPROPERTY(BlueprintAssignable,EditAnywhere,BlueprintReadWrite,Category = "Inventory")
	FOnAmmoChange OnAmmoChange;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ��� ������ ������� ��� ����������� � BP. ����� ������ � ���� ��������.
	// �� ������� � Type �����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
		TArray<FWeaponSlot> WeaponSlots;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
		TArray<FAmmoSlot> AmmoSlots;
	// ���������� ��� �������� (����������� �� �����)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
		int32 MaxSlotsWeapon = 0;

	// �������� �������, ������� ���������� � �������. ���������: ������ ������, ������ ������, ������ ����
	// OldInfo - ������� � ������� ������
	 bool SwitchWeaponToIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo);

	// ���� ���� ������� �������� �������� �� �������
	FAdditionalWeaponInfo GetAdditionalInfoWeapon(int32 IndexWeapon);

	// ����� �������� � ���������: ���� ������� ������ � ����� ������, ����� � ���� ������? 
	int32 GetWeaponIndexSlotByName(FName IdWeaponName);

	// ���������� ���� �� �������
	void SetAdditionalInfoWeapon(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo);
	// ��� ������ � AmmoTaken - ���-�� ��������, ������� �� ��������
	void WeaponChangeAmmo(EWeaponType TypeWeapon, int32 AmmoTaken);
};
