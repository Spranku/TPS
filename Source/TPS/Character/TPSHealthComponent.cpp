// Fill out your copyright notice in the Description page of Project Settings.


// #include "Character/TPSHealthComponent.h"
#include </My_Projects/TPS/Source/TPS/Character/TPSHealthComponent.h>

// Sets default values for this component's properties
UTPSHealthComponent::UTPSHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UTPSHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UTPSHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UTPSHealthComponent::GetCurrentHealth()
{
	return Health;
}

void UTPSHealthComponent::SetCurrentHealth(float NewHealth)
{
	Health = NewHealth;
}

void UTPSHealthComponent::ChangeHealthValue(float ChangeValue)
{
	// Перед тем, как наносить какой-либо урон, мы будем умножать 
	// изменяемое значение на глобальный коэф. дамага
	// Если он равен 2/3/4/5, то объект будет поглащать много урона
	// А если он равен 5.0f, то выходит что объект бронирован в 2 раза
	ChangeValue = ChangeValue*CoefDamage;

	Health += ChangeValue;

	if (Health > 100.0f)
	{
		Health = 100.0f;
	}
	else
	{
		if (Health < 0.0f)
		{
			OnDead.Broadcast();
		}
	}
	OnHealthChange.Broadcast(Health, ChangeValue);
}



