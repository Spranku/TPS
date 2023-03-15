// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "/My_Projects/TPS/Source/TPS/FunctionLibrary/Type.h"
#include "Components/ArrowComponent.h"
#include </My_Projects/TPS/Source/TPS/Public/Game/ProjectileDefault.h>
#include "WeaponDefault.generated.h"


UCLASS()
class TPS_API AWeaponDefault : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponDefault();

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

	// Ф-я которая инициализирует всё оружие
	void WeaponInit();

	// Булевая переменная, отвечающая за возможность стрельбы
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireLogic")
		bool WeaponFiring = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireLogic")
		bool WeaponReloading = false; // Флаг на перезарядку

	UFUNCTION(BlueprintCallable)
	void SetWeaponStateFire(bool bIsFire);

	bool CheckWeaponCanFire();

	FProjectileInfo GetProjectile();

	void Fire();

	void UpdateStateWeapon(EMovementState NewMovementState);
	void ChangeDispersion();

	//Timers'flags
	float FireTime = 0.0;

};
