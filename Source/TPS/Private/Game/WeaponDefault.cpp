// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/WeaponDefault.h"
#include <Kismet/GameplayStatics.h> 
#include "/My_Projects/TPS/Source/TPS/Character/TPSInventoryComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Game/TPS_StateEffect.h"



// Sets default values
AWeaponDefault::AWeaponDefault()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Создается объект, простой UScene компонент, сразу становится рутовый
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneComponent;

	// Создается скелетал меш 
	// Сообщаем, что он не должен генерировать overlap event
	// Коллизию на noCollision, т.к. оружие - это просто визуал
	SkeletalMeshWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	SkeletalMeshWeapon->SetGenerateOverlapEvents(false);
	SkeletalMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
	SkeletalMeshWeapon->SetupAttachment(RootComponent);

	// То же со статик мешем
	// Два компонента (статик и скелет) на случай, если оружие будет простым статик мешем
	StaticMeshWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh "));
	StaticMeshWeapon->SetGenerateOverlapEvents(false);
	StaticMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
	StaticMeshWeapon->SetupAttachment(RootComponent);

	// Шут локейшн просто создается и аттачится к руту
	ShootLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("ShootLocation"));
	ShootLocation->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AWeaponDefault::BeginPlay()
{
	Super::BeginPlay();
	  
	//WeaponInit(); Здесь больше не нужен,т.к. будет вызываться из Character
	WeaponInit();
	
}

// Called every frame
void AWeaponDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FireTick(DeltaTime);
	ReloadTick(DeltaTime);
	DispersionTick(DeltaTime);
	ClipDropTick(DeltaTime);
	ShellDropTick(DeltaTime);
}

void AWeaponDefault::FireTick(float DeltaTime)
{
	if (WeaponFiring && GetWeaponRound() > 0 && !WeaponReloading)
	{
		if (FireTime < 0.f)
		{
				Fire();
		}
		else
			FireTime -= DeltaTime;
	}
}

void AWeaponDefault::ReloadTick(float DeltaTime)
{
	// Когда оружие начинает перезарядку, в тике отсчитывается таймер
	// До тех пор,пока идёт перезарядка и ReloadTimer НЕ < 0
	// 
	//
	if (WeaponReloading)
	{
		if (ReloadTimer  < 0.0f)
		{
			// Как только меньше 0, говорим что оружие перезарядилось
			FinishReload();
		}
		else
		{
			// Пока таймер не дошёл до нуля, он должен уменьшаться по DeltaTime
			ReloadTimer -= DeltaTime;
		}
	}
}

void AWeaponDefault::DispersionTick(float DeltaTime)
{
	if (!WeaponReloading) /*Ecли оружие не перезаряжается*/
	{
		if (!WeaponFiring) /*Если оружие не стреляет*/
		{
			if (ShouldReduseDespersion) /*Уменьшаем разброс*/
				CurrentDispersion = CurrentDispersion - CurrentDispersionReduction;
			else                        /*Увеличиваем*/
				CurrentDispersion = CurrentDispersion + CurrentDispersionReduction;
		}

		if (CurrentDispersion < CurrentDispersionMin)
		{
			CurrentDispersion = CurrentDispersionMin;
		}
		else
		{
			if (CurrentDispersion > CurrentDispersionMax)
			{
				CurrentDispersion = CurrentDispersionMax;
			}
		}
	}
}

void AWeaponDefault::WeaponInit()
{
	// Проверка если у нашего скелета есть скелет,
	// то мы с ним ничего не делаем
	// Если вдруг его нету, то компонент уничножаем
	if (SkeletalMeshWeapon && !SkeletalMeshWeapon->SkeletalMesh)
	{
		SkeletalMeshWeapon->DestroyComponent(true);
	}
	// Так же и для статик меш компонента
	if (StaticMeshWeapon && !StaticMeshWeapon->GetStaticMesh())
	{
		StaticMeshWeapon->DestroyComponent();
	}
}

void AWeaponDefault::SetWeaponStateFire(bool bIsFire)
{
	// После проверки может ли оружие стрелять - меняется переменная вместе с той, что пришла
	// Нажали левую кнопу - приходит true, оружие находится в состоянии стрельбы
	// Если стрелять не может, оружие false
	// Так же в момент, когда мы отжали кнопку, переменная меняется на false
	if (CheckWeaponCanFire())
		WeaponFiring = bIsFire;
	else
		WeaponFiring = false;
	FireTime = 0.01f;
	// Когда оружие выстрелило,следующий кадр который придёт - файр тайм уже будет 0.5
	// и эта проверка не пройдет,а пойдет в элс и в след.кадре будет 0.5 -  Дельта = 0.4
	// и так с каждым кадром всё меньше, пока не дойдет до 0. Тогда оружие снова сможет стрелять
	//
	//
}

