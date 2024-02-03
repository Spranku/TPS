// Fill out your copyright notice in the Description page of Project Settings.


#include "/My_Projects/TPS/Source/TPS/Game/TPS_EnvironmentStructure.h"
#include <Materials/MaterialInterface.h>
#include "/UE/UE_5.0/Engine/Source/Runtime/Engine/Classes/Engine/ActorChannel.h"
#include <PhysicalMaterials/PhysicalMaterial.h>
#include "/UE/UE_5.0/Engine/Source/Runtime/Engine/Public/Net/UnrealNetwork.h"
#include "/UE/UE_5.0/Engine/Source/Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "/UE/UE_5.0/Engine/Source/Runtime/Engine/Classes/Engine/ActorChannel.h"
#include <TPS/FunctionLibrary/Type.h>
#include <Kismet/GameplayStatics.h> 

// Sets default values
ATPS_EnvironmentStructure::ATPS_EnvironmentStructure()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);

}

// Called when the game starts or when spawned
void ATPS_EnvironmentStructure::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATPS_EnvironmentStructure::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

EPhysicalSurface ATPS_EnvironmentStructure::GetSurfaceType()
{
	EPhysicalSurface Result = EPhysicalSurface::SurfaceType_Default;
	UStaticMeshComponent* myMesh = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (myMesh)
	{
		UMaterialInterface* myMaterial = myMesh->GetMaterial(0);
			if (myMaterial)
			{
				Result = myMaterial->GetPhysicalMaterial()->SurfaceType;
			}
	}
	return Result;
}

TArray<UTPS_StateEffect*> ATPS_EnvironmentStructure::GetAllCurrentEffects()
{
	return Effects;
}

void ATPS_EnvironmentStructure::RemoveEffect_Implementation(UTPS_StateEffect* RemoveEffect)
{
	Effects.Remove(RemoveEffect);

	if (!RemoveEffect->bIsAutoDestroyParticleEffect)
	{
		SwitchEffect(RemoveEffect, false);
		EffectRemove = RemoveEffect;
	}
}

void ATPS_EnvironmentStructure::AddEffect_Implementation(UTPS_StateEffect* newEffect)
{
	Effects.Add(newEffect);

	if (!newEffect->bIsAutoDestroyParticleEffect)
	{
		SwitchEffect(newEffect, true);
		EffectAdd = newEffect;
	}
	else
	{
		if (newEffect->ParticleEffect)
		{
			ExecuteEffectAdded_OnServer(newEffect->ParticleEffect);
		}
	}
}

void ATPS_EnvironmentStructure::EffectAdd_OnRep()
{
	if (EffectAdd)
	{
		SwitchEffect(EffectAdd, true);
	}
}

void ATPS_EnvironmentStructure::EffectRemove_OnRep()
{
	if (EffectRemove)
	{
		SwitchEffect(EffectAdd, false);
	}
}

void ATPS_EnvironmentStructure::ExecuteEffectAdded_OnServer_Implementation(UParticleSystem* ExecuteFX)
{
	ExecuteEffectAdded_Multicast(ExecuteFX);
}

void ATPS_EnvironmentStructure::ExecuteEffectAdded_Multicast_Implementation(UParticleSystem* ExecuteFX)
{
	UType::ExecuteEffectAdded(ExecuteFX,this, OffsetEffect, NAME_None);
}

void ATPS_EnvironmentStructure::SwitchEffect(UTPS_StateEffect* Effect, bool bIsAdd)
{
	if (bIsAdd)
	{
		if (Effect && Effect->ParticleEffect)
		{
			FName NameBoneToAttached = NAME_None;
			FVector Loc = OffsetEffect;

			USceneComponent* mySceneComp = GetRootComponent();
			if (mySceneComp)
			{
				UParticleSystemComponent* newParticleSystem = UGameplayStatics::SpawnEmitterAttached(Effect->ParticleEffect,
					/*myActor->GetRootComponent(),*/
					mySceneComp,
					NameBoneToAttached,
					Loc,
					FRotator::ZeroRotator,
					EAttachLocation::SnapToTarget,
					false);

				ParticleSystemEffects.Add(newParticleSystem);
			}
		}
	}
	else
	{
		int32 i = 0;
		bool bIsFind = false;

		if (ParticleSystemEffects.Num() > 0)
		{
			while (i < ParticleSystemEffects.Num(), !bIsFind)
			{
				if (ParticleSystemEffects[i]->Template && Effect->ParticleEffect && Effect->ParticleEffect == ParticleSystemEffects[i]->Template)
				{
					bIsFind = true;
					ParticleSystemEffects[i]->DeactivateSystem();
					ParticleSystemEffects[i]->DestroyComponent();
					ParticleSystemEffects.RemoveAt(i);
				}
				i++;
			}
		}
	}
}

bool ATPS_EnvironmentStructure::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool Wrote = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (int32 i = 0; i < Effects.Num(); i++)
	{
		if (Effects[i]) { Wrote |= Channel->ReplicateSubobject(Effects[i], *Bunch, *RepFlags); }
	}
	return Wrote;
}

void ATPS_EnvironmentStructure::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATPS_EnvironmentStructure, Effects);
	DOREPLIFETIME(ATPS_EnvironmentStructure, EffectAdd);
	DOREPLIFETIME(ATPS_EnvironmentStructure, EffectRemove);
}