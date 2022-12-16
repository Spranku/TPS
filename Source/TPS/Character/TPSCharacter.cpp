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

	// Вызов функции с параметром DeltaSeconds (Tick)
	MovementTick(DeltaSeconds);
}

void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* NewInputComponent)
{ 
	Super::SetupPlayerInputComponent(NewInputComponent);

	// InputComponent - это Axis,дальше текст с названием Input`a
	// Дальшее идет синтаксис this
	// Затем &ATPSCharacter:: и функция, которая будет вызываться при нажатии
	// Так же для кнопки вправо-влево.
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

	// Логика разворот пешки к курсору
	// Предварительно подключили библиотеку Kismet/GameplayStatics
	// Создаем указатель типа Controller, из GameplayStatics
	// берем контроллер игрока, передаем в качестве аргумента
	// GetWorld и нулевой индекс контроллера
	APlayerController* myController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	// проверка
	// Теперь у переменной можно вызвать функция GetHitResult
	if (myController)
	{
		// Эта функция принимает значение ETraceTypeQuery
		// По сути это трейс каналы, которые в С++ выглядят так:
		// TraceTypeQuery1 UMETA (Hidden)
		// TraceTypeQuery2 UMETA (Hidden)
		// TraceTypeQuery3 UMETA (Hidden)
		// ...
		// Все трейсы, которые мы добавили вручную, начинаются с 6го!
		// Поэтому выбираем шестой канал. Это и есть LanscapeCursor
		//
		// Третий аргумент HitResult возвращает значение, поэтому 
		// нужно заранее объявить переменную этого типа и передать 
		// её в качестве аргумента

		FHitResult ResultHit;
		myController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery6, false, ResultHit);
		

		// Далее чтобы прописать FindLookAtRotation, нужно подключить 
		// библиотеку <Kismet/KismetMathLibrary.h>. 
		// Оболочка Kismet для того, чтобы работать с ней в Blueprints
		// Первый аргумент - местоположение пешки 
		// Второй аргумент - ResultHit
		// Третий аргумент - локация, куда тыкнули курсором. 
		// UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location);
		// 
		// Далее вызываем функцию SetActorRotation.
		// Эта функция принимает кватернион.Это ротация с 4 переменными.
		// FQuat(X,Y,Z,W). Если пролистать аргументы функции FQuat,
		// то можно увидеть что он может определиться простым Rotator,
		// что нам и нужно. Определяем в кватернион Rotator. У него 
		// три аргумента: InPitch/InYaw/InRoll. Pitch и Roll по нулям,
		// но нужен Yaw (Z). Чтобы его достать мы можем поместить предыдущую
		// строку и через точку вызвать Yaw,т.к. FindLookAtRotation 
		// возвращает Rotation
		// 
		// SetActorRotation(FQuat(FRotator(0.0f, UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw, 0.0f)));
		// 
		// Чтобы упростить такую запись, достаточно создать переменную с типом float и присвоить ей строку c Yaw:
		float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
		SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
	}
}

// Функция, позволяющая менять скорость персонажа
void ATPSCharacter::CharacterUpdate()
{
	// Добавим переменную результирующей скорости
	// c значением пусть будет 600
	// Она понадобится для switch: будет ResSpeed
	// заменятьт на MovementInfo, прии этом вытаскивая
	// нужные параметры
	float ResSpeed = 600.0f;
	
	// Здесь посредством переменной MovementState, через switch
	// создается структура с нужными состояниями
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

	// После определения переменных, берем CharacterMovementComponent
	// и инициалзицируем максимальную скорость ходьбы результатаирующей
	// скоростью.
	GetCharacterMovement()->MaxWalkSpeed = ResSpeed;
}

void ATPSCharacter::ChangeMovementState(EMovementState NewMovementState)
{
	// В тело добавим условие:
	// Когда функция вызывается, то переменна принимает новое значение
	MovementState = NewMovementState;

	// Здесь будет вызываться функция CharacterUpdate
	CharacterUpdate();
}

