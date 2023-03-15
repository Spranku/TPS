// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/WeaponDefault.h"

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
}

void AWeaponDefault::FireTick(float DeltaTime)
{
	// Каждый кадр, по сути оружие пытается выстрелить
	// Проверка идёт: разрешено ли оружию стрелять
	// Лишь переменная контролирует огонь
	// Есть так же переменна FireTime - простой таймер, 
	// который обновляется из структуры RateOfFire
	
	if (WeaponFiring)
		if (FireTime < 0.f)
			Fire();
		else
			FireTime -= DeltaTime;
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
	// Когда оружие выстрелило,следующий кадр который придёт - файр тайм уже будет 0.5
	// и эта проверка не пройдет,а пойдет в элс и в след.кадре будет 0.5 -  Дельта = 0.4
	// и так с каждым кадром всё меньше, пока не дойдет до 0. Тогда оружие снова сможет стрелять
	//
	//
}

// Функция, которая говорит когда оружие может стрелять (true - всегда)
bool AWeaponDefault::CheckWeaponCanFire()
{
	return true;
}

FProjectileInfo AWeaponDefault::GetProjectile()
{
	return WeaponSetting.ProjectileSetting;
}

void AWeaponDefault::Fire()
{
	// У оружия есть структура веапон сеттинг,мы обращаемся к рейтоффайр,
	// Рейтоффайр у винтовки = 0.5, и Файр тайм становится 0.5
	//
	FireTime = WeaponSetting.RateOfFire;

	if (ShootLocation)
	{
		// Проверяем есть ли у оружия shootLocation (arrow comp)

		// Далее берем место, где должна заспавниться пуля у арро компонента
		// Берем так же ротацию
		FVector SpawnLocation = ShootLocation->GetComponentLocation();
		FRotator SpawnRotation = ShootLocation->GetComponentRotation();
		// Инициируем ProjectileInfo, берем инфо из функции,которая в свою очередь берет инфо
		// у Weapon Settings.ProjectileSettings
		FProjectileInfo ProjectileInfo;
		ProjectileInfo = GetProjectile();

		// Далее идёт структура. Проверяем в ней наличие класса projectile 
		if (ProjectileInfo.Projectile)
		{
			//Projectile Init ballistic fire
			// Если projectile есть, начинаем его спавнить
			
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = GetInstigator();

			AProjectileDefault* myProjectile = Cast<AProjectileDefault>(GetWorld()->SpawnActor(ProjectileInfo.Projectile, &SpawnLocation, &SpawnRotation, SpawnParams));
			if (myProjectile && myProjectile != nullptr)
			{
				//ToDo Init Projectile settings by id in table row(or keep in weapon table)
				// Добавляем условие, что пуля должна жить 20 секунд
				myProjectile->InitialLifeSpan = 20.0f;
				
				//Projectile->BulletProjectileMovement->InitialSpeed = 2500.0f;
			}
		}
		else
		{
			//ToDo Projectile null Init trace fire		
			// Если пули нет, допишем логику стрельбы трейсами
		}
	}
}

void AWeaponDefault::UpdateStateWeapon(EMovementState NewMovementState)
{
	//ToDo Dispersion
	ChangeDispersion();
}

void AWeaponDefault::ChangeDispersion()
{
}

