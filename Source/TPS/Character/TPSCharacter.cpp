// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h" // for cursor?
#include "Materials/Material.h"
#include <Kismet/GameplayStatics.h> 
#include <Kismet/KismetMathLibrary.h>
#include "/My_Projects/TPS/Source/TPS/Game/TPSGameInstance.h"
#include "/My_Projects/TPS/Source/TPS/Character/TPSInventoryComponent.h"
#include <Game/ProjectileDefault.h>
#include </My_Projects/TPS/Source/TPS/TPS.h>
#include "/UE/UE_5.0/Engine/Source/Runtime/Engine/Public/Net/UnrealNetwork.h"
#include <PhysicalMaterials/PhysicalMaterial.h>
#include "Engine/World.h"

ATPSCharacter::ATPSCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	//CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->TargetArmLength = 800.f;
	//CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	 //Create a decal in the world to show the cursor's location
	//CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	//CursorToWorld->SetupAttachment(RootComponent);
	//static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/Blueprint/Character/M_Cursor_Decal.M_Cursor_Decal'"));
	//if (DecalMaterialAsset.Succeeded())
	//{
	//	CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	//}
	//CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	//CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	InventoryComponent = CreateDefaultSubobject<UTPSInventoryComponent>(TEXT("InventoryComponent"));
	CharHealthComponent = CreateDefaultSubobject<UTPSCharacterHealthComponent>(TEXT("HealthComponent"));

	if (CharHealthComponent)
	{
		CharHealthComponent->OnDead.AddDynamic(this, &ATPSCharacter::CharDead);
	}

	if (InventoryComponent)
	{
		InventoryComponent->OnSwitchWeapon.AddDynamic(this, &ATPSCharacter::InitWeapon);
	}

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	//NetWork
	bReplicates = true;
}

void ATPSCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	//if (CursorToWorld != nullptr) //Remove Epic Code 
	//{
	//	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	//	{
	//		if (UWorld* World = GetWorld())
	//		{
	//			FHitResult HitResult;
	//			FCollisionQueryParams Params(NAME_None, FCollisionQueryParams::GetUnknownStatId());
	//			FVector StartLocation = TopDownCameraComponent->GetComponentLocation();
	//			FVector EndLocation = TopDownCameraComponent->GetComponentRotation().Vector() * 2000.0f;
	//			Params.AddIgnoredActor(this);
	//			World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);
	//			FQuat SurfaceRotation = HitResult.ImpactNormal.ToOrientationRotator().Quaternion();
	//			CursorToWorld->SetWorldLocationAndRotation(HitResult.Location, SurfaceRotation);
	//		}
	//	}
	//	else if ((APlayerController* PC = Cast<APlayerController>(GetController()))
	//	{
	//		FHitResult TraceHitResult;
	//		PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);			
	//		FVector CursorFV = TraceHitResult.ImpactNormal;
	//		FRotator CursorR = CursorFV.Rotation();
	//		CursorToWorld->SetWorldLocation(TraceHitResult.Location);
	//		CursorToWorld->SetWorldRotation(CursorR);			
	//	}
	//}

	if (CurrentCursor)
	{
		APlayerController* myPC = Cast<APlayerController>(GetController());
		if (myPC && myPC->IsLocalPlayerController())
		{
			FHitResult TraceHitResult;
			myPC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();

			CurrentCursor->SetWorldLocation(TraceHitResult.Location);
			CurrentCursor->SetWorldRotation(CursorR);
		}
	}


	// ����� ������� � ���������� DeltaSeconds (Tick)
	MovementTick(DeltaSeconds);
	
}

void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld() && GetWorld()->GetNetMode() != NM_DedicatedServer)
	{
		if (CursorMaterial && GetLocalRole() == ROLE_AutonomousProxy || GetLocalRole() == ROLE_Authority)
		{
			CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
		}
	}

	// ????????????????????????????????????????????????????????????????????????????????
	// ????????????????????????????????????????????????????????????????????????????????
	// InitWeapon(InitWeaponName);


}

