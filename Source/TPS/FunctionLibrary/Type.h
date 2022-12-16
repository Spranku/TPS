// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Kismet/BlueprintFunctionLibrary.h>
#include "Type.generated.h"


// Eнумерация с доступом к BP
// Класс Eнумерации EMovementState 
// У Eнумерации обязательно нужно указать её длину.
// uint8 должно хватить. 
// В теле Eнумерации нужно прописать состояния движения.
// Первым будет Aim_State, далее синтаксис
// UMETA(DisplayName... - то,что отображается в BP,
// грубо говоря имя Eнумерации. Далее запятая,
// вторая инумерация Walk_State,третья Run_State.
// 
UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Aim_State UMETA(DisplayName = "Aim State"),
	Walk_State UMETA(DisplayeName = "Walk State"),
	Run_State UMETA(DisplayeName = "Run State")
};

// Чтобы всё было структурировано, добавим структуру
// Структуры добавляются практически также как ENUM
// За исключением названия макроса.
// Сперва добавляется GENERATED_BODY()
// В структуре будет храниться float переменные
// AimSpeed (скорость в прицелиивании),
// WalkSpeed (скорость ходьбы (не обязательно),
// RunSpeed (скорость бега)
// Так же нужно добавить Unreal`овский макрос
// UPROPERTY(EditAnywhere, BlueprintReadWrite)
// Эта струкрура будет виидна в BP, ей можно
// читать и изменять.
// 
USTRUCT(BlueprintType) 
struct FCharacterSpeed
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Movement")
	float AimSpeed = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RunSpeed = 600.0f;
};

UCLASS()
class TPS_API UType : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};