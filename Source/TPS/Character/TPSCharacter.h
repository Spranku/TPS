// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <TPS/FunctionLibrary/Type.h>
#include "Game/WeaponDefault.h"
#include "TPSCharacter.generated.h"


//USTRUCT(BlueprintType)
//struct FCharacterSpeedInfo
//{
//	GENERATED_BODY()
//
//
//};

UCLASS(Blueprintable)
class ATPSCharacter : public ACharacter
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
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
	/** Returns CursorToWorld subobject **/
	//FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//class UDecalComponent* CursorToWorld;
public:
	// Cursor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	UMaterialInterface* CursorMaterial = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
   	FVector CursorSize = FVector(20.0f, 40.0f, 40.0f);

	// Переменные спринта, ходьбы и прицеливания 
	
	// Добаввление переменной для ENUM
	// По умолчанию она будет Run. Чтобы использовать ее в BP
	// необходим так же макрос
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	EMovementState MovementState = EMovementState::Run_State;
	
	// Добавление переменной для STRUCT из Type.h
	// Ранее в Type.h мы создавали структуру FCharracterrSpeed.
	// Её необходимо указать здесь и назвать переменную MovementInfo
	// Инициализация не нужна,т.к. она есть в Type.h
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FCharacterSpeed MovementInfo;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool SprintRunEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool WalkEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool AimEnabled = false;

	// переменная таймера спринта
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	int SprintTime = 0;

	// Оружие, которым владеет наш персонаж	
	AWeaponDefault* CurrentWeapon = nullptr;
	//for demo 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo")
		FName InitWeaponName;
	// Cursor
	UDecalComponent* CurrentCursor = nullptr;
	


	// Описание инпутов вверх-вниз
	UFUNCTION()
	void InputAxisX(float Value);
    // Описание инпутов вправо-влево
	UFUNCTION()
	void InputAxisY(float Value);
	// Инпуты, отвечающие за стрельбу левой кнопкой мыши
	UFUNCTION()
	void InputAttackPressed();
	UFUNCTION()
	void InputAttackReleased();

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

	// Функция, которую вызывают инпуты стрельюы
	UFUNCTION(BlueprintCallable)
	void AttackCharEvent(bool bIsFiring);
	//CharacterUpdate
	// С помощью неё будем менять состояние персонажа
	// Например сост. прицеливания - меняем скорость
	UFUNCTION(BlueprintCallable)
	void CharacterUpdate();
	// ChangeMovementState будет принимать наш EMovementState
	// для этого подключим Type.h файл.
	//UFUNCTION(BlueprintCallable)
	//void ChangeMovementState(EMovementState NewMovementState);
	UFUNCTION(BlueprintCallable)
    void ChangeMovementState();
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool ToggleMouseInput = true;

	// Функция, чтобы нельзя было получать доступ напрямую из BP
	UFUNCTION(BlueprintCallable)
	AWeaponDefault* GetCurrentWeapon();
	 
	// Функция, которая спавнит оружие
	UFUNCTION(BlueprintCallable)
	void InitWeapon(FName IdWeapon);
	UFUNCTION(BlueprintCallable)
	void TryReloadEvent();

	// Функции делегатов перезарядки
	UFUNCTION()
		void WeaponReloadStart(UAnimMontage* Anim);
	UFUNCTION()
		void WeaponReloadEnd();
	// Делегаты, которые видны в BP
	UFUNCTION(BlueprintNativeEvent)
		void WeaponReloadStart_BP(UAnimMontage* Anim);
	UFUNCTION(BlueprintNativeEvent)
		void WeaponReloadEnd_BP();

	UFUNCTION(BlueprintCallable)
	UDecalComponent* GetCursorToWorld();
};