void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* NewInputComponent)
{
	Super::SetupPlayerInputComponent(NewInputComponent);

	// InputComponent - ��� Axis,������ ����� � ��������� Input`a
	// ������� ���� ��������� this
	// ����� &ATPSCharacter:: � �������, ������� ����� ���������� ��� �������
	// ��� �� ��� ������ ������-�����.
	NewInputComponent->BindAxis(TEXT("MoveForward"), this, &ATPSCharacter::InputAxisX);
	NewInputComponent->BindAxis(TEXT("MoveRight"), this, &ATPSCharacter::InputAxisY);

	NewInputComponent->BindAction(TEXT("FireEvent"), EInputEvent::IE_Pressed, this, &ATPSCharacter::InputAttackPressed);
	NewInputComponent->BindAction(TEXT("FireEvent"), EInputEvent::IE_Released, this, &ATPSCharacter::InputAttackReleased);

	NewInputComponent->BindAction(TEXT("ReloadEvent"), EInputEvent::IE_Released, this, &ATPSCharacter::TryReloadWeapon);
	
	NewInputComponent->BindAction(TEXT("SwitchNextWeapon"), EInputEvent::IE_Pressed, this, &ATPSCharacter::TrySwitchNextWeapon);
	NewInputComponent->BindAction(TEXT("SwitchPreviosWeapon"), EInputEvent::IE_Pressed, this, &ATPSCharacter::SwitchPreviosWeapon);

	NewInputComponent->BindAction(TEXT("AbilityAction"), EInputEvent::IE_Pressed, this, &ATPSCharacter::TryAbilityEnabled);

	TArray<FKey> HotKeys;
	HotKeys.Add(EKeys::One);
	HotKeys.Add(EKeys::Two);
	HotKeys.Add(EKeys::Three);
	HotKeys.Add(EKeys::Four);
	HotKeys.Add(EKeys::Five);
	HotKeys.Add(EKeys::Six);
	HotKeys.Add(EKeys::Seven);
	HotKeys.Add(EKeys::Eight);
	HotKeys.Add(EKeys::Nine);
	HotKeys.Add(EKeys::Zero);

	NewInputComponent->BindKey(HotKeys[1], IE_Pressed, this, &ATPSCharacter::TKeyPressed<1>);
	NewInputComponent->BindKey(HotKeys[2], IE_Pressed, this, &ATPSCharacter::TKeyPressed<2>);
	NewInputComponent->BindKey(HotKeys[3], IE_Pressed, this, &ATPSCharacter::TKeyPressed<3>);
	NewInputComponent->BindKey(HotKeys[4], IE_Pressed, this, &ATPSCharacter::TKeyPressed<4>);
	NewInputComponent->BindKey(HotKeys[5], IE_Pressed, this, &ATPSCharacter::TKeyPressed<5>);
	NewInputComponent->BindKey(HotKeys[6], IE_Pressed, this, &ATPSCharacter::TKeyPressed<6>);
	NewInputComponent->BindKey(HotKeys[7], IE_Pressed, this, &ATPSCharacter::TKeyPressed<7>);
	NewInputComponent->BindKey(HotKeys[8], IE_Pressed, this, &ATPSCharacter::TKeyPressed<8>);
	NewInputComponent->BindKey(HotKeys[9], IE_Pressed, this, &ATPSCharacter::TKeyPressed<9>);
	NewInputComponent->BindKey(HotKeys[0], IE_Pressed, this, &ATPSCharacter::TKeyPressed<0>);
}

void ATPSCharacter::InputAxisX(float Value)
{
	AxisX = Value;
}

void ATPSCharacter::InputAxisY(float Value)
{
	AxisY = Value;
}

void ATPSCharacter::InputAttackPressed()
{
	if (bIsAlive)
	{
		AttackCharEvent(true);
	}
}

void ATPSCharacter::InputAttackReleased()
{
	AttackCharEvent(false);
}

