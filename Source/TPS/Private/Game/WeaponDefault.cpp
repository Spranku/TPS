// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/WeaponDefault.h"

// Sets default values
AWeaponDefault::AWeaponDefault()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// ��������� ������, ������� UScene ���������, ����� ���������� �������
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneComponent;

	// ��������� �������� ��� 
	// ��������, ��� �� �� ������ ������������ overlap event
	// �������� �� noCollision, �.�. ������ - ��� ������ ������
	SkeletalMeshWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	SkeletalMeshWeapon->SetGenerateOverlapEvents(false);
	SkeletalMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
	SkeletalMeshWeapon->SetupAttachment(RootComponent);

	// �� �� �� ������ �����
	// ��� ���������� (������ � ������) �� ������, ���� ������ ����� ������� ������ �����
	StaticMeshWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh "));
	StaticMeshWeapon->SetGenerateOverlapEvents(false);
	StaticMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
	StaticMeshWeapon->SetupAttachment(RootComponent);

	// ��� ������� ������ ��������� � ��������� � ����
	ShootLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("ShootLocation"));
	ShootLocation->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AWeaponDefault::BeginPlay()
{
	Super::BeginPlay();
	  
	//WeaponInit(); ����� ������ �� �����,�.�. ����� ���������� �� Character
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
	// ������ ����, �� ���� ������ �������� ����������
	// �������� ���: ��������� �� ������ ��������
	// ���� ���������� ������������ �����
	// ���� ��� �� ��������� FireTime - ������� ������, 
	// ������� ����������� �� ��������� RateOfFire
	
	if (WeaponFiring)
		if (FireTime < 0.f)
			Fire();
		else
			FireTime -= DeltaTime;
}

void AWeaponDefault::WeaponInit()
{
	// �������� ���� � ������ ������� ���� ������,
	// �� �� � ��� ������ �� ������
	// ���� ����� ��� ����, �� ��������� ����������
	if (SkeletalMeshWeapon && !SkeletalMeshWeapon->SkeletalMesh)
	{
		SkeletalMeshWeapon->DestroyComponent(true);
	}
	// ��� �� � ��� ������ ��� ����������
	if (StaticMeshWeapon && !StaticMeshWeapon->GetStaticMesh())
	{
		StaticMeshWeapon->DestroyComponent();
	}
}

void AWeaponDefault::SetWeaponStateFire(bool bIsFire)
{
	// ����� �������� ����� �� ������ �������� - �������� ���������� ������ � ���, ��� ������
	// ������ ����� ����� - �������� true, ������ ��������� � ��������� ��������
	// ���� �������� �� �����, ������ false
	// ��� �� � ������, ����� �� ������ ������, ���������� �������� �� false
	if (CheckWeaponCanFire())
		WeaponFiring = bIsFire;
	else
		WeaponFiring = false;
	// ����� ������ ����������,��������� ���� ������� ����� - ���� ���� ��� ����� 0.5
	// � ��� �������� �� �������,� ������ � ��� � � ����.����� ����� 0.5 -  ������ = 0.4
	// � ��� � ������ ������ �� ������, ���� �� ������ �� 0. ����� ������ ����� ������ ��������
	//
	//
}

// �������, ������� ������� ����� ������ ����� �������� (true - ������)
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
	// � ������ ���� ��������� ������ �������,�� ���������� � ����������,
	// ���������� � �������� = 0.5, � ���� ���� ���������� 0.5
	//
	FireTime = WeaponSetting.RateOfFire;

	if (ShootLocation)
	{
		// ��������� ���� �� � ������ shootLocation (arrow comp)

		// ����� ����� �����, ��� ������ ������������ ���� � ���� ����������
		// ����� ��� �� �������
		FVector SpawnLocation = ShootLocation->GetComponentLocation();
		FRotator SpawnRotation = ShootLocation->GetComponentRotation();
		// ���������� ProjectileInfo, ����� ���� �� �������,������� � ���� ������� ����� ����
		// � Weapon Settings.ProjectileSettings
		FProjectileInfo ProjectileInfo;
		ProjectileInfo = GetProjectile();

		// ����� ��� ���������. ��������� � ��� ������� ������ projectile 
		if (ProjectileInfo.Projectile)
		{
			//Projectile Init ballistic fire
			// ���� projectile ����, �������� ��� ��������
			
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = GetInstigator();

			AProjectileDefault* myProjectile = Cast<AProjectileDefault>(GetWorld()->SpawnActor(ProjectileInfo.Projectile, &SpawnLocation, &SpawnRotation, SpawnParams));
			if (myProjectile && myProjectile != nullptr)
			{
				//ToDo Init Projectile settings by id in table row(or keep in weapon table)
				// ��������� �������, ��� ���� ������ ���� 20 ������
				myProjectile->InitialLifeSpan = 20.0f;
				
				//Projectile->BulletProjectileMovement->InitialSpeed = 2500.0f;
			}
		}
		else
		{
			//ToDo Projectile null Init trace fire		
			// ���� ���� ���, ������� ������ �������� ��������
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