// Функция, которая говорит когда оружие может стрелять (true - всегда)
bool AWeaponDefault::CheckWeaponCanFire()
{
	return !BlockFire;
}

FProjectileInfo AWeaponDefault::GetProjectile()
{
	return WeaponSetting.ProjectileSetting;
}

void AWeaponDefault::Fire()
{
	UAnimMontage* AnimToPlay = nullptr;

	if (WeaponAiming)
		AnimToPlay = WeaponSetting.AnimWeaponInfo.AnimCharFireAim;
	else
		AnimToPlay = WeaponSetting.AnimWeaponInfo.AnimCharFire;

	if (WeaponSetting.AnimWeaponInfo.AnimWeaponFire
		&& SkeletalMeshWeapon
		&& SkeletalMeshWeapon->GetAnimInstance())//Bad Code? maybe best way init local variable or in func
	{
		SkeletalMeshWeapon->GetAnimInstance()->Montage_Play(WeaponSetting.AnimWeaponInfo.AnimWeaponFire);
	}

	if (WeaponSetting.ShellBullets.DropMesh)
	{
		if (WeaponSetting.ShellBullets.DropMeshTime < 0.0f)
		{
			InitDropMesh(WeaponSetting.ShellBullets.DropMesh, 
				WeaponSetting.ShellBullets.DropMeshOffset, 
				WeaponSetting.ShellBullets.DropMeshImpulseDir, 
				WeaponSetting.ShellBullets.DropMeshLifeTime, 
				WeaponSetting.ShellBullets.ImpulseRandomDispersion, 
				WeaponSetting.ShellBullets.PowerImpulse, 
				WeaponSetting.ShellBullets.CustomMass);
		}
		else
		{
			DropShellFlag = true;
			DropShellTimer = WeaponSetting.ShellBullets.DropMeshTime;
		}
	}
	// У оружия есть структура веапон сеттинг,мы обращаемся к рейтоффайр,
	// Рейтоффайр у винтовки = 0.5, и Файр тайм становится 0.5
	//
	FireTime = WeaponSetting.RateOfFire;
	// В момент выстрела минусуем патроны 
	AdditionalWeaponInfo.Round = AdditionalWeaponInfo.Round - 1; \
	ChangeDispersionByShoot();

	OnWeaponFireStart.Broadcast(AnimToPlay);

	// Звук ружия
	// SpawnSoundAtLocation() принимает аргументы: GetWorld(), сам звук( уже настроен в таблицах SoundFireWeapon)
	// Локация, в которой будет играть звук ( возьмем локацию дула (ShootLocation) 
	// Остальные аргументы не трогаем
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WeaponSetting.SoundFireWeapon, ShootLocation->GetComponentLocation());
	// Эффект оружия
	// То же самое, только берем данные из таблицы EffectFireWeapon
	// И Transform нужно передать. Передать его нужно от дула, откуда вылетают пули
	//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponSetting.EffectFireWeapon, ShootLocation->GetComponentTransform());
	UGameplayStatics::SpawnEmitterAttached(WeaponSetting.EffectFireWeapon, ShootLocation);
	// Ф-я для дробовика
	// Берем количество пуль
	int8 NumberProjectile = GetNumberProjectileByShot();

	if (ShootLocation)
	{
		// Проверяем есть ли у оружия shootLocation (arrow comp)

		// Далее берем место, где должна заспавниться пуля у арров компонента					!!!
		// Берем так же ротацию																	!!!
		FVector SpawnLocation = ShootLocation->GetComponentLocation();
		FRotator SpawnRotation = ShootLocation->GetComponentRotation();
		// Инициируем ProjectileInfo, берем инфо из функции,которая в свою очередь берет инфо
		// у Weapon Settings.ProjectileSettings
		FProjectileInfo ProjectileInfo;
		ProjectileInfo = GetProjectile();
		FVector EndLocation;

		// В этом цикле для каждой следующей пули, которая будет спавниться создается вектор
		// Внутри цикла будет вся логика спавна и разворота пули

		for (int8 i = 0; i < NumberProjectile; i++)
		{
			EndLocation = GetFireEndLocation();

			// Далее идёт структура. Проверяем в ней наличие класса projectile 
			if (ProjectileInfo.Projectile)
			{
				//Projectile Init ballistic fire	
				// Нужно определить, в какую сторону поворачивать пулю (её ротацию)
			    // Для этого получаем Direction(направление), берем местоположение крусора,
				// отнимаем его от вектора где находится Arrow Component
				// После этого вектор обязательно нормализовать
				FVector Dir = /*ShootEndLocation*/ GetFireEndLocation() - SpawnLocation;
				Dir.Normalize();
				// Далее строим матрицу, даём ей нормализованный вектор,даём векторы по плоскости Y,Z 
				// и нулевой вектор (это просто 0,0,0)
				// После получения матрицы, можно с помощью функции Rotator() получить ротацию для пули
				//
				// В конспекте указано,что 0,1, и 0,0,1 - не верно,хотя и не влияет на работу
				// Исправил на 000 и 000 соответсвенно
				FMatrix myMatrix(Dir, FVector(0, 0, 0), FVector(0, 0, 0), FVector::ZeroVector);
				SpawnRotation = myMatrix.Rotator();

				//Projectile Init ballistic fire
				// Если projectile есть, начинаем его спавнить
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Owner = GetOwner();
				SpawnParams.Instigator = GetInstigator();

				AProjectileDefault* myProjectile = Cast<AProjectileDefault>(GetWorld()->SpawnActor(ProjectileInfo.Projectile, &SpawnLocation, &SpawnRotation, SpawnParams));
				if (myProjectile /* && myProjectile != nullptr*/ )
				{
					//ToDo Init Projectile settings by id in table row(or keep in weapon table)
					// 
					// Добавляем условие, что пуля должна жить 20 секунд
					//myProjectile->InitialLifeSpan = 20.0f;
					//Projectile->BulletProjectileMovement->InitialSpeed = 2500.0f;

					 myProjectile->InitProjectile(WeaponSetting.ProjectileSetting);
				}
			}
			else
			{
				// Если пули нет,исполняется логика стрельбы трейсами

				FHitResult Hit;
				TArray<AActor*> Actors;

				UKismetSystemLibrary::LineTraceSingle(GetWorld(),
					SpawnLocation,
					EndLocation * WeaponSetting.DistanceTrace,
					TraceTypeQuery4,
					false,
					Actors,
					EDrawDebugTrace::ForDuration,
					Hit,
					true,
					FLinearColor::Red,
					FLinearColor::Green,
					5.0f);

				if (ShowDebug)
				{
					DrawDebugLine(GetWorld(),
						SpawnLocation,
						EndLocation + ShootLocation->GetForwardVector() * WeaponSetting.DistanceTrace,
						FColor::Black,
						false,
						5.0f,
						(uint8)'\000',
						0.5f);
				}

				if (Hit.GetActor() && Hit.PhysMaterial.IsValid())
				{
					EPhysicalSurface mySurfaceType = UGameplayStatics::GetSurfaceType(Hit);

					if (WeaponSetting.ProjectileSetting.HitDecals.Contains(mySurfaceType))
					{
						UMaterialInterface* myMaterial = WeaponSetting.ProjectileSetting.HitDecals[mySurfaceType];

						if (myMaterial && Hit.GetComponent())
						{
							UGameplayStatics::SpawnDecalAttached(myMaterial,
								FVector(20.0f),
								Hit.GetComponent(),
								NAME_None,
								Hit.ImpactPoint,
								Hit.ImpactNormal.Rotation(),
								EAttachLocation::KeepWorldPosition,
								5.0f);
						}
					}
					if (WeaponSetting.ProjectileSetting.HitFXs.Contains(mySurfaceType))
					{
						UParticleSystem* myParticle = WeaponSetting.ProjectileSetting.HitFXs[mySurfaceType];
						if (myParticle)
						{
							UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
								myParticle,
								FTransform(Hit.ImpactNormal.Rotation(),
								Hit.ImpactPoint,
								FVector(1.0f)));
						}
					}

					if (WeaponSetting.ProjectileSetting.HitSound)
					{
						UGameplayStatics::PlaySoundAtLocation(GetWorld(),
							WeaponSetting.ProjectileSetting.HitSound,
							Hit.ImpactNormal);
					}

					UType::AddEffecttBySurfaceType(Hit.GetActor(),Hit.BoneName, ProjectileInfo.Effect, mySurfaceType);

					UGameplayStatics::ApplyPointDamage(Hit.GetActor(), 
						WeaponSetting.ProjectileSetting.ProjectileDamage, 
						Hit.TraceStart, 
						Hit, 
						GetInstigatorController(), 
						this, 
						NULL);
				}
			}
		}
	}

	// После того как оружие отработает и посчитает сколько патронов было потрачено,
	// тогда  иниц проверку может оружию пора перезаряжаться?
	// Если это правда, и оно не находится в сост. перезарядки,
	// пытаемся перезарядить
	if (GetWeaponRound() <= 0 && !WeaponReloading)
	{
		if (CheckCanWeaponReload())
			InitReload();
	}
}

