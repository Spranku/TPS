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


	// Вызов функции с параметром DeltaSeconds (Tick)
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

	// InputComponent - это Axis,дальше текст с названием Input`a
	// Дальшее идет синтаксис this
	// Затем &ATPSCharacter:: и функция, которая будет вызываться при нажатии
	// Так же для кнопки вправо-влево.
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
	// Когда пытаемся поменять текущее оружие на какое-то другое,проверяем есть ли хотя-бы 2 слота
	// в персонаже. Если есть, то сохраняем локальную переменную в виде старого индекса
	// 
	//
	if (CurrentWeapon && !CurrentWeapon->WeaponReloading && InventoryComponent->WeaponSlots.Num() > 1)
	{
		int8 OldIndex = CurrentIndexWeapon;
		FAdditionalWeaponInfo OldInfo;
		if (CurrentWeapon)
		{
			// Берем текущее оружие
			// загружаем инфу в локальную переменную (сколько патронов там было)
			// Если оружие перезаряжалось, то делаем отмену перезарядки
			OldInfo = CurrentWeapon->AdditionalWeaponInfo;
			if (CurrentWeapon->WeaponReloading)
				CurrentWeapon->CancelReload();
		}
		if (InventoryComponent)
		{
			// Убеждаемся, что есть InventoryComponent
			// 
			//
			if(InventoryComponent->SwitchWeaponToIndexByNextPreviosIndex(CurrentIndexWeapon +1, OldIndex, OldInfo,true))
			{ }
		}
	}

}

void ATPSCharacter::SwitchPreviosWeapon()
{
	//	То же самое,только индекс - 1
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
		// Проверка и инициализация
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

			// Логика разворот пешки к курсору
			// Предварительно подключили библиотеку Kismet/GameplayStatics
			// Создаем указатель типа Controller, из GameplayStatics
			// берем контроллер игрока, передаем в качестве аргумента
			// GetWorld и нулевой индекс контроллера
			// APlayerController* myController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			// проверка
			// Теперь у переменной можно вызвать функцию GetHitResult
			// if (myController && ToggleMouseInput)
			// {
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

			// 	FHitResult ResultHit;
			// 	myController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery6, false, ResultHit);

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
			// 	float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
			// 	SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
			// }

			// Ранее была допущена ошибка. В итоге поменяли функцию и изменили параметры.
			// Раньше был TraceQuert6,сейчас ECC_GameTraceChannel1. Теперь курсор будет обрабатываться правильно

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
							// Прицелились - оружие сводится. Стреляем - увеличивается на 1
							// и сразу уходит вниз,по тихоньку пытаясь уменьшаться
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
						// У нас есть результат попадания курсора на поверхность (ResultHit)
						// В оружие посылаем этот результат
						//CurrentWeapon->ShootEndLocation = ResultHit.Location + Displacement;
						CurrentWeapon->UpdateWeaponByCharacterMovementStateOnServer(ResultHit.Location + Displacement, bIsReduceDispersion);
					}
				}
			}
		}
		
	}	
	// Если у персонажа есть оружие и если скорость персонажа = 0,
	// то оружие уменьшает сведение разброса
	// Если персонаж бежит, то сведение увеличивается
	// if (CurrentWeapon)
	//	if (FMath::IsNearlyZero(GetVelocity().Size(), 0.5f))
	//		CurrentWeapon->ShouldReduseDespersion = true;
	//	else
	//		CurrentWeapon->ShouldReduseDespersion = false;
	// В итоге вынесли эту логику наверх

}

void ATPSCharacter::AttackCharEvent(bool bIsFiring)
{
	// Создаем локальную переменную
	// Вызываем функцию, которая возвращает оружие, которое находится у нас в руках
	AWeaponDefault* myWeapon = nullptr;
	myWeapon = GetCurrentWeapon();
	if (myWeapon)
	{
		// Устанавливаем тригер у этого оружия,что оно находится в состоянии стрельбы либо не стрельбы
		// (зажали либо расжали курок)
		// 
		//ToDo Check melee or range
		myWeapon->SetWeaponStateFire_OnServer(bIsFiring);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("ATPSCharacter::AttackCharEvent - CurrentWeapon -NULL"));
	// Иначе CurrentWeapon -NULL
}

// Функция, позволяющая менять скорость персонажа
void ATPSCharacter::CharacterUpdate()
{
	// Добавим переменную результирующей скорости
	// c значением пусть будет 600
	// Она понадобится для switch: будет ResSpeed
	// заменять на MovementInfo, прии этом вытаскивая
	// нужные параметры
	float ResSpeed = 600.0f;
	
	// Здесь посредством переменной MovementState, через switch
	// создается структура с нужными состояниями
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

	// После определения переменных, берем CharacterMovementComponent
	// и инициалзицируем максимальную скорость ходьбы результатаирующей
	// скоростью.
	GetCharacterMovement()->MaxWalkSpeed = ResSpeed;
}