void ATPSCharacter::TrySwitchNextWeapon()
{
	// ����� �������� �������� ������� ������ �� �����-�� ������,��������� ���� �� ����-�� 2 �����
	// � ���������. ���� ����, �� ��������� ��������� ���������� � ���� ������� �������
	// 
	//
	if (CurrentWeapon && !CurrentWeapon->WeaponReloading && InventoryComponent->WeaponSlots.Num() > 1)
	{
		int8 OldIndex = CurrentIndexWeapon;
		FAdditionalWeaponInfo OldInfo;
		if (CurrentWeapon)
		{
			// ����� ������� ������
			// ��������� ���� � ��������� ���������� (������� �������� ��� ����)
			// ���� ������ ��������������, �� ������ ������ �����������
			OldInfo = CurrentWeapon->AdditionalWeaponInfo;
			if (CurrentWeapon->WeaponReloading)
				CurrentWeapon->CancelReload();
		}
		if (InventoryComponent)
		{
			// ����������, ��� ���� InventoryComponent
			// 
			//
			if(InventoryComponent->SwitchWeaponToIndexByNextPreviosIndex(CurrentIndexWeapon +1, OldIndex, OldInfo,true))
			{ }
		}
	}

}

void ATPSCharacter::SwitchPreviosWeapon()
{
	//	�� �� �����,������ ������ - 1
	//
	if (CurrentWeapon && !CurrentWeapon->WeaponReloading && InventoryComponent->WeaponSlots.Num() > 1)
	{
		int8 OldIndex = CurrentIndexWeapon;
		FAdditionalWeaponInfo OldInfo;
		if (CurrentWeapon)
		{
			OldInfo = CurrentWeapon->AdditionalWeaponInfo;
			if (CurrentWeapon->WeaponReloading)
				1 + 1;
				//CurrentWeapon->CancelReload();
		}
		if (InventoryComponent)
		{
			if (InventoryComponent->SwitchWeaponToIndexByNextPreviosIndex(CurrentIndexWeapon - 1, OldIndex, OldInfo,false))
			{
			}
		}
	}
}

void ATPSCharacter::TryAbilityEnabled()
{
	if (AbilityEffect)
	{
		UTPS_StateEffect* NewEffect = NewObject<UTPS_StateEffect>(this, AbilityEffect);
		// �������� � �������������
		if (NewEffect)
		{
			NewEffect->InitObject(this,NAME_None);
		}
	}
}