void AWeaponDefault::UpdateStateWeapon(EMovementState NewMovementState)
{
	//ToDo Dispersion
	BlockFire = false;

	switch (NewMovementState)
	{
	case EMovementState::Aim_State:
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.Aim_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSetting.DispersionWeapon.Aim_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.Aim_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSetting.DispersionWeapon.Aim_StateDispersionAimReduction;
			break;
	case EMovementState::AimWalk_State:
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.AimWalk_StateDispersionAimMax;
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.AimWalk_StateDispersionAimMin;
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.AimWalk_StateDispersionAimRecoil;
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.AimWalk_StateDispersionAimReduction;
		break;
	case EMovementState::Walk_State:
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.Walk_StateDispersionAimMax;
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.Walk_StateDispersionAimMin;
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.Walk_StateDispersionAimRecoil;
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.Walk_StateDispersionAimReduction;
		break;
	case EMovementState::Run_State:
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.Run_StateDispersionAimMax;
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.Run_StateDispersionAimMin;
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.Run_StateDispersionAimRecoil;
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.Run_StateDispersionAimReduction;
		break;
	case EMovementState::SprintRun_State:
		BlockFire = true;
		SetWeaponStateFire(false);
		break;
	default:
		break;
	}
	ChangeDispersion();
}

void AWeaponDefault::ChangeDispersion()
{
}

