// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "/My_Projects/TPS/Source/TPS/FunctionLibrary/Type.h"
#include "TPSInventoryComponent.generated.h"

// Делегат, сообщающий что оружие начало меняться
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSwitchWeapon,
	FName,
	WeaponIdName,
	FAdditionalWeaponInfo,
	WeaponAdditionalInfo,
	int32,
	NewCurrentIndexWeapon);
// Делегат, сообщающий что перезарядка прошла успешно
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChange,
	EWeaponType,
	TypeAmmo,
	int32,
	Cout);
// Делегат,сообщающий что делаем перезарядку
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponAdditionalInfoChange,
	int32,
	IndexSlot,
	FAdditionalWeaponInfo,
	AdditionalInfo);
// Делегат, сообщающий что патроны для оружия закончились
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoEmpty,
	EWeaponType,
	WeaponType);

// Делегат, сообщающий что сменили оружие
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoAviable, 
	EWeaponType, 
	WeaponType);

// Делегат, сообщающий что произошли изменения в слотах оружия
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
	// Переменная делегата OnSwitchWeapon сообщает, что оружие начало меняться
	FOnSwitchWeapon OnSwitchWeapon;
	// Переменна делегата, сообщает что перезарядка прошла успешно
	UPROPERTY(BlueprintAssignable,EditAnywhere,BlueprintReadWrite,Category = "Inventory")
		FOnAmmoChange OnAmmoChange;
	//Переменная делегата, сообщает, что делаем перезарядку
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FOnWeaponAdditionalInfoChange OnWeaponAdditionalInfoChange;
	// Переменная делегата, сообщает, что патроны для оружия закончились
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

	// Два важных массива для отображения в BP. Слоты оружия и типа патронов.
	// Всё описано в Type файле
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
		TArray<FWeaponSlot> WeaponSlots;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
		TArray<FAmmoSlot> AmmoSlots;
	// Информация для виджетов (ограничение на слоты)
		int32 MaxSlotsWeapon = 0;

	// Основная функция, которая пригодится в будущем. Аргументы: другой индекс, старый индекс, старая инфа
	// OldInfo - патроны в текущем оружии
	 bool SwitchWeaponToIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo, bool bIsForward);

	// Дает инфо сколько патронов осталось по индексу
	FAdditionalWeaponInfo GetAdditionalInfoWeapon(int32 IndexWeapon);

	// Можно спросить у инвентаря: есть текущее оружие с таким именем, какой у него индекс? 
	int32 GetWeaponIndexSlotByName(FName IdWeaponName);

	// Ф-я которая вернет имя оружия по слоту
	FName GetWeaponNameBySlotIndex(int32 IndexSlot);

	// Записывает инфо по индексу
	void SetAdditionalInfoWeapon(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo);
	
	// Тип оружия и AmmoTaken - кол-во патронов, которые мы отобрали
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
	UFUNCTION(BlueprintCallable, Category = "Interface")
		bool TryGetWeaponToInventory(FWeaponSlot NewWeapon);
	UFUNCTION(BlueprintCallable, Category = "Interface")
		bool GetDropItemInfoFromInventory(int32 IndexSlot, FDropItem &DropItemInfo);


};
