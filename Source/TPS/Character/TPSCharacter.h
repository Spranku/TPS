// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <TPS/FunctionLibrary/Type.h>
#include "TPSCharacter.generated.h"

UCLASS(Blueprintable)
class ATPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATPSCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	// ��������������� �������, ������� ��������� �������������� ��� ������
	virtual void SetupPlayerInputComponent(class UInputComponent* NewInputComponent) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
public:

	// ����������� ���������� ��� ENUM
	// �� ��������� ��� ����� Run. ����� ������������ �� � BP
	// ��������� ��� �� ������
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Movement")
	EMovementState MovementState = EMovementState::Run_State;

	// ���������� ���������� ��� STRUCT �� Type.h
	// ����� � Type.h �� ��������� ��������� FCharracterrSpeed.
	// Ÿ ���������� ������� ����� � ������� ���������� MovementInfo
	// ������������� �� �����,�.�. ��� ���� � Type.h
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FCharacterSpeed MovementInfo;

	// �������� ������� �����-����
	UFUNCTION()
	void InputAxisX(float Value);

	// �������� ������� ������-�����
	UFUNCTION()
	void InputAxisY(float Value);

	// ���������� ��� �������
	float AxisX = 0.0f;
	float AxisY = 0.0f;

	// Tick Function
	// ����� ����� ���������� Add Movement Input
	// ����������� �������� � � �������� Tick
	UFUNCTION()
	void MovementTick(float DeltaTime);

	// ��������� ������ ������� � Character
	// � UE ���� ������� ��� �������
	// �� ����� ������������ BlueprintCallable,
	// ������� ������� � ���, ��� ��� ������� �����
	// ������� � UE �� BP
	
	//CharacterUpdate
	// � ������� �� ����� ������ ��������� ���������
	// �������� ����. ������������ - ������ ��������
	UFUNCTION(BlueprintCallable)
	void CharacterUpdate();

	// ChangeMovementState ����� ��������� ��� EMovementState
	// ��� ����� ��������� Type.h ����.
	UFUNCTION(BlueprintCallable)
	void ChangeMovementState(EMovementState NewMovementState);
};