void ATPSCharacter::MovementTick(float DeltaTime)
{
	if (bIsAlive)
	{
		if (GetController () && GetController()->IsLocalPlayerController())
		{
			// Up-Down movement
			AddMovementInput(FVector(1.0, 0.0, 0.0), AxisX);
			// Left-Rigth movement
			AddMovementInput(FVector(0.0, 1.0, 0.0), AxisY);

			FString SEnum = UEnum::GetValueAsString(GetMovementState());
			UE_LOG(LogTPS_Net, Warning, TEXT("MovementState - %s"), *SEnum);

			// ������ �������� ����� � �������
			// �������������� ���������� ���������� Kismet/GameplayStatics
			// ������� ��������� ���� Controller, �� GameplayStatics
			// ����� ���������� ������, �������� � �������� ���������
			// GetWorld � ������� ������ �����������
			// APlayerController* myController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			// ��������
			// ������ � ���������� ����� ������� ������� GetHitResult
			// if (myController && ToggleMouseInput)
			// {
				// ��� ������� ��������� �������� ETraceTypeQuery
				// �� ���� ��� ����� ������, ������� � �++ �������� ���:
				// TraceTypeQuery1 UMETA (Hidden)
				// TraceTypeQuery2 UMETA (Hidden)
				// TraceTypeQuery3 UMETA (Hidden)
				// ...
				// ��� ������, ������� �� �������� �������, ���������� � 6��!
				// ������� �������� ������ �����. ��� � ���� LanscapeCursor
				// 
				// ������ �������� HitResult ���������� ��������, ������� 
				// ����� ������� �������� ���������� ����� ���� � �������� 
				// � � �������� ���������

			// 	FHitResult ResultHit;
			// 	myController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery6, false, ResultHit);

			// ����� ����� ��������� FindLookAtRotation, ����� ���������� 
				// ���������� <Kismet/KismetMathLibrary.h>. 
				// �������� Kismet ��� ����, ����� �������� � ��� � Blueprints
				// ������ �������� - �������������� ����� 
				// ������ �������� - ResultHit
				// ������ �������� - �������, ���� ������� ��������. 
				// UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location);
				// 
				// ����� �������� ������� SetActorRotation.
				// ��� ������� ��������� ����������.��� ������� � 4 �����������.
				// FQuat(X,Y,Z,W). ���� ���������� ��������� ������� FQuat,
				// �� ����� ������� ��� �� ����� ������������ ������� Rotator,
				// ��� ��� � �����. ���������� � ���������� Rotator. � ���� 
				// ��� ���������: InPitch/InYaw/InRoll. Pitch � Roll �� �����,
				// �� ����� Yaw (Z). ����� ��� ������� �� ����� ��������� ����������
				// ������ � ����� ����� ������� Yaw,�.�. FindLookAtRotation 
				// ���������� Rotation
				// 
				// SetActorRotation(FQuat(FRotator(0.0f, UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw, 0.0f)));
				// 
				// ����� ��������� ����� ������, ���������� ������� ���������� � ����� float � ��������� �� ������ c Yaw:
			// 	float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
			// 	SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
			// }

			// ����� ���� �������� ������. � ����� �������� ������� � �������� ���������.
			// ������ ��� TraceQuert6,������ ECC_GameTraceChannel1. ������ ������ ����� �������������� ���������

			if (MovementState == EMovementState::SprintRun_State)
			{
				FVector myRotationVector = FVector(AxisX, AxisY, 0.0f);
				FRotator myRotator = myRotationVector.ToOrientationRotator();
				SetActorRotation((FQuat(myRotator)));

				SetActorRotationByYaw_OnServer(myRotator.Yaw);
			}
			else
			{
				APlayerController* myController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
				if (myController)
				{
					FHitResult ResultHit;
					myController->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);

					float FindRotaterResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
					SetActorRotation(FQuat(FRotator(0.0f, FindRotaterResultYaw, 0.0f)));
					SetActorRotationByYaw_OnServer(FindRotaterResultYaw);


					//
					if (CurrentWeapon)
					{
						FVector Displacement = FVector(0);
						bool bIsReduceDispersion = false;
						switch (MovementState)
						{
						case EMovementState::Aim_State:
							// ����������� - ������ ��������. �������� - ������������� �� 1
							// � ����� ������ ����,�� �������� ������� �����������
							Displacement = FVector(0.0f, 0.0f, 160.0f);
							bIsReduceDispersion = true;
							//CurrentWeapon->ShouldReduseDispersion = true;
							break;
						case EMovementState::AimWalk_State:
							//CurrentWeapon->ShouldReduseDispersion = true;
							bIsReduceDispersion = true;
							Displacement = FVector(0.0f, 0.0f, 160.0f);
							break;
						case EMovementState::Walk_State:
							Displacement = FVector(0.0f, 0.0f, 120.0f);
							//CurrentWeapon->ShouldReduseDispersion = false;
							break;
						case EMovementState::Run_State:
							Displacement = FVector(0.0f, 0.0f, 120.0f);
							//CurrentWeapon->ShouldReduseDispersion = false;
							break;
						case EMovementState::SprintRun_State:
							break;
						default:
							break;
						}
						// � ��� ���� ��������� ��������� ������� �� ����������� (ResultHit)
						// � ������ �������� ���� ���������
						//CurrentWeapon->ShootEndLocation = ResultHit.Location + Displacement;
						CurrentWeapon->UpdateWeaponByCharacterMovementStateOnServer(ResultHit.Location + Displacement, bIsReduceDispersion);
					}
				}
			}
		}
		
	}	
	// ���� � ��������� ���� ������ � ���� �������� ��������� = 0,
	// �� ������ ��������� �������� ��������
	// ���� �������� �����, �� �������� �������������
	// if (CurrentWeapon)
	//	if (FMath::IsNearlyZero(GetVelocity().Size(), 0.5f))
	//		CurrentWeapon->ShouldReduseDespersion = true;
	//	else
	//		CurrentWeapon->ShouldReduseDespersion = false;
	// � ����� ������� ��� ������ ������

}

