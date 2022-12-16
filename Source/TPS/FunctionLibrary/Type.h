// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Kismet/BlueprintFunctionLibrary.h>
#include "Type.generated.h"


// E��������� � �������� � BP
// ����� E��������� EMovementState 
// � E��������� ����������� ����� ������� � �����.
// uint8 ������ �������. 
// � ���� E��������� ����� ��������� ��������� ��������.
// ������ ����� Aim_State, ����� ���������
// UMETA(DisplayName... - ��,��� ������������ � BP,
// ����� ������ ��� E���������. ����� �������,
// ������ ���������� Walk_State,������ Run_State.
// 
UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Aim_State UMETA(DisplayName = "Aim State"),
	Walk_State UMETA(DisplayeName = "Walk State"),
	Run_State UMETA(DisplayeName = "Run State")
};

// ����� �� ���� ���������������, ������� ���������
// ��������� ����������� ����������� ����� ��� ENUM
// �� ����������� �������� �������.
// ������ ����������� GENERATED_BODY()
// � ��������� ����� ��������� float ����������
// AimSpeed (�������� � �������������),
// WalkSpeed (�������� ������ (�� �����������),
// RunSpeed (�������� ����)
// ��� �� ����� �������� Unreal`������ ������
// UPROPERTY(EditAnywhere, BlueprintReadWrite)
// ��� ��������� ����� ������ � BP, �� �����
// ������ � ��������.
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