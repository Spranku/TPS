// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "/My_Projects/TPS/Source/TPS/FunctionLibrary/Type.h"
#include "TPSInventoryComponent.generated.h"

// �������, ���������� ��� ������ ������ ��������
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSwitchWeapon,
	FName,
	WeaponIdName,
	FAdditionalWeaponInfo,
	WeaponAdditionalInfo,
	int32,
	NewCurrentIndexWeapon);
// �������, ���������� ��� ����������� ������ �������
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChange,
	EWeaponType,
	TypeAmmo,
	int32,
	Cout);
// �������,���������� ��� ������ �����������
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponAdditionalInfoChange,
	int32,
	IndexSlot,
	FAdditionalWeaponInfo,
	AdditionalInfo);
// �������, ���������� ��� ������� ��� ������ �����������
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoEmpty,
	EWeaponType,
	WeaponType);

// �������, ���������� ��� ������� ������
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoAviable, 
	EWeaponType, 
	WeaponType);

// �������, ���������� ��� ��������� ��������� � ������ ������
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateWeaponSlots, 
	int32,
	IndexSlotChange,
	FWeaponSlot,
	NewInfo);

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
	//���������� ��������, ��������, ��� ������ �����������
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FOnWeaponAdditionalInfoChange OnWeaponAdditionalInfoChange;
	// ���������� ��������, ��������, ��� ������� ��� ������ �����������
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FOnWeaponAmmoEmpty OnWeaponAmmoEmpty;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FOnWeaponAmmoAviable OnWeaponAmmoAviable;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FOnUpdateWeaponSlots OnUpdateWeaponSlots;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ��� ������ ������� ��� ����������� � BP. ����� ������ � ���� ��������.
	// �� ������� � Type �����
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Weapons")
		TArray<FWeaponSlot> WeaponSlots;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Weapons")
		TArray<FAmmoSlot> AmmoSlots;
	// ���������� ��� �������� (����������� �� �����)
		int32 MaxSlotsWeapon = 0;

	// �������� �������, ������� ���������� � �������. ���������: ������ ������, ������ ������, ������ ����
	// OldInfo - ������� � ������� ������
	 bool SwitchWeaponToIndexByNextPreviosIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo, bool bIsForward);

	 bool SwitchWeaponByIndex(int32 IndexWeaponToChange, int32 PreviosIndex, FAdditionalWeaponInfo PreviosWeaponInfo);

	// ���� ���� ������� �������� �������� �� �������
	FAdditionalWeaponInfo GetAdditionalInfoWeapon(int32 IndexWeapon);

	// ����� �������� � ���������: ���� ������� ������ � ����� ������, ����� � ���� ������? 
	int32 GetWeaponIndexSlotByName(FName IdWeaponName);
	// ��������������� �-��� ����������� ����� ������������
	bool GetWeaponTypeByIndexSlot(int32 IndexSlot, EWeaponType& WeaponType);
	bool GetWeaponTypeByNameWeapon(FName IdWeaponName, EWeaponType& WeaponType);
	// �-� ������� ������ ��� ������ �� �����
	FName GetWeaponNameBySlotIndex(int32 IndexSlot);
	// ���������� ���� �� �������
	void SetAdditionalInfoWeapon(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo);
	// ��� ������ � AmmoTaken - ���-�� ��������, ������� �� ��������
	UFUNCTION(BlueprintCallable)
	void AmmoSlotChangeValue(EWeaponType TypeWeapon, int32 CoutChangeAmmo);

	bool CheckAmmoForWeapon(EWeaponType TypeWeapon, int8 &AviableAmmoForWeapon);
	
	// Interface PickUp Actors
	UFUNCTION(BlueprintCallable,Category = "Interface")
		bool CheckCanTakeAmmo(EWeaponType AmmoType);
	UFUNCTION(BlueprintCallable, Category = "Interface")
		bool CheckCanTakeWeapon(int32 &FreeSlot);
	UFUNCTION(BlueprintCallable, Category = "Interface")
		bool SwitchWeaponToInventory(FWeaponSlot NewWeapon,int32 IndexSlot,int32 CurrentIndexWeaponChar,FDropItem &DropItemInfo);
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Interface")
		void TryGetWeaponToInventory_OnServer(AActor* PickUpActor, FWeaponSlot NewWeapon);
	UFUNCTION(BlueprintCallable, Category = "Interface")
		bool GetDropItemInfoFromInventory(int32 IndexSlot, FDropItem &DropItemInfo);

	UFUNCTION(BlueprintCallable, Category = "Inv")
		TArray<FWeaponSlot> GetWeaponSlots();
	UFUNCTION(BlueprintCallable, Category = "Inv")
		TArray<FAmmoSlot> GetAmmoSlots();

	//NetWork
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Inv")
		void InitInventory_OnServer(const TArray<FWeaponSlot>& NewWeaponSlotsInfo, const TArray<FAmmoSlot>& NewAmmoSlotsInfo);
	UFUNCTION(NetMulticast, Reliable)
		void AmmoChangeEvent_Multicast(EWeaponType TypeWeapon, int32 Cout);

	UFUNCTION(Server, Reliable)
		void SwitchWeaponEvent_OnServer(FName Name, FAdditionalWeaponInfo Info, int32 Index);
	UFUNCTION(NetMulticast, Reliable)
		void ReloadEvent_Multicast(int32 Index, FAdditionalWeaponInfo Info);
	UFUNCTION(NetMulticast, Reliable)
		void AmmoEmptyEvent_Multicast(EWeaponType WeaponType);
	UFUNCTION(NetMulticast,  Reliable)
		void AmmoAviable_Multicast(EWeaponType WeaponType);
	UFUNCTION(NetMulticast,  Reliable)
		void UpdateWeaponSlots_Multicast(int32 Index, FWeaponSlot Weapon);
};
