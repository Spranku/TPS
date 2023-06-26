// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <TPS/FunctionLibrary/Type.h>
#include "Game/WeaponDefault.h"
#include </My_Projects/TPS/Source/TPS/Character/TPSCharacterHealthComponent.h>
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

	// ����� �������� ������ �������������, ��� ������
	FTimerHandle TimerHandle_RagDollTimer;


	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	// ��������������� �������, ������� ��������� �������������� ��� ������
	virtual void SetupPlayerInputComponent(class UInputComponent* NewInputComponent) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	//FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
		class UTPSInventoryComponent* InventoryComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
		class UTPSCharacterHealthComponent* CharHealthComponent;


private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// ��������� ���������
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//	class UTPSInventoryComponent* InventoryComponent;

	/** A decal that projects to the cursor location. */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//class UDecalComponent* CursorToWorld;
public:
	// Cursor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	UMaterialInterface* CursorMaterial = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
   	FVector CursorSize = FVector(20.0f, 40.0f, 40.0f);

	// ���������� �������, ������ � ������������ 
	
	// ����������� ���������� ��� ENUM
	// �� ��������� ��� ����� Run. ����� ������������ �� � BP
	// ��������� ��� �� ������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	EMovementState MovementState = EMovementState::Run_State;
	
	// ���������� ���������� ��� STRUCT �� Type.h
	// ����� � Type.h �� ��������� ��������� FCharracterrSpeed.
	// Ÿ ���������� ������� ����� � ������� ���������� MovementInfo
	// ������������� �� �����,�.�. ��� ���� � Type.h
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FCharacterSpeed MovementInfo;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool SprintRunEnabled = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool WalkEnabled = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool AimEnabled = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool bIsAlive = true;
	// ������ � ���������� ������� 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		TArray<UAnimMontage*> DeadsAnim;

	// ���������� ������� �������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	int SprintTime = 0;

	// ������, ������� ������� ��� ��������	
	AWeaponDefault* CurrentWeapon = nullptr;
	//for demo 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo")
		FName InitWeaponName;
	// Cursor
	UDecalComponent* CurrentCursor = nullptr;

	// ������ �������� ������
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		int32 CurrentIndexWeapon = 0;

	// �������� ������� �����-����
	UFUNCTION()
		void InputAxisX(float Value);
    // �������� ������� ������-�����
	UFUNCTION()
		void InputAxisY(float Value);
	// ������, ���������� �� �������� ����� ������� ����
	UFUNCTION()
		void InputAttackPressed();
	UFUNCTION()
		void InputAttackReleased();
	UFUNCTION()
		void TrySwitchNextWeapon();
	UFUNCTION()
		void SwitchPreviosWeapon();

	// ���������� ��� �������
	float AxisX = 0.0f;
	float AxisY = 0.0f;

	// Tick Function
	// ����� ����� ���������� Add Movement Input
	// ����������� �������� � � �������� Tick
	UFUNCTION()
	void MovementTick(float DeltaTime);

	// �������, ������� �������� ������ ��������
	UFUNCTION(BlueprintCallable)
	void AttackCharEvent(bool bIsFiring);
	//CharacterUpdate
	// � ������� �� ����� ������ ��������� ���������
	// �������� ����. ������������ - ������ ��������
	UFUNCTION(BlueprintCallable)
	void CharacterUpdate();
	// ChangeMovementState ����� ��������� ��� EMovementState
	// ��� ����� ��������� Type.h ����.
	//UFUNCTION(BlueprintCallable)
	//void ChangeMovementState(EMovementState NewMovementState);
	UFUNCTION(BlueprintCallable)
    void ChangeMovementState();
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool ToggleMouseInput = true;

	// �������, ����� ������ ���� �������� ������ �������� �� BP
	UFUNCTION(BlueprintCallable)
	AWeaponDefault* GetCurrentWeapon();
	 
	// �������, ������� ������� ������
	UFUNCTION(BlueprintCallable)
	void InitWeapon(FName IdWeaponName, FAdditionalWeaponInfo WeaponAdditionalInfo,int32 NewCurrentIndexWeapon);
	UFUNCTION(BlueprintCallable)
	void TryReloadEvent();

	// ������� ��������� �����������
	UFUNCTION()
		void WeaponReloadStart(UAnimMontage* Anim);
	UFUNCTION()
		void WeaponReloadEnd(bool bIsSuccess,int32 AmmoSafe);
	// ��������, ������� ����� � BP
	UFUNCTION(BlueprintNativeEvent)
		void WeaponReloadStart_BP(UAnimMontage* Anim);
	UFUNCTION(BlueprintNativeEvent)
		void WeaponReloadEnd_BP(bool bIsSuccess);
	UFUNCTION()
		void WeaponFireStart(UAnimMontage* Anim);
	UFUNCTION(BlueprintNativeEvent)
		void WeaponFireStart_BP(UAnimMontage* Anim);

	UFUNCTION(BlueprintCallable)
	UDecalComponent* GetCursorToWorld();

	// �-� ���������� ���������� ������ 
	UFUNCTION(BlueprintCallable)
	void CharDead();
	void EnableRagDoll();
	virtual float TakeDamage(float DamageAmount, 
		struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, 
		AActor* DamageCauser) override;
};

