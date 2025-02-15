// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "/My_Projects/TPS/Source/TPS/FunctionLibrary/Type.h"
#include "Components/ArrowComponent.h"
#include </My_Projects/TPS/Source/TPS/Public/Game/ProjectileDefault.h>
#include "/UE/UE_5.0/Engine/Source/Runtime/Engine/Public/Net/UnrealNetwork.h"
#include "WeaponDefault.generated.h"


// �������� ��� �����������
// ������ DECLARE_DYNAMIC_MULTICAST_DELEGATE
// � ���������: ��� ��������, ������ �� ����.������ � ��� ��������
// ����,� ������ ���� ������ ��� ���������� ���� ���� ��������
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponReloadStart, UAnimMontage*, Anim);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponReloadEnd,bool,bIsSuccess,int32 ,AmmoSafe);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponFireStart, UAnimMontage*, AnimFireChar);


UCLASS()
class TPS_API AWeaponDefault : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponDefault();
	// ���������� ���������� ��������
	FOnWeaponReloadStart OnWeaponReloadStart;
	FOnWeaponReloadEnd OnWeaponReloadEnd;
	FOnWeaponFireStart OnWeaponFireStart;

	// ����������,������� ��������� � ��������� ��� ���������� � BP
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class USceneComponent* SceneComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class USkeletalMeshComponent* SkeletalMeshWeapon = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UStaticMeshComponent* StaticMeshWeapon = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UArrowComponent* ShootLocation = nullptr;

	// ��������� FWeaponInfo,������� ������� � Types
	UPROPERTY()
	FWeaponInfo WeaponSetting;
	// ��� ���� ���������. ���������� � ��� ����� ��������
	// ��� ����� ��������� ���-�� �������� � ������
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "WeaponInfo")
	FAdditionalWeaponInfo AdditionalWeaponInfo;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void FireTick(float DeltaTime);
	// ��� �����������
	void ReloadTick(float DeltaTime);

	// ��� ��������
	void DispersionTick(float DeltaTime);

	// �-� ������� �������������� �� ������
	void WeaponInit();


	// ������� ����������, ���������� �� ����������� ��������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireLogic")
		bool WeaponFiring = false;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "ReloadLogic")
		bool WeaponReloading = false; // ���� �� �����������
	

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetWeaponStateFire_OnServer(bool bIsFire);

	bool CheckWeaponCanFire();

	FProjectileInfo GetProjectile();


	void Fire();

	UFUNCTION (Server, Reliable)
	void UpdateStateWeapon_OnServer(EMovementState NewMovementState);
	void ChangeDispersion();
	//
	void ChangeDispersionByShoot();
	float GetCurrentDispersion() const;
	FVector ApplyDispersionToShoot(FVector DirectionShoot)const;
	FVector GetFireEndLocation()const;
	// ������� ��� ���������
	int8 GetNumberProjectileByShot() const;

	UFUNCTION(BlueprintCallable)
	int32 GetWeaponRound();

	// ������������� �����������
	void InitReload();
	// ����� �����������
	void FinishReload();
	// ������ �����������
	void CancelReload();

	bool CheckCanWeaponReload();
	int8 GetAviableAmmoForReload();

	void ShellDropTick(float DeltaTime);
	void ClipDropTick(float DeltaTime);

	UFUNCTION(Server, Reliable)
		void InitDropMesh_OnServer(UStaticMesh* DropMesh, 
			FTransform Offset, 
			FVector DropImpulseDirection, 
			float LifeTimeMesh, 
			float ImpulseRandomDispersion, 
			float PowerImpulse, 
			float CustomMass);

	//Timers'flags
	float FireTime = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReloadLogic")
	float ReloadTimer = 0.0f;
	bool WeaponAiming = false;

	bool BlockFire = false;
	// Timer drop magazine on Reload
	bool DropClipFlag = false;
	float DropClipTimer = -1.0f;
	// Shell flag
	bool DropShellFlag = false;
	float DropShellTimer = -1.0f;
	
	// �������
	//UPROPERTY(Replicated)
	bool ShouldReduseDispersion = false;
	float CurrentDispersion = 0.0f;
	float CurrentDispersionMax = 1.0f;
	float CurrentDispersionMin = 0.1f;
	float CurrentDispersionRecoil = 0.1f;
	float CurrentDispersionReduction = 0.1f;

	UPROPERTY(Replicated)
	FVector ShootEndLocation = FVector(0);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		bool  ShowDebug = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	    float SizeVectorToChangeShootDirectionLogic = 100.0f;;
	
	UFUNCTION(Server, Unreliable)
		void UpdateWeaponByCharacterMovementStateOnServer(FVector NewShootEndLocation, bool NewShouldReduceDispersion);

	UFUNCTION(NetMulticast, Unreliable)
		void AnimWeaponStart_Multicast(UAnimMontage* Anim);
	UFUNCTION(NetMulticast, Unreliable)
		void ShellDropFire_Multicast(UStaticMesh* DropMesh, 
			FTransform Offset, FVector DropImpulseDirection, 
			float LifeTimeMesh, 
			float ImpulseRandomDispersion, 
			float PowerImpulse, 
			float CustomMass,
			FVector LocalDir);
	UFUNCTION(NetMulticast, Unreliable)
		void FXWeaponFire_Multicast(UParticleSystem* FxFire, USoundBase* SoundFire);
	/// /////////////////////////////// ��� � ����� ���, �� ���� ��� �����
	UFUNCTION(NetMulticast, Reliable)
		void TraceFX_Multicast(UParticleSystem* FxTemplate, FHitResult HitResult);
	UFUNCTION(NetMulticast, Reliable)
		void TraceSound_Multicast(USoundBase* HitSound, FHitResult HitResult);
};
