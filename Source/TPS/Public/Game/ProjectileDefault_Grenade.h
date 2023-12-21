// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/ProjectileDefault.h"
#include "GameFramework/Actor.h"
#include "ProjectileDefault_Grenade.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API AProjectileDefault_Grenade : public AProjectileDefault
{
	
	GENERATED_BODY()

public:
	AProjectileDefault_Grenade();

protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;

	void TimerExplose(float DeltaTime);
	
	virtual void BulletCollisionSphereHit(class UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit) override;

	virtual void ImpactProjectile() override;

	void Explose();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade")
	bool TimerEnabled = false;

	float TimerToExplose = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade")
	float TimeToExlose = 5.0f;
	

	/// /////////////////////////////// Это я писал сам, не факт что верно
	UFUNCTION(NetMulticast, Reliable)
	void ExploseFX_Multicast(UParticleSystem* FxTemplate, FVector Loc, FRotator Rot);
	UFUNCTION(NetMulticast, Reliable)
	void ExploseSound_Multicast(USoundBase* HitSound, FVector HitLoc);
};
