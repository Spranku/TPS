// Fill out your copyright notice in the Description page of Project Settings.


#include <TPS/Game/WorldItemDefault.h>

// Sets default values
AWorldItemDefault::AWorldItemDefault()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWorldItemDefault::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWorldItemDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

