// Fill out your copyright notice in the Description page of Project Settings.


#include "/My_Projects/TPS/Source/TPS/Game/TPS_EnvironmentStructure.h"
#include <Materials/MaterialInterface.h>
#include <PhysicalMaterials/PhysicalMaterial.h>

// Sets default values
ATPS_EnvironmentStructure::ATPS_EnvironmentStructure()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

void ATPS_EnvironmentStructure::RemoveEffect(UTPS_StateEffect* RemoveEffect)
{
	Effects.Remove(RemoveEffect);
}

void ATPS_EnvironmentStructure::AddEffect(UTPS_StateEffect* newEffect)
{
	Effects.Add(newEffect);
}