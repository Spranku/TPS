// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <TPS/FunctionLibrary/Type.h>
#include "Game/WeaponDefault.h"
#include </My_Projects/TPS/Source/TPS/Character/TPSCharacterHealthComponent.h>
#include "/My_Projects/TPS/Source/TPS/Interface/TPS_IGameActor.h"
#include <TPS/Public/Game/TPS_StateEffect.h>
#include "TPSCharacter.generated.h"

//USTRUCT(BlueprintType)
//struct FCharacterSpeedInfo
//{
//	GENERATED_BODY()
//
//
//};

UCLASS(Blueprintable)
class ATPSCharacter : public ACharacter, public ITPS_IGameActor 
{
	GENERATED_BODY()
protected:
	bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
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
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	UPROPERTY(Replicated)
	EMovementState MovementState = EMovementState::Run_State;
	
	// ���������� ���������� ��� STRUCT �� Type.h
	// ����� � Type.h �� ��������� ��������� FCharacterSpeed.
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
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	//	bool bIsAlive = true; 
	
	// ������ � ���������� ������� 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		TArray<UAnimMontage*> DeadsAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
		TSubclassOf<UTPS_StateEffect> AbilityEffect;

	// ������ �������� 
	UPROPERTY(Replicated)
	TArray<UTPS_StateEffect*> Effects;
	UPROPERTY(ReplicatedUsing = EffectAdd_OnRep)
		UTPS_StateEffect* EffectAdd = nullptr;
	UPROPERTY(ReplicatedUsing = EffectRemove_OnRep)
		UTPS_StateEffect* EffectRemove = nullptr;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Debag")
		TArray<UParticleSystemComponent*> PartycleSystemEffects;



	// ���������� ������� �������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	int SprintTime = 0;

	// ������, ������� ������� ��� ��������	
	UPROPERTY(Replicated)
	AWeaponDefault* CurrentWeapon = nullptr;
	//for demo 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo")
		FName InitWeaponName;
	// Cursor
	UDecalComponent* CurrentCursor = nullptr;

	// ������ �������� ������
	UPROPERTY(Replicated /*BlueprintReadOnly, EditDefaultsOnly*/ )
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

	void TryAbilityEnabled();

	// ������ �������
	template<int32 Id>
	void TKeyPressed()
	{
		TrySwitchWeaponToIndexByKeyInput_OnServer(Id);
	}

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
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AWeaponDefault* GetCurrentWeapon();
	 
	// �������, ������� ������� ������
	UFUNCTION(BlueprintCallable)
	void InitWeapon(FName IdWeaponName, FAdditionalWeaponInfo WeaponAdditionalInfo,int32 NewCurrentIndexWeapon);
	UFUNCTION(BlueprintCallable)
	void TryReloadWeapon();

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
	UFUNCTION(Server, Reliable)
	void TrySwitchWeaponToIndexByKeyInput_OnServer(int32 ToIndex);



	UFUNCTION(BlueprintCallable)
		UDecalComponent* GetCursorToWorld();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		EMovementState GetMovementState();
	//UFUNCTION(BlueprintCallable, BlueprintPure)
	//	TArray<UTPS_StateEffect*> GetCurrentEffectsOnChar();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		int32 GetCurrentWeaponIndex();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool GetIsAlive();

	// Interface
	EPhysicalSurface GetSurfaceType() override;
	TArray<UTPS_StateEffect*> GetAllCurrentEffects() override;

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
		void RemoveEffect(UTPS_StateEffect* RemoveEffect);
	void RemoveEffect_Implementation(UTPS_StateEffect* RemoveEffect) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void AddEffect(UTPS_StateEffect* newEffect) ;
	void AddEffect_Implementation(UTPS_StateEffect* newEffect) override;
	// End Interface

	// �-� ���������� ���������� ������ 
	UFUNCTION(BlueprintCallable)
		void CharDead();
	UFUNCTION(NetMulticast, Reliable)
		void EnableRagdoll_Multicast();

	virtual float TakeDamage(float DamageAmount, 
		struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, 
		AActor* DamageCauser) override;

	UFUNCTION(BlueprintNativeEvent)
		void CharDead_BP();

	// Network
	UFUNCTION(Server, Unreliable)
		void SetActorRotationByYaw_OnServer(float Yaw);
	UFUNCTION(NetMulticast, Unreliable)
		void SetActorRotationByYaw_Multicast(float Yaw);

	UFUNCTION(Server, Reliable)
		void SetMovementState_OnServer(EMovementState NewState);
	UFUNCTION(NetMulticast, Reliable)
		void SetMovementState_Multicast(EMovementState NewState);

	UFUNCTION(Server, Reliable)
		void TryReloadWeapon_OnServer();
	UFUNCTION(NetMulticast, Reliable)
		void PlayAnim_Multicast(UAnimMontage* Anim);

	UFUNCTION()
		void EffectAdd_OnRep();
	UFUNCTION()
		void EffectRemove_OnRep();
	UFUNCTION(Server, Reliable)
		void ExecuteEffectAdded_OnServer(UParticleSystem* ExecuteFX);
	UFUNCTION(NetMulticast, Reliable)
		void ExecuteEffectAdded_Multicast(UParticleSystem* ExecuteFX);

	UFUNCTION()
		void SwitchEffect(UTPS_StateEffect* Effect, bool bIsAdd);
};