void AWeaponDefault::ChangeDispersionByShoot()
{
	// Когда делаем выстрел - разброс будет меняться на параметр из таблицы
	// ChangeDispersionByShoot вызывается в Fire();
	CurrentDispersion = CurrentDispersion + CurrentDispersionRecoil;
}

float AWeaponDefault::GetCurrentDispersion() const
{
	float Result = CurrentDispersion;
	return Result;
}

FVector AWeaponDefault::ApplyDispersionToShoot(FVector DirectionShoot) const
{
	//if (ShowDebug)
	//{
	//	DrawDebugCone(GetWorld(), ShootLocation->GetComponentLocation(), -DirectionShoot, WeaponSetting.DistanceTrace, GetCurrentDispersion() * PI / 180.f);
	//}
	// Эта фукнция бюерет направление вектора и относительно него , в зависимости
	// от параметром GetCurrentDispersion (который выдаст какой- то разброс) умножаем на PI / 180, чтобы получить радианы
	// и получаем новый вектор, который будет немного смещен по конусу. 
	// 
	// GetCurrentDispersion будет работать в зависимости от табличных данных.
	return FMath::VRandCone(DirectionShoot, GetCurrentDispersion() * PI / 180.f);
}

FVector AWeaponDefault::GetFireEndLocation() const
{
	bool bShootDirection = false;
	FVector EndLocation = FVector(0.f);
	// отнимаем точку дула от направления, куда хотим стрельнуть, получаем вектор tmpV
	FVector tmpV = (ShootLocation->GetComponentLocation() - ShootEndLocation);
	// Далее берём его размер и сравниваем с переменной равной сотни
	if (tmpV.Size() > SizeVectorToChangeShootDirectionLogic)
	{
		//EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot((ShootLocation->GetComponentLocation() - ShootEndLocation).GetSafeNormal()) * -20000.0f;
		EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot((ShootLocation->GetComponentLocation() - ShootEndLocation).GetSafeNormal()) * -20000.0f;
	}
	else
	{
		// Получаем конечную точку пули, куда она дожна полететь(конечный вектор)
			// Далее берем ArrowComponent, который находится у дула  
			// И вызываем ф-ю ApplyDispersionToShoot, во входящий параметр передаем GetForwardVector от нашего ArrowComponent
		//EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot(ShootLocation->GetForwardVector()) * 20000.0f;
		EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot(ShootLocation->GetForwardVector()) * 20000.0f;
	}
	return EndLocation;
}