void ATPSCharacter::AttackCharEvent(bool bIsFiring)
{
	// ������� ��������� ����������
	// �������� �������, ������� ���������� ������, ������� ��������� � ��� � �����
	AWeaponDefault* myWeapon = nullptr;
	myWeapon = GetCurrentWeapon();
	if (myWeapon)
	{
		// ������������� ������ � ����� ������,��� ��� ��������� � ��������� �������� ���� �� ��������
		// (������ ���� ������� �����)
		// 
		//ToDo Check melee or range
		myWeapon->SetWeaponStateFire_OnServer(bIsFiring);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("ATPSCharacter::AttackCharEvent - CurrentWeapon -NULL"));
	// ����� CurrentWeapon -NULL
}

// �������, ����������� ������ �������� ���������
void ATPSCharacter::CharacterUpdate()
{
	// ������� ���������� �������������� ��������
	// c ��������� ����� ����� 600
	// ��� ����������� ��� switch: ����� ResSpeed
	// �������� �� MovementInfo, ���� ���� ����������
	// ������ ���������
	float ResSpeed = 600.0f;
	
	// ����� ����������� ���������� MovementState, ����� switch
	// ��������� ��������� � ������� �����������
	switch (MovementState)
	{
	case EMovementState::Aim_State:
		ResSpeed = MovementInfo.AimSpeed;
		break;
	case EMovementState::AimWalk_State:
		ResSpeed = MovementInfo.AimWalkSpeed;
		break;
	case EMovementState::Walk_State:
		ResSpeed = MovementInfo.WalkSpeed;
		break;
	case EMovementState::Run_State:
		ResSpeed = MovementInfo.RunSpeed;
		break;
	case EMovementState::SprintRun_State:
		ResSpeed = MovementInfo.SprintRunSpeed;
		break;
	default:
		break;
	}

	// ����� ����������� ����������, ����� CharacterMovementComponent
	// � ��������������� ������������ �������� ������ �����������������
	// ���������.
	GetCharacterMovement()->MaxWalkSpeed = ResSpeed;
}

void ATPSCharacter::ChangeMovementState()
{
	EMovementState NewState = EMovementState::Run_State;

	if (!WalkEnabled && !SprintRunEnabled && !AimEnabled)
	{
		// ��������� �������� ���� ����� �������
		//ToggleMouseInput = true;
		NewState = EMovementState::Run_State;
	}
	else
	{
		if (SprintRunEnabled )//&& AxisX) // ������ AxisX ��� ����� ���������� ���������� �������
		{
			// ���������� �������� ����
			//ToggleMouseInput = false;
			WalkEnabled = false;
			AimEnabled = false;
			NewState = EMovementState::SprintRun_State;
		}
		else
		{
			if (WalkEnabled && !SprintRunEnabled && AimEnabled)
			{
				NewState = EMovementState::AimWalk_State;
			}

			else
			{
				if (WalkEnabled && !SprintRunEnabled && !AimEnabled)
				{
					NewState = EMovementState::Walk_State;
				}
				else
				{
					if (!WalkEnabled && !SprintRunEnabled && AimEnabled)
					{
						NewState = EMovementState::Aim_State;
					}
				}
			}
		}
	}

	SetMovementState_OnServer(NewState);

	//CharacterUpdate();

	//Weapon state update
	AWeaponDefault* myWeapon = GetCurrentWeapon();
	if (myWeapon)
	{
		myWeapon->UpdateStateWeapon_OnServer(NewState);
	}
}

// �������, ����� ������ ���� �������� ������ �������� �� BP � ���� ����������
AWeaponDefault* ATPSCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

EMovementState ATPSCharacter::GetMovementState()
{
	return MovementState;
}

