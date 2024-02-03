// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TPS_StateEffect.h"
#include "TPS/Character/TPSCharacterHealthComponent.h"
#include "/My_Projects/TPS/Source/TPS/Interface/TPS_IGameActor.h"
#include <Kismet/GameplayStatics.h> 
#include "/My_Projects/TPS/Source/TPS/Character/TPSCharacter.h"
#include "/UE/UE_5.0/Engine/Source/Runtime/Engine/Public/Net/UnrealNetwork.h"


bool UTPS_StateEffect::InitObject(AActor* Actor, FName NameBoneHit)
{
	myActor = Actor;

	NameBone = NameBoneHit;

	ITPS_IGameActor* myInterface = Cast<ITPS_IGameActor>(myActor);
	if (myInterface)
	{
		myInterface->Execute_AddEffect(myActor ,this);
	}
	
	return true;
}

void UTPS_StateEffect::DestroyObject()
{
	ITPS_IGameActor* myInterface = Cast<ITPS_IGameActor>(myActor);

	if (myInterface)
	{
		myInterface->Execute_RemoveEffect(myActor,this);
	}

	// Перед уничтожение объекта чистятся ссылки
	// Если объект существует и он валиден,
	// то начинаем уничтожение
	myActor = nullptr;
	if (this && this->IsValidLowLevel())
	{
		this->ConditionalBeginDestroy();
	}
	
}

void UTPS_StateEffect::FXSpawnByStateEffect_Multicast_Implementation(UParticleSystem* Effect, FName NameBoneHit)
{
	// Если эффект есть,то присоединяем его к актору,котрый инициировался в начале
	//if (Effect)
	//{
	//	FName NameBoneToAttached = NameBoneHit; // Место,куда хотим присоединить эффект;
	//	FVector Location = FVector(0);		    // Место,куда хотим присоединить эффект;

	//	USceneComponent* myMesh = Cast<USceneComponent>(myActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	//	if (myMesh)
	//	{
	//		ParticleEmmiter = UGameplayStatics::SpawnEmitterAttached(Effect,
	//			/*myActor->GetRootComponent(),*/
	//			myMesh,
	//			NameBoneToAttached,
	//			Location,
	//			FRotator::ZeroRotator,
	//			EAttachLocation::SnapToTarget,
	//			false);
	//	}
	//	else
	//	{
	//		ParticleEmmiter = UGameplayStatics::SpawnEmitterAttached(Effect,
	//			myActor->GetRootComponent(),
	//			NameBoneToAttached,
	//			Location,
	//			FRotator::ZeroRotator,
	//			EAttachLocation::SnapToTarget,
	//			false);
	//	}
	//}
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
	// Получение доступа к HealthComponent
	if (myActor)
	{
		UTPSHealthComponent* myHealthComp = Cast<UTPSHealthComponent>(myActor->GetComponentByClass(UTPSHealthComponent::StaticClass()));
		if (myHealthComp)
		{
			myHealthComp->ChangeHealthValue_OnServer(Power);
		}
	}
	DestroyObject();
}

bool UTPS_StateEffect_ExecuteTimer::InitObject(AActor* Actor, FName NameBoneHit)
{
	Super::InitObject(Actor, NameBoneHit);

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_EffectTimer, this, &UTPS_StateEffect_ExecuteTimer::DestroyObject, Timer, false);
		// Создание зацикленного таймера,который отрабатывает каждую секунду.
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ExecuteTimer, this, &UTPS_StateEffect_ExecuteTimer::Execute, RateTime, true);
	}

	//if (ParticleEffect)
	//{
		// FXSpawnByStateEffect_Multicast(ParticleEffect,NameBoneHit); //To Remove
	//}
	

	// Если эффект есть,то присоединяем его к актору,котрый инициировался в начале
	//if (ParticleEffect)
	//{
	//	FName NameBoneToAttached = NameBoneHit; // Место,куда хотим присоединить эффект;
	//	FVector Location = FVector(0);		    // Место,куда хотим присоединить эффект;

	//	USceneComponent* myMesh = Cast<USceneComponent>(myActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	//		if (myMesh)
	//		{
	//			ParticleEmmiter = UGameplayStatics::SpawnEmitterAttached(ParticleEffect,
	//				/*myActor->GetRootComponent(),*/
	//				myMesh,
	//				NameBoneToAttached,
	//				Location,
	//				FRotator::ZeroRotator,
	//				EAttachLocation::SnapToTarget,
	//				false);
	//		}
	//		else
	//		{
	//			ParticleEmmiter = UGameplayStatics::SpawnEmitterAttached(ParticleEffect,
	//				myActor->GetRootComponent(),
	//				NameBoneToAttached,
	//				Location,
	//				FRotator::ZeroRotator,
	//				EAttachLocation::SnapToTarget,
	//				false);
	//		}
	//}
	return true;
}

void UTPS_StateEffect_ExecuteTimer::DestroyObject()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	}

	//ParticleEmmiter->DestroyComponent();
	//ParticleEmmiter = nullptr;

	Super::DestroyObject();
}

void UTPS_StateEffect_ExecuteTimer::Execute()
{
	// Получение доступа к HealthComponent
	if (myActor)
	{
		UTPSHealthComponent* myHealthComp = Cast<UTPSHealthComponent>(myActor->GetComponentByClass(UTPSHealthComponent::StaticClass()));
		if (myHealthComp)
		{
			myHealthComp->ChangeHealthValue_OnServer(Power);
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
		FName NameBoneToAttached; // Место,куда хотим присоединить эффект;
		FVector Location;		  // Место,куда хотим присоединить эффект;

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
{	// Получение доступа к персонажу
	ATPSCharacter* newChar = Cast<ATPSCharacter>(myActor);
	
	if (newChar)
	{
		// Отключение инпута персонажа
		newChar->DisableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));

		//newChar->bIsAlive = false;
		
		bool newAlive = false;
		newAlive = newChar->CharHealthComponent->GetIsAlive();

		// Анимация стана 
		newChar->PlayAnimMontage(StunAnim,0.2f);
		// Таймер на дестрой
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ExecuteTimer, this, &UTPS_StunEffect::ReturnInput, EnableInputTimer, true);
	}
}

void UTPS_StunEffect::ReturnInput()
{	// Включение инпута персонажа
	ATPSCharacter* newChar = Cast<ATPSCharacter>(myActor);
	if (newChar)
	{
		//newChar->WalkEnabled = false;
		newChar->EnableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));

		//newChar->bIsAlive = true;
		bool newAlive = true;
		newAlive = newChar->CharHealthComponent->GetIsAlive();
		
		bIsReturn = true;
		DestroyObject();
	}
}
	
// НЕ РАБОТАЕТ
//void UTPS_StunEffect::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

//	DOREPLIFETIME(UTPS_StunEffect, NameBone);
//}

