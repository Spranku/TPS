// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Kismet/BlueprintFunctionLibrary.h>
#include "Engine/DataTable.h"
#include "Type.generated.h"


// E��������� � �������� � BP
// ����� E��������� EMovementState 
// � E��������� ����������� ����� ������� � �����.
// uint8 ������ �������. 
// � ���� E��������� ����� ��������� ��������� ��������.
// ������ ����� Aim_State, ����� ���������
// UMETA(DisplayName... - ��,��� ������������ � BP,
// ����� ������ ��� E���������. ����� �������,
// ������ ���������� Walk_State,������ Run_State.
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

// ����� �� ���� ���������������, ������� ���������
// ��������� ����������� ����������� ����� ��� ENUM
// �� ����������� �������� �������.
// ������ ����������� GENERATED_BODY()
// � ��������� ����� ��������� float ����������
// AimSpeed (�������� � �������������),
// WalkSpeed (�������� ������ (�� �����������),
// RunSpeed (�������� ����)
// ��� �� ����� �������� Unreal`������ ������
// UPROPERTY(EditAnywhere, BlueprintReadWrite)
// ��� ��������� ����� ������ � BP, � �����
// ������ � ��������.
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
	// ����� ������ ����� ���� � � ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
	TSubclassOf<class AProjectileDefault> Projectile = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
	float ProjectileDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
		float ProjectileLifeTime = 30.0f; // ����� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
		float IninSpeed = 2000.0f;       // �������� ����

	//Hit FX Actor?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
		bool bIsLikeBomp = false;        // ��� ������� �������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
		float ProjectileMaxRadiusDamage = 200.0f;

};

USTRUCT(BlueprintType)
struct FWeaponDispersion
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
		float DispersionAimStart = 0.5f;	 // ������� ��� ������ ������������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
		float DispersionAimMAx = 1.0f;		 // ��� �������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
		float DispersionAimMin = 0.1f;		 //  MIN �������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
		float DispersionAimShootCoef = 1.0f; // ����. ��� ��������

};

USTRUCT(BlueprintType)
// ���� ��������� � ������������ DataTable ���������
// ��������� �� ������ ���� ��������� ������� � �������.
struct FWeaponInfo : public FTableRowBase 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Class")
		TSubclassOf<class AWeaponDefault> WeaponClass = nullptr;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "State")
		float RateOfFire = 0.5f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "State")
		float ReloadTime = 2.0f;							// ����� �����������
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "State")
		int32 MaxRound = 10;								// ���������� �������� ���

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Dispersion")
		FWeaponDispersion DispersionWeapon;					// ��������� �������� ������

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Sound")
		USoundBase* SoundFireWeapon = nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Sound")
		USoundBase* SoundReloadWeapon = nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "FX")
		UParticleSystem* EffectFireWeapon = nullptr;	// ������ ��� ��������

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Projectile")
		FProjectileInfo ProjectileSetting;

	// � ������,���� ��� projectile, �� ������ ����� �������� ��������
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Trace")
		float WeaponDamage = 20.0f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Trace")
		float DistanceTrace = 2000.0f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "HitEffect")
		UDecalComponent* DecalOnHit = nullptr;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Anim")
		UAnimMontage* AnimCharFire = nullptr;				// ���������� ��������
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Anim")
		UAnimMontage* AnimCharReload = nullptr;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Mesh")
		UStaticMesh* MagazineDrop = nullptr;                // �������� �������
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Mesh")
		UStaticMesh* SleeveBullets = nullptr;				// �������� ������

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
		int32 Round = 10; // ��������� ���-�� �������� � �������� 

};

UCLASS()
class TPS_API UType : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};