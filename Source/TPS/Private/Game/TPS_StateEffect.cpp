// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TPS_StateEffect.h"
#include "TPS/Character/TPSCharacterHealthComponent.h"
#include "/My_Projects/TPS/Source/TPS/Interface/TPS_IGameActor.h"
#include <Kismet/GameplayStatics.h> 
#include "/My_Projects/TPS/Source/TPS/Character/TPSCharacter.h"


bool UTPS_StateEffect::InitObject(AActor* Actor, FName NameBoneHit)
{
	myActor = Actor;

	ITPS_IGameActor* myInterface = Cast<ITPS_IGameActor>(myActor);
	if (myInterface)
	{
		myInterface->AddEffect(this);
	}

	return true;
}

void UTPS_StateEffect::DestroyObject()
{
	ITPS_IGameActor* myInterface = Cast<ITPS_IGameActor>(myActor);

	if (myInterface)
	{
		myInterface->RemoveEffect(this);
	}

	// ����� ����������� ������� �������� ������
	// ���� ������ ���������� � �� �������,
	// �� �������� �����������
	myActor = nullptr;
	if (this && this->IsValidLowLevel())
	{
		this->ConditionalBeginDestroy();
	}
	
}

bool UTPS_StateEffect_ExecuteOnce::InitObject(AActor* Actor, FName NameBoneHit)
{
	Super::InitObject(Actor, NameBoneHit);
	ExecuteOnce();
	return true;
}

void UTPS_StateEffect_ExecuteOnce::DestroyObject()
{
	Super::DestroyObject();
}

void UTPS_StateEffect_ExecuteOnce::ExecuteOnce()
{
	// ��������� ������� � HealthComponent
	if (myActor)
	{
		UTPSHealthComponent* myHealthComp = Cast<UTPSHealthComponent>(myActor->GetComponentByClass(UTPSHealthComponent::StaticClass()));
		if (myHealthComp)
		{
			myHealthComp->ChangeHealthValue(Power);
		}
	}
	DestroyObject();
}

bool UTPS_StateEffect_ExecuteTimer::InitObject(AActor* Actor, FName NameBoneHit)
{
	Super::InitObject(Actor, NameBoneHit);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_EffectTimer, this, &UTPS_StateEffect_ExecuteTimer::DestroyObject, Timer, false);

	// �������� ������������ �������,������� ������������ ������ �������.
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_ExecuteTimer, this, &UTPS_StateEffect_ExecuteTimer::Execute, RateTime, true);
	
	// ���� ������ ����,�� ������������ ��� � ������,������ ������������� � ������
	if (ParticleEffect)
	{
		FName NameBoneToAttached = NameBoneHit; // �����,���� ����� ������������ ������;
		FVector Location = FVector(0);		    // �����,���� ����� ������������ ������;

		USceneComponent* myMesh = Cast<USceneComponent>(myActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
			if (myMesh)
			{
				ParticleEmmiter = UGameplayStatics::SpawnEmitterAttached(ParticleEffect,
					/*myActor->GetRootComponent(),*/
					myMesh,
					NameBoneToAttached,
					Location,
					FRotator::ZeroRotator,
					EAttachLocation::SnapToTarget,
					false);
			}
			else
			{
				ParticleEmmiter = UGameplayStatics::SpawnEmitterAttached(ParticleEffect,
					myActor->GetRootComponent(),
					NameBoneToAttached,
					Location,
					FRotator::ZeroRotator,
					EAttachLocation::SnapToTarget,
					false);
			}
	}
	return true;
}

void UTPS_StateEffect_ExecuteTimer::DestroyObject()
{
	ParticleEmmiter->DestroyComponent();
	ParticleEmmiter = nullptr;

	Super::DestroyObject();
}

void UTPS_StateEffect_ExecuteTimer::Execute()
{
	// ��������� ������� � HealthComponent
	if (myActor)
	{
		UTPSHealthComponent* myHealthComp = Cast<UTPSHealthComponent>(myActor->GetComponentByClass(UTPSHealthComponent::StaticClass()));
		if (myHealthComp)
		{
			myHealthComp->ChangeHealthValue(Power);
		}
	}
}

bool UTPS_StunEffect::InitObject(AActor* Actor,FName NameBoneHit)
{
	Super::InitObject(Actor,NameBoneHit);
	UE_LOG(LogTemp, Warning, TEXT("InitObject successful"));

	ExecuteStun();

	if (ParticleEffect)
	{
		FName NameBoneToAttached; // �����,���� ����� ������������ ������;
		FVector Location;		  // �����,���� ����� ������������ ������;

		ParticleEmmiter = UGameplayStatics::SpawnEmitterAttached(ParticleEffect,
			myActor->GetRootComponent(),
			NameBoneToAttached,
			Location,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			false);
	}
	if (StunSound)
	{
		UGameplayStatics::SpawnSound2D(this, StunSound, 0.5);
	}

	return true;
}

void UTPS_StunEffect::DestroyObject()
{
	if (bIsReturn)
	{
		ParticleEmmiter->DestroyComponent();
		ParticleEmmiter = nullptr;
		myActor = nullptr;
		this->ConditionalBeginDestroy();
		
	}
}

void UTPS_StunEffect::ExecuteStun()
{	// ��������� ������� � ���������
	ATPSCharacter* newChar = Cast<ATPSCharacter>(myActor);
	
	if (newChar)
	{
		// ���������� ������ ���������
		newChar->DisableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		newChar->bIsAlive = false;

		// �������� ����� 
		newChar->PlayAnimMontage(StunAnim,0.2f);
		// ������ �� �������
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ExecuteTimer, this, &UTPS_StunEffect::ReturnInput, EnableInputTimer, true);
	}
}

void UTPS_StunEffect::ReturnInput()
{	// ��������� ������ ���������
	ATPSCharacter* newChar = Cast<ATPSCharacter>(myActor);
	if (newChar)
	{
		//newChar->WalkEnabled = false;
		newChar->EnableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		newChar->bIsAlive = true;
		
		bIsReturn = true;
		DestroyObject();
	}
}
	
	
