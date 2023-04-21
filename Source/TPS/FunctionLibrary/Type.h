// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Kismet/BlueprintFunctionLibrary.h>
#include "Engine/DataTable.h"
#include "Type.generated.h"


// Eнумерация с доступом к BP
// Класс Eнумерации EMovementState 
// У Eнумерации обязательно нужно указать её длину.
// uint8 должно хватить. 
// В теле Eнумерации нужно прописать состояния движения.
// Первым будет Aim_State, далее синтаксис
// UMETA(DisplayName... - то,что отображается в BP,
// грубо говоря имя Eнумерации. Далее запятая,
// вторая инумерация Walk_State,третья Run_State.
// 
UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Aim_State UMETA(DisplayName = "Aim State"),
	AimWalk_State UMETA(DisplayeName = "Aim Walk State"),
	Walk_State UMETA(DisplayeName = "Walk State"),
	Run_State UMETA(DisplayeName = "Run State"),
	SprintRun_State UMETA(DisplayName = "Sprint Run Speed")
};

// Чтобы всё было структурировано, добавим структуру
// Структуры добавляются практически также как ENUM
// За исключением названия макроса.
// Сперва добавляется GENERATED_BODY()
// В структуре будет храниться float переменные
// AimSpeed (скорость в прицелиивании),
// WalkSpeed (скорость ходьбы (не обязательно),
// RunSpeed (скорость бега)
// Так же нужно добавить Unreal`овский макрос
// UPROPERTY(EditAnywhere, BlueprintReadWrite)
// Эта струкрура будет виидна в BP, её можно
// читать и изменять.
// 
USTRUCT(BlueprintType) 
struct FCharacterSpeed
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Movement")
	float AimSpeed = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AimWalkSpeed = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RunSpeed = 600.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintRunSpeed = 800.0f;
};

USTRUCT(BlueprintType)
struct FProjectileInfo
{
	GENERATED_BODY()
	// Здесь описан класс пули и её урон
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
	TSubclassOf<class AProjectileDefault> Projectile = nullptr;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		class UStaticMesh* ProjectileStaticMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		FTransform ProjectileStaticMeshOffset = FTransform();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		class UParticleSystem* ProjectileTrialFX = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		FTransform ProjectileTrialFxOffset = FTransform();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
	float ProjectileDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
		float ProjectileLifeTime = 30.0f; // Жизнь пули
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
		float ProjcetileInitSpeed = 2000.0f;       // Скорость пули

	// TMap - массив, в котором есть контейнер и ключ к нему(value)
	// Далее нужна обёртка TEnumAsByte - шаблон, который позволяет обернуть SurfaceType
	// затем EPhysicalSurface - это тип поверхности (которых порядка 64 штук)
	// По EPhysicalSurface нужен UMaterialInterface, назовём это HitDecals
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
		TMap<TEnumAsByte<EPhysicalSurface>, UMaterialInterface*> HitDecals;
	// Так же со звуком
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
		USoundBase* HitSound = nullptr;
	// И для эффекта попадания
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
		TMap<TEnumAsByte<EPhysicalSurface>, UParticleSystem*> HitFXs;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		UParticleSystem* ExploseFX = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		USoundBase* ExploseSound = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
		float ProjectileMaxRadiusDamage = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		float ProjectileMinRadiusDamage = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		float ExploseMaxDamage = 40.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		float ExploseFalloffCoef = 1.0f;

	 

	//Hit FX Actor?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
		bool bIsLikeBomp = false;        // Для будущих взрывов
	

};

USTRUCT(BlueprintType)
struct FWeaponDispersion
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
		float Aim_StateDispersionAimMax = 2.0f;	 // Разброс при начала прицеливания
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
		float Aim_StateDispersionAimMin = 0.3f;		 // МАХ разброс
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
		float Aim_StateDispersionAimRecoil = 1.0f;		 //  MIN разброс
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
		float Aim_StateDispersionAimReduction = 0.3f; // Коэф. при выстреле

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
		float AimWalk_StateDispersionAimMax = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
			float AimWalk_StateDispersionAimMin = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
			float AimWalk_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
			float AimWalk_StateDispersionAimReduction = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
			float Walk_StateDispersionAimMax = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
			float Walk_StateDispersionAimMin = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
			float Walk_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
			float Walk_StateDispersionAimReduction = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
			float Run_StateDispersionAimMax = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
			float Run_StateDispersionAimMin = 4.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
			float Run_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
			float Run_StateDispersionAimReduction = 0.1f;


};

USTRUCT(BlueprintType)
// Этот синтаксис и подключенный DataTable позволяет
// создавать на основе этой структуры таблицы в проекте.
struct FWeaponInfo : public FTableRowBase 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Class")
		TSubclassOf<class AWeaponDefault> WeaponClass = nullptr;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "State")
		float RateOfFire = 0.5f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "State")
		float ReloadTime = 2.0f;							// Время перезарядки
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "State")
		int32 MaxRound = 10;								// Количество патронов МАХ
	// Для дробовика
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
		int32 NumberProjectileByShot = 1;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Dispersion")
		FWeaponDispersion DispersionWeapon;					// Структура разброса оружия

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Sound")
		USoundBase* SoundFireWeapon = nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Sound")
		USoundBase* SoundReloadWeapon = nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "FX")
		UParticleSystem* EffectFireWeapon = nullptr;	// Эффект при выстреле

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Projectile")
		FProjectileInfo ProjectileSetting;

	// В случае,если нет projectile, то оружие будет стрелять трейсами
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Trace")
		float WeaponDamage = 20.0f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Trace")
		float DistanceTrace = 2000.0f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "HitEffect")
		UDecalComponent* DecalOnHit = nullptr;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Anim")
		UAnimMontage* AnimCharFire = nullptr;				// Переменные анимаций
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Anim")
		UAnimMontage* AnimCharReload = nullptr;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Mesh")
		UStaticMesh* MagazineDrop = nullptr;                // Падающий магазин
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Mesh")
		UStaticMesh* SleeveBullets = nullptr;				// Падающие гильзы

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponSettings")
	//	float WeaponDamage = 20.0f;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponSettings")
	//	float RateOfFire = 0.5f;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponSettings")
	//	FProjectileInfo ProjectileSetting;

};
USTRUCT(BlueprintType)
struct FAddicionalWeaponInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
		int32 Round = 10; // Стартовое кол-во патронов в магазине 

};

USTRUCT(BlueprintType)
struct FAnimationWeaponInfo
{
	GENERATED_BODY()

		// Переменные анимаций
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Char")
		UAnimMontage* AnimCharFire = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Char")
		UAnimMontage* AnimCharFireAim = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Char")
		UAnimMontage* AnimCharReload = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Char")
		UAnimMontage* AnimCharReloadAim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Weapon")
		UAnimMontage* AnimWeaponReload = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Weapon")
		UAnimMontage* AnimWeaponReloadAim = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Weapon")
		UAnimMontage* Fire = nullptr;
};

UCLASS()
class TPS_API UType : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
		
};