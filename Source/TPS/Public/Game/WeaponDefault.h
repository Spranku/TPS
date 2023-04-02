// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "/My_Projects/TPS/Source/TPS/FunctionLibrary/Type.h"
#include "Components/ArrowComponent.h"
#include </My_Projects/TPS/Source/TPS/Public/Game/ProjectileDefault.h>
#include "WeaponDefault.generated.h"

// Делегаты для перезарядки
// Макрос DECLARE_DYNAMIC_MULTICAST_DELEGATE
// В аргументы: Имя делегата, ССЫЛКУ на аним.монтаж и его название
// Ниже,в паблик поле класса эти переменные тоже надо объявить
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponReloadStart, UAnimMontage*, Anim);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponReloadEnd);


UCLASS()
class TPS_API AWeaponDefault : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponDefault();
	// Объявление переменных делегата
	FOnWeaponReloadStart OnWeaponReloadStart;
	FOnWeaponReloadEnd OnWeaponReloadEnd;

	// Переменные,которые создаются и заносятся как компоненты в BP
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class USceneComponent* SceneComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class USkeletalMeshComponent* SkeletalMeshWeapon = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UStaticMeshComponent* StaticMeshWeapon = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UArrowComponent* ShootLocation = nullptr;

	// Структура FWeaponInfo,которая описана в Types
	UPROPERTY()
	FWeaponInfo WeaponSetting;
	// Еще одна структура. Информация в ней будет меняться
	// Там будет храниться кол-во патронов в оружии
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponInfo")
	FAddicionalWeaponInfo WeaponInfo;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void FireTick(float DeltaTime);
	// Тик перезарядки
	void ReloadTick(float DeltaTime);

	// Тик разброса
	void DispersionTick(float DeltaTime);

	// Ф-я которая инициализирует всё оружие
	void WeaponInit();

	// Булевая переменная, отвечающая за возможность стрельбы
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireLogic")
		bool WeaponFiring = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReloadLogic")
		bool WeaponReloading = false; // Флаг на перезарядку

	UFUNCTION(BlueprintCallable)
	void SetWeaponStateFire(bool bIsFire);

	bool CheckWeaponCanFire();

	FProjectileInfo GetProjectile();

	void Fire();

	void UpdateStateWeapon(EMovementState NewMovementState);
	void ChangeDispersion();
	//
	void ChangeDispersionByShoot();
	float GetCurrentDispersion() const;
	FVector ApplyDispersionToShoot(FVector DirectionShoot)const;
	FVector GetFireEndLocation()const;
	// Функция для дробовика
	int8 GetNumberProjectileByShot() const;

	UFUNCTION(BlueprintCallable)
	int32 GetWeaponRound();

	// Инициализация перезарядки
	void InitReload();
	// Конец перезарядки
	void FinishReload();

	//Timers'flags
	float FireTime = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReloadLogic")
	float ReloadTimer = 0.0f;

	bool BlockFire = false;
	// Разброс
	bool ShouldReduseDespersion = false;
	float CurrentDispersion = 0.0f;
	float CurrentDispersionMax = 1.0f;
	float CurrentDispersionMin = 0.1f;
	float CurrentDispersionRecoil = 0.1f;
	float CurrentDispersionReduction = 0.1f;

	FVector ShootEndLocation = FVector(0);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		bool  ShowDebug = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	    float SizeVectorToChangeShootDirectionLogic = 100.0f;;
};
