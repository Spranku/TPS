// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <PhysicalMaterials/PhysicalMaterial.h>
#include "Particles/ParticleSystemComponent.h"
#include "TPS_StateEffect.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class TPS_API UTPS_StateEffect : public UObject
{
	GENERATED_BODY()
public:

	virtual bool InitObject(AActor* Actor);
	//virtual void ExecuteEffect(float DeltaTime);
	virtual void DestroyObject();
	
	//virtual bool CheckStakebleEffect();

	// ������ ����� ��������, ��������� � ��������������
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Setting")
		TArray<TEnumAsByte<EPhysicalSurface>> PossibleInteractSurface;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
		bool bIsStakable = false;

	AActor* myActor = nullptr;
};

// ����� ��������, �������������� ���� ���
UCLASS()
class TPS_API UTPS_StateEffect_ExecuteOnce : public UTPS_StateEffect
{
	GENERATED_BODY()

public:
	bool InitObject(AActor* Actor) override;
	void DestroyObject() override;

	virtual void ExecuteOnce();

	// ���� �������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Execute Once")
		float Power = 20.0f;
};

// ����� �������, ����� ������� ������������ �� �������
UCLASS()
class TPS_API UTPS_StateEffect_ExecuteTimer : public UTPS_StateEffect
{
	GENERATED_BODY()

public:
	bool InitObject(AActor* Actor) override;
	void DestroyObject() override;

	virtual void Execute();

	// ���� �������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Execute Timer")
		float Power = 20.0f;
	// ���������� ������ �� 5�,���������� Execute()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Execute Timer")
		float Timer = 5.0f;
	// 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Execute Timer")
		float RateTime = 1.0f;

	FTimerHandle TimerHandle_ExecuteTimer;
	FTimerHandle TimerHandle_EffectTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Execute Effect")
	UParticleSystem* ParticleEffect = nullptr;
	// ������ �� ��������� �������, ����� ���������� ��� ����� �� ������ ������������
	UParticleSystemComponent* ParticleEmmiter = nullptr;
};

UCLASS()
class TPS_API UTPS_StunEffect : public UTPS_StateEffect
{
	GENERATED_BODY()

public:

	bool InitObject(AActor* Actor) override;
	void DestroyObject() override;
	
	void ExecuteStun();
	void ReturnInput();
	// flag
	bool bIsReturn = false;

	// ���������� ������ �� 5�,���������� Execute()
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Execute Timer")
	//	float Timer = 5.0f;
	// 
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Execute Timer")
	//	float RateTime = 1.0f;

	FTimerHandle TimerHandle_ExecuteTimer;
	FTimerHandle TimerHandle_EffectTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Execute Timer")
		float Timer = 5.0f;
	// 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Execute Timer")
		float EnableInputTimer = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Execute Montage")
		UAnimMontage* StunAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Execute Effect")
		UParticleSystem* ParticleEffect = nullptr;

	UParticleSystemComponent* ParticleEmmiter = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Execute Sound")
		USoundBase* StunSound = nullptr;

};

