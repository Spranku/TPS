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
		if (myPC)
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

	// ????????????????????????????????????????????????????????????????????????????????
	// ????????????????????????????????????????????????????????????????????????????????
	// InitWeapon(InitWeaponName);

	if (CursorMaterial)
	{
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
	}
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

	NewInputComponent->BindAction(TEXT("ReloadEvent"), EInputEvent::IE_Released, this, &ATPSCharacter::TryReloadEvent);
	
	NewInputComponent->BindAction(TEXT("SwitchNextWeapon"), EInputEvent::IE_Pressed, this, &ATPSCharacter::TrySwitchNextWeapon);
	NewInputComponent->BindAction(TEXT("SwitchPreviosWeapon"), EInputEvent::IE_Pressed, this, &ATPSCharacter::SwitchPreviosWeapon);

	NewInputComponent->BindAction(TEXT("AbilityAction"), EInputEvent::IE_Pressed, this, &ATPSCharacter::TryAbilityEnabled);
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
	AttackCharEvent(true);
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
	if (InventoryComponent->WeaponSlots.Num() > 1)
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
			if(InventoryComponent->SwitchWeaponToIndex(CurrentIndexWeapon +1, OldIndex, OldInfo,true))
			{ }
		}
	}

}

void ATPSCharacter::SwitchPreviosWeapon()
{
	//	�� �� �����,������ ������ - 1
	//
	if (InventoryComponent->WeaponSlots.Num() > 1)
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
			if (InventoryComponent->SwitchWeaponToIndex(CurrentIndexWeapon - 1, OldIndex, OldInfo,false))
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
			NewEffect->InitObject(this);
		}
	}
}

void ATPSCharacter::MovementTick(float DeltaTime)
{
	if (bIsAlive)
	{
		// Up-Down movement
		AddMovementInput(FVector(1.0, 0.0, 0.0), AxisX);
		// Left-Rigth movement
		AddMovementInput(FVector(0.0, 1.0, 0.0), AxisY);

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


				//
				if (CurrentWeapon)
				{
					FVector Displacement = FVector(0);
					switch (MovementState)
					{
					case EMovementState::Aim_State:
						// ����������� - ������ ��������. �������� - ������������� �� 1
						// � ����� ������ ����,�� �������� ������� �����������
						Displacement = FVector(0.0f, 0.0f, 160.0f);
						CurrentWeapon->ShouldReduseDespersion = true;
						break;
					case EMovementState::AimWalk_State:
						CurrentWeapon->ShouldReduseDespersion = true;
						Displacement = FVector(0.0f, 0.0f, 160.0f);
						break;
					case EMovementState::Walk_State:
						Displacement = FVector(0.0f, 0.0f, 120.0f);
						CurrentWeapon->ShouldReduseDespersion = false;
						break;
					case EMovementState::Run_State:
						Displacement = FVector(0.0f, 0.0f, 120.0f);
						CurrentWeapon->ShouldReduseDespersion = false;
						break;
					case EMovementState::SprintRun_State:
						break;
					default:
						break;
					}
					// � ��� ���� ��������� ��������� ������� �� ����������� (ResultHit)
					// � ������ �������� ���� ���������
					CurrentWeapon->ShootEndLocation = ResultHit.Location + Displacement;
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
		myWeapon->SetWeaponStateFire(bIsFiring);
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
	if (!WalkEnabled && !SprintRunEnabled && !AimEnabled)
	{
		// ��������� �������� ���� ����� �������
		//ToggleMouseInput = true;
		MovementState = EMovementState::Run_State;
	}
	else
	{
		if (SprintRunEnabled )//&& AxisX) // ������ AxisX ��� ����� ���������� ���������� �������
		{
			// ���������� �������� ����
			//ToggleMouseInput = false;
			WalkEnabled = false;
			AimEnabled = false;
			MovementState = EMovementState::SprintRun_State;
		}
		
		if (WalkEnabled && !SprintRunEnabled && AimEnabled)
		{
			MovementState = EMovementState::AimWalk_State;
		}

		else
		{
			if (WalkEnabled && !SprintRunEnabled && !AimEnabled)
			{
				MovementState = EMovementState::Walk_State;
			}
			else
			{
				if (!WalkEnabled && !SprintRunEnabled && AimEnabled)
				{
					MovementState = EMovementState::Aim_State;
				}
			}
		}
	}
	CharacterUpdate();
	//Weapon state update
	AWeaponDefault* myWeapon = GetCurrentWeapon();
	if (myWeapon)
	{
		myWeapon->UpdateStateWeapon(MovementState);
	}
}

// �������, ����� ������ ���� �������� ������ �������� �� BP � ���� ����������
AWeaponDefault* ATPSCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

// ��� ������� ������ ����� ��������� ID ������
void ATPSCharacter::InitWeapon(FName IdWeaponName, FAdditionalWeaponInfo WeaponAdditionalInfo, int32 NewCurrentIndexWeapon)
{
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
					myWeapon->UpdateStateWeapon(MovementState);
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

void ATPSCharacter::TryReloadEvent()
{

	// ������ ��������� ��� � ��������� ���� ������� ������ � ��� �� ��������������
	if (CurrentWeapon && !CurrentWeapon->WeaponReloading)
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

	UnPossessed();

	// ������ �� ����� ������������ �������� ������,����� ��� ��������� �-� ����
	GetWorldTimerManager().SetTimer(TimerHandle_RagDollTimer, this, &ATPSCharacter::EnableRagDoll, TimeAnim, false);

	//GetCursorToWorld()->SetVisibility(false); �������� � ������
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
			UType::AddEffecttBySurfaceType(this, myProjectile->ProjectileSetting.Effect, GetSurfaceType());
		}
	}
	return ActualDamage;
}