int32 ATPSCharacter::GetCurrentWeaponIndex()
{
	return CurrentIndexWeapon;
}

bool ATPSCharacter::GetIsAlive()
{
	return bIsAlive;
}

// ��� ������� ������ ����� ��������� ID ������
void ATPSCharacter::InitWeapon(FName IdWeaponName, FAdditionalWeaponInfo WeaponAdditionalInfo, int32 NewCurrentIndexWeapon)
{
	//OnServer
	
	// �������� ���� �� �������� ������
	// ������ ���������� ������� ������ � ����� � ������ ���������� ������
	// ���� ���������� ����������� (CurrentWeapon = myWeapon)
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}
	// ����� ����� ���� ������� � ������� ���� �� ����
	// ����� ����� ���������� ��� �-�, ����� ������ ��� BP_GameInstance 
	UTPSGameInstance* myGI = Cast<UTPSGameInstance>(GetGameInstance());
	FWeaponInfo myWeaponInfo;
	if (myGI)
	{
		// ���� ��� ������ ������, �� �� �������� � ���� ��������
	    // ������� GetWeaponInfoByName(ID Weapon) � ���� �����
		// ����� �������� ��������� FWeaponInfo � ��� ��������� ����������
		// �� ������ ���� �������
		//
		// ���� �-� GeTWeaponInfoByName ���������� ������, �� ��������� MyWeaponInfo � ��� ����
		if (myGI->GetWeaponInfoByName(IdWeaponName, myWeaponInfo))
		{
			// ���������, ��� ���������� �� ������
			// ����� �� ���������� ���-�� ����������.
			if ((myWeaponInfo.WeaponClass))
			{
				// ������� ���������� ��� ������, ������� ����� ������������ ���� � ����� ������
				FVector SpawnLocation = FVector(0);
				FRotator SpawnRotation = FRotator(0);

				// ������� ����� ���������, ������� ������� � ���,��� �� ������ ���������� ������ 
				// ��������� ��������� � ������� ���������� 
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Owner = this;           // ������������� ���
				SpawnParams.Instigator = GetInstigator(); // TPSCharacter

				// ����� ���� ������� GetWold()->SpawnActor, ������� ��������� ���������
				// UCLASS,location,rotation,SpawnParams � �� ������ � ��� ���������� AActor
				// ����� (���� ������ ������) �� �������� ���� AActor ��������� �� ��� ����� ����� AWeaponDefault
				// ���� ���� �������� �������, �� ������������ � ���������� myWeapon,������� ������� ����� �������� 
				AWeaponDefault* myWeapon = Cast<AWeaponDefault>(GetWorld()->SpawnActor(myWeaponInfo.WeaponClass, &SpawnLocation, &SpawnRotation, SpawnParams));
				if (myWeapon)
				{
					// ����� ��������, ��� ���������� ���������, �� ������� � � ������ ���� 
					// ����� ������� ������ � ��������� �������� ������, ���� ����� ����������
					//
					FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
					myWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));
					// ����� ����� ���������� � ���������� ������, ������� ������� ��� ��������
					CurrentWeapon = myWeapon;
				
					// ��������� myWeaponInfo �������������� ������
					//myWeapon->WeaponInit(myWeaponInfo);
					myWeapon->WeaponSetting = myWeaponInfo;
					
					// What is this?
					/*----------------------------------------*/
					// myWeapon->ReloadTime = myWeaponInfo.ReloadTime;
					// myWeapon->UpdateStateWeapon(MovementState);
					/*----------------------------------------*/
					myWeapon->UpdateStateWeapon_OnServer(MovementState);
					myWeapon->AdditionalWeaponInfo = WeaponAdditionalInfo;
					CurrentIndexWeapon = NewCurrentIndexWeapon;
				
					// ����� ����. ������, ����� �������� ��������
					// ������ �������� �������� �� ������ � �����  �����������
					// ����� �������� AddDynamic,� �������� ���� ������ �� ����,
					// ������ ��� ��������� ��������? Character. ����� ���, ��� �������
					// �������� �������. � ��� ������ WeaponReloadStart ��� End
					myWeapon->OnWeaponReloadStart.AddDynamic(this, &ATPSCharacter::WeaponReloadStart);
					myWeapon->OnWeaponReloadEnd.AddDynamic(this, &ATPSCharacter::WeaponReloadEnd);
					myWeapon->OnWeaponFireStart.AddDynamic(this, &ATPSCharacter::WeaponFireStart);

					// ����� ������� ������������� �� ������, � � �� ��� ��������,
					// � ������ ���������� ����� ����. ����������� (���� ��� ����� �������)
					// � ���� ������� � ������ = 0
					//
					if (CurrentWeapon->GetWeaponRound() <= 0 && CurrentWeapon->CheckCanWeaponReload())
						CurrentWeapon->InitReload();

					// ��������,��� ������ �������� � � ������ ���������� ��������
					if(InventoryComponent)
						InventoryComponent->OnWeaponAmmoAviable.Broadcast(myWeapon->WeaponSetting.WeaponType);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("ATPSCharacter::InitWeapon - Weapon not found in table -NULL"));
			}
		}
	}
}