int8 AWeaponDefault::GetNumberProjectileByShot() const
{
	// Возвращает число пуль, которых должно вылететь
	return WeaponSetting.NumberProjectileByShot;
}

int32 AWeaponDefault::GetWeaponRound()
{
	return AdditionalWeaponInfo.Round;
}
// Ф-я перезарядки
void AWeaponDefault::InitReload()
{
	WeaponReloading = true;

	// Когда инициализировался таймер, передаем значение
	// из структуры: какое время нужно оружию, чтобы перезарядиться
	ReloadTimer = WeaponSetting.ReloadTime; // (2 секунды)

	// Делегат для анимации перезарядки
	// Выписать имя делегата, вызвать Broadcast()
	// Это буквально значит "скажи всем кто меня слушает..."
	//
	// Проверка если есть анимация, делаем Broadcast()
	if(WeaponSetting.AnimCharReload)
		OnWeaponReloadStart.Broadcast(WeaponSetting.AnimCharReload);

	// Когда начинается перезарядка, логика InitDropMesh отрабатывает
	if (WeaponSetting.ClipDropMesh.DropMesh)
	{
		DropClipFlag = true;
		DropClipTimer = WeaponSetting.ClipDropMesh.DropMeshTime;
	}
}

void AWeaponDefault::FinishReload()
{
	WeaponReloading = false;

	int8 AviableAmmoFromInventory = GetAviableAmmoForReload();
	int8 AmmoNeedTakeFromInv;
	int8 NeedToReload = WeaponSetting.MaxRound - AdditionalWeaponInfo.Round;

	if (NeedToReload > AviableAmmoFromInventory)
	{
		AdditionalWeaponInfo.Round = AviableAmmoFromInventory;
		AmmoNeedTakeFromInv = AviableAmmoFromInventory;
	}
	else
	{
		AdditionalWeaponInfo.Round += NeedToReload;
		AmmoNeedTakeFromInv = NeedToReload;
	}

	// Когда оружие закончило перезарядку, отправляется информация в делегат
	OnWeaponReloadEnd.Broadcast(true, -AmmoNeedTakeFromInv);
}