void ATPSCharacter::ChangeMovementState()
{
	EMovementState NewState = EMovementState::Run_State;

	if (!WalkEnabled && !SprintRunEnabled && !AimEnabled)
	{
		// Включение поворота мыши после спринта
		//ToggleMouseInput = true;
		NewState = EMovementState::Run_State;
	}
	else
	{
		if (SprintRunEnabled )//&& AxisX) // Вместо AxisX мне нужно подставить координаты курсора
		{
			// Отключение поворота мыши
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

// Функция, чтобы нельзя было получать доступ напрямую из BP к этой переменной
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

// Эта функция теперь будет принимать ID Оружия
void ATPSCharacter::InitWeapon(FName IdWeaponName, FAdditionalWeaponInfo WeaponAdditionalInfo, int32 NewCurrentIndexWeapon)
{
	//OnServer
	
	// Проверка есть ли нынешнее оружие
	// Логика уничтожает текущее оружие в руках и делает переменную пустой
	// Ниже переменная обновляется (CurrentWeapon = myWeapon)
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}
	// Нужно взять гейм инстанс и сделать каст на него
	// Когда будет вызываться эта ф-я, будет давать наш BP_GameInstance 
	UTPSGameInstance* myGI = Cast<UTPSGameInstance>(GetGameInstance());
	FWeaponInfo myWeaponInfo;
	if (myGI)
	{
		// Если это прошло удачно, то мы вызываем у гейм инстанса
	    // функцию GetWeaponInfoByName(ID Weapon) в виде имени
		// Далее получаем структуру FWeaponInfo и эта структура получается
		// на выходе этой функции
		//
		// Если ф-я GeTWeaponInfoByName возвращает правду, то структура MyWeaponInfo у нас есть
		if (myGI->GetWeaponInfoByName(IdWeaponName, myWeaponInfo))
		{
			// Проверяем, что переменная не пустая
			// тогда мы попытаемся что-то заспавнить.
			if ((myWeaponInfo.WeaponClass))
			{
				// Готовим переменные для спавна, которые будут использованы ниже в спавн акторе
				FVector SpawnLocation = FVector(0);
				FRotator SpawnRotation = FRotator(0);

				// Готовим спавн параметры, которые говорят о том,что мы должны заспавнить объект 
				// игнорируя конфликты с другими коллизиями 
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Owner = this;           // устанавливаем наш
				SpawnParams.Instigator = GetInstigator(); // TPSCharacter

				// Далее идет функция GetWold()->SpawnActor, который принимает параметры
				// UCLASS,location,rotation,SpawnParams и на выходе у нас получается AActor
				// Далее (если справа налево) мы пытаемся этот AActor закастить на наш новый класс AWeaponDefault
				// Если каст проходит успешно, он записывается в переменную myWeapon,которая создана здесь локально 
				AWeaponDefault* myWeapon = Cast<AWeaponDefault>(GetWorld()->SpawnActor(myWeaponInfo.WeaponClass, &SpawnLocation, &SpawnRotation, SpawnParams));
				if (myWeapon)
				{
					// После проверки, что переменная создалась, мы аттачим её к нашему мешу 
					// Задаём правила спавна и указываем название сокета, куда нужно заспавнить
					//
					FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
					myWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));
					// После этого записываем в переменную оружие, которым владеет наш персонаж
					CurrentWeapon = myWeapon;
				
					// Структура myWeaponInfo инициализирует оружие
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
				
					// Когда иниц. оружие, нужно добавить делегаты
					// Сперва получаем делегаты на начало и конец  перезарядки
					// Далее вызываем AddDynamic,в аргумент даем ссылку на себя,
					// тоесть кто слушатель делегата? Character. Далее тот, кто услышал
					// вызывает функцию. В это случае WeaponReloadStart или End
					myWeapon->OnWeaponReloadStart.AddDynamic(this, &ATPSCharacter::WeaponReloadStart);
					myWeapon->OnWeaponReloadEnd.AddDynamic(this, &ATPSCharacter::WeaponReloadEnd);
					myWeapon->OnWeaponFireStart.AddDynamic(this, &ATPSCharacter::WeaponFireStart);

					// Когда обратно переключаемся на оружие, а в нём нет патронов,
					// в начале жизненного цикла иниц. перезарядку (если это можно сделать)
					// и если патроны в оружие = 0
					//
					if (CurrentWeapon->GetWeaponRound() <= 0 && CurrentWeapon->CheckCanWeaponReload())
						CurrentWeapon->InitReload();

					// Сообщает,что оружие поменяли и в оружии достаточно патронов
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

	// Сперва проверяем что у персонажа есть текущее оружие и оно не перезаряжается
	if (bIsAlive && CurrentWeapon && !CurrentWeapon->WeaponReloading)
	{
		// Если текущее кол-во патронов больше? или равно максимальному количеству,
		// то вызываем ф-ю инициализации перезарядки
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
	// Берем инветарь и меняем паторы
	// Когда перезаряжаем оружие, берем текущий тип оружия и сколько должны взять патронов
	// Передаем в инвентарь сколько патронов осталось (AmmoSafe)
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

// Реализация интерфейса
EPhysicalSurface ATPSCharacter::GetSurfaceType()
{
	EPhysicalSurface Result = EPhysicalSurface::SurfaceType_Default;
	if (CharHealthComponent)
	{ // Проверка если у персонажа щит еще есть, то никакой эффект накладываться не будет
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
	// Ф-я возвращает между 0 и числом которое поставили
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

	// Таймер на время проигрывания анимации смерти,после нее вызовется ф-я ниже
	GetWorldTimerManager().SetTimer(TimerHandle_RagDollTimer, this, &ATPSCharacter::EnableRagDoll, TimeAnim, false);

	//GetCursorToWorld()->SetVisibility(false); приводит к ошибке

	// Персонаж умер, стрельба не возможна
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

	// Если дамаг идёт от RadialDamage то попробуем добавить эффект, если он есть
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





