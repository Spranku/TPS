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

	// Переопределение функции, которая позволяет переопределить все кнопки
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

	// Добаввление переменной для ENUM
	// По умолчанию она будет Run. Чтобы использовать ее в BP
	// необходим так же макрос
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Movement")
	EMovementState MovementState = EMovementState::Run_State;

	// Добавление переменной для STRUCT из Type.h
	// Ранее в Type.h мы создавали структуру FCharracterrSpeed.
	// Её необходимо указать здесь и назвать переменную MovementInfo
	// Инициализация не нужна,т.к. она есть в Type.h
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FCharacterSpeed MovementInfo;

	// Описание инпутов вверх-вниз
	UFUNCTION()
	void InputAxisX(float Value);

	// Описание инпутов вправо-влево
	UFUNCTION()
	void InputAxisY(float Value);

	// Переменные для инпутов
	float AxisX = 0.0f;
	float AxisY = 0.0f;

	// Tick Function
	// Здесь будет вызываться Add Movement Input
	// Обязательно добавить её в основной Tick
	UFUNCTION()
	void MovementTick(float DeltaTime);

	// Добавляем важные функции в Character
	// В UE есть макросы для функций
	// Мы будем использовать BlueprintCallable,
	// которая говорит о том, что эту функцию можно
	// вызвать в UE из BP
	
	//CharacterUpdate
	// С помощью неё будем менять состояние персонажа
	// Например сост. прицеливания - меняем скорость
	UFUNCTION(BlueprintCallable)
	void CharacterUpdate();

	// ChangeMovementState будет принимать наш EMovementState
	// для этого подключим Type.h файл.
	UFUNCTION(BlueprintCallable)
	void ChangeMovementState(EMovementState NewMovementState);
};