void ATPSCharacter::TryReloadWeapon()
{

	// ������ ��������� ��� � ��������� ���� ������� ������ � ��� �� ��������������
	if (bIsAlive && CurrentWeapon && !CurrentWeapon->WeaponReloading)
	{
		// ���� ������� ���-�� �������� ������? ��� ����� ������������� ����������,
		// �� �������� �-� ������������� �����������
		if (CurrentWeapon->GetWeaponRound() < CurrentWeapon->WeaponSetting.MaxRound && CurrentWeapon->CheckCanWeaponReload())
		{
			CurrentWeapon->InitReload();
		}
	}

}

void ATPSCharacter::WeaponReloadStart(UAnimMontage* Anim)
{
	//
	WeaponReloadStart_BP(Anim);
}

void ATPSCharacter::WeaponReloadEnd(bool bIsSuccess,int32 AmmoTake)
{
	// ����� �������� � ������ ������
	// ����� ������������ ������, ����� ������� ��� ������ � ������� ������ ����� ��������
	// �������� � ��������� ������� �������� �������� (AmmoSafe)
	if (InventoryComponent)
	{
		InventoryComponent->AmmoSlotChangeValue(CurrentWeapon->WeaponSetting.WeaponType,AmmoTake); 
		
		InventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->AdditionalWeaponInfo);
	}
	WeaponReloadEnd_BP(bIsSuccess);
}

void ATPSCharacter::WeaponReloadStart_BP_Implementation(UAnimMontage* Anim)
{
	//
	
}

void ATPSCharacter::WeaponReloadEnd_BP_Implementation(bool bIsSuccess)
{
	//
	
}

void ATPSCharacter::WeaponFireStart(UAnimMontage* Anim)
{
	if (InventoryComponent && CurrentWeapon)
	{
		InventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->AdditionalWeaponInfo);
	}
	WeaponFireStart_BP(Anim);
}

void ATPSCharacter::WeaponFireStart_BP_Implementation(UAnimMontage* Anim)
{
	// in BP
}

bool ATPSCharacter::TrySwitchWeaponToIndexByKeyInput(int32 ToIndex)
{
	bool bIsSuccess = false;

	if (CurrentWeapon && !CurrentWeapon->WeaponReloading && InventoryComponent->WeaponSlots.IsValidIndex(ToIndex))
	{
		if (CurrentIndexWeapon != ToIndex && InventoryComponent)
		{
			int32 OldIndex = CurrentIndexWeapon;
			FAdditionalWeaponInfo OldInfo;

			if (CurrentWeapon)
			{
				OldInfo = CurrentWeapon->AdditionalWeaponInfo;
				if (CurrentWeapon->WeaponReloading)
					CurrentWeapon->CancelReload();
			}
			bIsSuccess = InventoryComponent->SwitchWeaponByIndex(ToIndex, OldIndex, OldInfo);
		}
	}
	return false;
}