void AWeaponDefault::CancelReload()
{
	// Если есть скелет у оружия, то останавливаем у оружия все анимации,
	// убираем флаг на перезарядку 
	WeaponReloading = false;
	if (SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
		SkeletalMeshWeapon->GetAnimInstance()->StopAllMontages(0.15f);
	// Отправляется информация в делегат
	OnWeaponReloadEnd.Broadcast(false,0);
}

bool AWeaponDefault::CheckCanWeaponReload()
{
	// У этого оружия есть владелец
	// у владельца есть инвентарь
	// Если MyInv есть,тогда постараемся стянуть информацию, может ли он перезаряжаться
	// Если нет ни владельца, ни инвентаря, тогда result = true;
	// Если мы не можем найти инвентарь, тогда считаем что боеприпасы бесконечны.
	//
	bool result = true;
	if (GetOwner())
	{
		UTPSInventoryComponent* MyInv = Cast<UTPSInventoryComponent>(GetOwner()->GetComponentByClass(UTPSInventoryComponent::StaticClass()));
		if (MyInv)
		{
			int8 AviableAmmoForWeapon;
			if (!MyInv->CheckAmmoForWeapon(WeaponSetting.WeaponType, AviableAmmoForWeapon))
			{
				result = false;
			}
		}
	}
	return result;
}
// Взять количество патронов до перезарядки
int8 AWeaponDefault::GetAviableAmmoForReload()
{
	int8 AviableAmmoForWeapon = WeaponSetting.MaxRound;
	if (GetOwner())
	{
		UTPSInventoryComponent* MyInv = Cast<UTPSInventoryComponent>(GetOwner()->GetComponentByClass(UTPSInventoryComponent::StaticClass()));
		if (MyInv)
		{
			if (MyInv->CheckAmmoForWeapon(WeaponSetting.WeaponType, AviableAmmoForWeapon))
			{
				AviableAmmoForWeapon = AviableAmmoForWeapon;
			}
		}
	}
	return AviableAmmoForWeapon;
}

void AWeaponDefault::ShellDropTick(float DeltaTime)
{
	if (DropShellFlag)
	{
		if (DropShellTimer < 0.0f)
		{
			DropShellFlag = false;
			InitDropMesh(WeaponSetting.ShellBullets.DropMesh, 
				WeaponSetting.ShellBullets.DropMeshOffset, 
				WeaponSetting.ShellBullets.DropMeshImpulseDir, 
				WeaponSetting.ShellBullets.DropMeshLifeTime, 
				WeaponSetting.ShellBullets.ImpulseRandomDispersion, 
				WeaponSetting.ShellBullets.PowerImpulse, 
				WeaponSetting.ShellBullets.CustomMass);
				
		}
		else
			DropShellTimer -= DeltaTime;
	}
}

void AWeaponDefault::ClipDropTick(float DeltaTime)
{
	if (DropClipFlag)
	{
		if (DropClipTimer < 0.0f)
		{
			DropClipFlag = false;
			InitDropMesh(WeaponSetting.ClipDropMesh.DropMesh, 
				WeaponSetting.ClipDropMesh.DropMeshOffset, 
				WeaponSetting.ClipDropMesh.DropMeshImpulseDir, 
				WeaponSetting.ClipDropMesh.DropMeshLifeTime, 
				WeaponSetting.ClipDropMesh.ImpulseRandomDispersion, 
				WeaponSetting.ClipDropMesh.PowerImpulse, 
				WeaponSetting.ClipDropMesh.CustomMass);
		}
		else
			DropClipTimer -= DeltaTime;
	}
}

void AWeaponDefault::InitDropMesh(UStaticMesh* DropMesh, 
	FTransform Offset, FVector DropImpulseDirection, 
	float LifeTimeMesh, 
	float ImpulseRandomDispersion, 
	float PowerImpulse, 
	float CustomMass)
{
	// Берем нулевой пивот блупринта оружия, берем форвард вектор и умножаем его по X,Y,Z, получая локальную позицию
	// относительно оружия. Запихиваем это в трансформ, чтобы заспавнить объект создаем переменную статик меш актор
	// Основная ф-я GetWorld()->SpawnActor<AStaticMeshActor>, делаем его статичным, выставляем трансформ и параметры
	if (DropMesh)
	{
		FTransform Transform;

		FVector LocalDir = this->GetActorForwardVector() * Offset.GetLocation().X + this->GetActorRightVector() * Offset.GetLocation().Y + this->GetActorUpVector() * Offset.GetLocation().Z;

		Transform.SetLocation(GetActorLocation() + LocalDir);
		Transform.SetScale3D(Offset.GetScale3D());

		Transform.SetRotation((GetActorRotation() + Offset.Rotator()).Quaternion());
		AStaticMeshActor* NewActor = nullptr;


		FActorSpawnParameters Param;
		Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		Param.Owner = this;
		NewActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Transform, Param);

		if (NewActor && NewActor->GetStaticMeshComponent())
		{
			NewActor->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
			NewActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

			//set parameter for new actor
			NewActor->SetActorTickEnabled(false);
			NewActor->InitialLifeSpan = LifeTimeMesh;

			NewActor->GetStaticMeshComponent()->Mobility = EComponentMobility::Movable;
			NewActor->GetStaticMeshComponent()->SetSimulatePhysics(true);
			NewActor->GetStaticMeshComponent()->SetStaticMesh(DropMesh);
			//NewActor->GetStaticMeshComponent()->SetCollisionObjectType()



			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Block);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECollisionResponse::ECR_Block);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECollisionResponse::ECR_Block);



			if (CustomMass > 0.0f)
			{
				NewActor->GetStaticMeshComponent()->SetMassOverrideInKg(NAME_None, CustomMass, true);
			}

			if (!DropImpulseDirection.IsNearlyZero())
			{
				FVector FinalDir;
				LocalDir = LocalDir + (DropImpulseDirection * 1000.0f);

				if (!FMath::IsNearlyZero(ImpulseRandomDispersion))
					FinalDir += UKismetMathLibrary::RandomUnitVectorInConeInDegrees(LocalDir, ImpulseRandomDispersion);
				FinalDir.GetSafeNormal(0.0001f);

				NewActor->GetStaticMeshComponent()->AddImpulse(FinalDir * PowerImpulse);
			}
		}
	}
}

