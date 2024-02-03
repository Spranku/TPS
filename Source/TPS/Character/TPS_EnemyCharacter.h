// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "/My_Projects/TPS/Source/TPS/Interface/TPS_IGameActor.h"
#include "TPS_EnemyCharacter.generated.h"

UCLASS()
class TPS_API ATPS_EnemyCharacter : public ACharacter, public ITPS_IGameActor
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPS_EnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void RemoveEffect(UTPS_StateEffect* RemoveEffect);
	void RemoveEffect_Implementation(UTPS_StateEffect* RemoveEffect) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void AddEffect(UTPS_StateEffect* newEffect);
	void AddEffect_Implementation(UTPS_StateEffect* newEffect) override;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Setting")
		TArray<UTPS_StateEffect*> Effects;

	UPROPERTY(ReplicatedUsing = EffectAdd_OnRep)
		UTPS_StateEffect* EffectAdd = nullptr;
	UPROPERTY(ReplicatedUsing = EffectRemove_OnRep)
		UTPS_StateEffect* EffectRemove = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		TArray<UParticleSystemComponent*> ParticleSystemEffects;
	

	UFUNCTION()
		void EffectAdd_OnRep();
	UFUNCTION()
		void EffectRemove_OnRep();
	UFUNCTION(Server, Reliable)
		void ExecuteEffectAdded_OnServer(UParticleSystem* ExecuteFX);
	UFUNCTION(NetMulticast, Reliable)
		void ExecuteEffectAdded_Multicast(UParticleSystem* ExecuteFX);

	UFUNCTION()
		void SwitchEffect(UTPS_StateEffect* Effect, bool bIsAdd);
};
