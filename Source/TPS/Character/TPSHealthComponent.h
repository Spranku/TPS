// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPSHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChange, float, Health,float, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDead);

//USTRUCT(BlueprintType)
//struct FStartsParam
//{
//	GENERATED_BODY()
//};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_API UTPSHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTPSHealthComponent();

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Health")
		FOnHealthChange OnHealthChange;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Health")
		FOnDead OnDead;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	// Основной параметр жизни
	float Health = 100.0f;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Глобальный коэф. дамага
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Health")
		float CoefDamage = 1.0f;

	UFUNCTION(BlueprintCallable, Category = "Health")
		float GetCurrentHealth();
	UFUNCTION(BlueprintCallable, Category = "Health")
		void SetCurrentHealth(float NewHealth);

	UFUNCTION(BlueprintCallable, Category = "Health")
		virtual void ChangeHealthValue(float ChangeValue);
	// UFUNCTION(BlueprintCallable, Category = "Health")
	// 	virtual void ReceiveDamage(float Damage);
};