UDecalComponent* ATPSCharacter::GetCursorToWorld()
{
	return CurrentCursor;
}

// ���������� ����������
EPhysicalSurface ATPSCharacter::GetSurfaceType()
{
	EPhysicalSurface Result = EPhysicalSurface::SurfaceType_Default;
	if (CharHealthComponent)
	{ // �������� ���� � ��������� ��� ��� ����, �� ������� ������ ������������� �� �����
		if (CharHealthComponent->GetCurrentShield() <= 0)
		{
			if (GetMesh())
			{
				UMaterialInterface* myMaterial = GetMesh()->GetMaterial(0);
				if (myMaterial)
				{
					Result = myMaterial->GetPhysicalMaterial()->SurfaceType;
				}
			}
		}
	}
	return Result;
}

TArray<UTPS_StateEffect*> ATPSCharacter::GetAllCurrentEffects()
{
	return Effects;
}

void ATPSCharacter::RemoveEffect(UTPS_StateEffect* RemoveEffect)
{
	Effects.Remove(RemoveEffect);
}

void ATPSCharacter::AddEffect(UTPS_StateEffect* newEffect)
{
	Effects.Add(newEffect);
}

void ATPSCharacter::CharDead()
{
	float TimeAnim = 0.0f;
	// �-� ���������� ����� 0 � ������ ������� ���������
	int32 rnd = FMath::RandHelper(DeadsAnim.Num());
	if (DeadsAnim.IsValidIndex(rnd) && DeadsAnim[rnd] && GetMesh() && GetMesh()->GetAnimInstance())
	{
		TimeAnim = DeadsAnim[rnd]->GetPlayLength();
		GetMesh()->GetAnimInstance()->Montage_Play(DeadsAnim[rnd]);
	}
	bIsAlive = false;

	if (GetController())
	{
		GetController()->UnPossess();
	}

	UnPossessed();

	// ������ �� ����� ������������ �������� ������,����� ��� ��������� �-� ����
	GetWorldTimerManager().SetTimer(TimerHandle_RagDollTimer, this, &ATPSCharacter::EnableRagDoll, TimeAnim, false);

	//GetCursorToWorld()->SetVisibility(false); �������� � ������

	// �������� ����, �������� �� ��������
	AttackCharEvent(false);
	

	CharDead_BP();
}

void ATPSCharacter::EnableRagDoll()
{
	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetSimulatePhysics(true);
	}
}

float ATPSCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (bIsAlive)
	{
		//CharHealthComponent->ChangeCurrentHealth(-DamageAmount);
		CharHealthComponent->ChangeHealthValue(-DamageAmount);
	}

	// ���� ����� ��� �� RadialDamage �� ��������� �������� ������, ���� �� ����
	if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		AProjectileDefault* myProjectile = Cast<AProjectileDefault>(DamageCauser);
		if (myProjectile)
		{
			UType::AddEffecttBySurfaceType(this,NAME_None, myProjectile->ProjectileSetting.Effect, GetSurfaceType()); // To Do Name none - bone for radial damage
		}
	}
	return ActualDamage;
}

void ATPSCharacter::SetActorRotationByYaw_OnServer_Implementation(float Yaw)
{
	SetActorRotationByYaw_Multicast(Yaw);
}

void ATPSCharacter::SetActorRotationByYaw_Multicast_Implementation(float Yaw)
{
	if (Controller && !Controller->IsLocalPlayerController())
	{
		SetActorRotation(FQuat(FRotator(0.0f, Yaw, 0.0f)));
	}
}

void ATPSCharacter::SetMovementState_OnServer_Implementation(EMovementState NewState)
{
	SetMovementState_Multicast(NewState);
}

void ATPSCharacter::SetMovementState_Multicast_Implementation(EMovementState NewState)
{
	MovementState = NewState;
	CharacterUpdate();
}

void ATPSCharacter::CharDead_BP_Implementation()
{
	//BP
}

void ATPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATPSCharacter, MovementState);
	DOREPLIFETIME(ATPSCharacter, CurrentWeapon);
}





