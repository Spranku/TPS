// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include <Kismet/GameplayStatics.h> 
#include <Kismet/KismetMathLibrary.h>
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
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATPSCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	// ����� ������� � ���������� DeltaSeconds (Tick)
	MovementTick(DeltaSeconds);
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
}

void ATPSCharacter::InputAxisX(float Value)
{
	AxisX = Value;
}

void ATPSCharacter::InputAxisY(float Value)
{
	AxisY = Value;
}

void ATPSCharacter::MovementTick(float DeltaTime)
{
	// Up-Down movement
	AddMovementInput(FVector(1.0, 0.0, 0.0) , AxisX);

	// Left-Rigth movement
	AddMovementInput(FVector(0.0, 1.0, 0.0) , AxisY);

	// ������ �������� ����� � �������
	// �������������� ���������� ���������� Kismet/GameplayStatics
	// ������� ��������� ���� Controller, �� GameplayStatics
	// ����� ���������� ������, �������� � �������� ���������
	// GetWorld � ������� ������ �����������
	APlayerController* myController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	// ��������
	// ������ � ���������� ����� ������� ������� GetHitResult
	if (myController)
	{
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

		FHitResult ResultHit;
		myController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery6, false, ResultHit);
		

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
		float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
		SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
	}
}

// �������, ����������� ������ �������� ���������
void ATPSCharacter::CharacterUpdate()
{
	// ������� ���������� �������������� ��������
	// c ��������� ����� ����� 600
	// ��� ����������� ��� switch: ����� ResSpeed
	// ��������� �� MovementInfo, ���� ���� ����������
	// ������ ���������
	float ResSpeed = 600.0f;
	
	// ����� ����������� ���������� MovementState, ����� switch
	// ��������� ��������� � ������� �����������
	switch (MovementState)
	{
	case EMovementState::Aim_State:
		ResSpeed = MovementInfo.AimSpeed;
		break;
	case EMovementState::Walk_State:
		ResSpeed = MovementInfo.WalkSpeed;
		break;
	case EMovementState::Run_State:
		ResSpeed = MovementInfo.RunSpeed;
		break;
	default:
		break;
	}

	// ����� ����������� ����������, ����� CharacterMovementComponent
	// � ��������������� ������������ �������� ������ �����������������
	// ���������.
	GetCharacterMovement()->MaxWalkSpeed = ResSpeed;
}

void ATPSCharacter::ChangeMovementState(EMovementState NewMovementState)
{
	// � ���� ������� �������:
	// ����� ������� ����������, �� ��������� ��������� ����� ��������
	MovementState = NewMovementState;

	// ����� ����� ���������� ������� CharacterUpdate
	CharacterUpdate();
}

