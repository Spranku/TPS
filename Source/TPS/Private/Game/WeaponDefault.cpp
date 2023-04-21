// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/WeaponDefault.h"
#include <Kismet/GameplayStatics.h> 


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
	// ��� ����������� ����� � �������� ����
	ReloadTick(DeltaTime);
	DispersionTick(DeltaTime);
}

void AWeaponDefault::FireTick(float DeltaTime)
{
	// ������ ����, �� ���� ������ �������� ����������
	// �������� ���: ��������� �� ������ ��������
	// ���� ���������� ������������ �����
	// ���� ��� �� ��������� FireTime - ������� ������, 
	// ������� ����������� �� ��������� RateOfFire
	
	// ���� �������� ������ 0, �� ��������
	if (GetWeaponRound() > 0)
	{
		if (WeaponFiring)
			if (FireTime < 0.f)
			{
				// ���� ������ ��������� � ���������� ���������, �� ��� ����� ��������
				if (!WeaponReloading)
					Fire();
			}
			else
				FireTime -= DeltaTime;
	}
	else
		// ����� ���������� �����������
	{
		if (!WeaponReloading)
			// ���� ������ ��� �� ��������������
		{
			InitReload();
		}
	}
}

void AWeaponDefault::ReloadTick(float DeltaTime)
{
	// ����� ������ �������� �����������, � ���� ������������� ������
	// �� ��� ���,���� ��� ����������� � ReloadTimer �� < 0
	// 
	//
	if (WeaponReloading)
	{
		if (ReloadTimer  < 0.0f)
		{
			// ��� ������ ������ 0, ������� ��� ������ ��������������
			FinishReload();
		}
		else
		{
			// ���� ������ �� ����� �� ����, �� ������ ����������� �� DeltaTime
			ReloadTimer -= DeltaTime;
		}
	}
}

void AWeaponDefault::DispersionTick(float DeltaTime)
{
	if (!WeaponReloading) /*Ec�� ������ �� ��������������*/
	{
		if (!WeaponFiring) /*���� ������ �� ��������*/
		{
			if (ShouldReduseDespersion) /*��������� �������*/
				CurrentDispersion = CurrentDispersion - CurrentDispersionReduction;
			else                        /*�����������*/
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
	FireTime = 0.01f;
	// ����� ������ ����������,��������� ���� ������� ����� - ���� ���� ��� ����� 0.5
	// � ��� �������� �� �������,� ������ � ��� � � ����.����� ����� 0.5 -  ������ = 0.4
	// � ��� � ������ ������ �� ������, ���� �� ������ �� 0. ����� ������ ����� ������ ��������
	//
	//
}

// �������, ������� ������� ����� ������ ����� �������� (true - ������)
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
	// � ������ ���� ��������� ������ �������,�� ���������� � ����������,
	// ���������� � �������� = 0.5, � ���� ���� ���������� 0.5
	//
	FireTime = WeaponSetting.RateOfFire;
	// � ������ �������� �������� ������� 
	WeaponInfo.Round = WeaponInfo.Round - 1; \
	ChangeDispersionByShoot();

	// ���� �����
	// SpawnSoundAtLocation() ��������� ���������: GetWorld(), ��� ����( ��� �������� � �������� SoundFireWeapon)
	// �������, � ������� ����� ������ ���� ( ������� ������� ���� (ShootLocation) 
	// ��������� ��������� �� �������
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WeaponSetting.SoundFireWeapon, ShootLocation->GetComponentLocation());
	// ������ ������
	// �� �� �����, ������ ����� ������ �� ������� EffectFireWeapon
	// � Transform ����� ��������. �������� ��� ����� �� ����, ������ �������� ����
	//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponSetting.EffectFireWeapon, ShootLocation->GetComponentTransform());
	UGameplayStatics::SpawnEmitterAttached(WeaponSetting.EffectFireWeapon, ShootLocation);
	// �-� ��� ���������
	// ����� ���������� ����
	int8 NumberProjectile = GetNumberProjectileByShot();

	if (ShootLocation)
	{
		// ��������� ���� �� � ������ shootLocation (arrow comp)

		// ����� ����� �����, ��� ������ ������������ ���� � ����� ����������					!!!
		// ����� ��� �� �������																	!!!
		FVector SpawnLocation = ShootLocation->GetComponentLocation();
		FRotator SpawnRotation = ShootLocation->GetComponentRotation();
		// ���������� ProjectileInfo, ����� ���� �� �������,������� � ���� ������� ����� ����
		// � Weapon Settings.ProjectileSettings
		FProjectileInfo ProjectileInfo;
		ProjectileInfo = GetProjectile();
		FVector EndLocation;

		// � ���� ����� ��� ������ ��������� ����, ������� ����� ���������� ��������� ������
		// ������ ����� ����� ��� ������ ������ � ��������� ����

		for (int8 i = 0; i < NumberProjectile; i++)
		{
			EndLocation = GetFireEndLocation();

			// ����� ��� ���������. ��������� � ��� ������� ������ projectile 
			if (ProjectileInfo.Projectile)
			{
				//Projectile Init ballistic fire	
				// ����� ����������, � ����� ������� ������������ ���� (� �������)
			    // ��� ����� �������� Direction(�����������), ����� �������������� �������,
				// �������� ��� �� ������� ��� ��������� Arrow Component
				// ����� ����� ������ ����������� �������������
				FVector Dir = /*ShootEndLocation*/ GetFireEndLocation() - SpawnLocation;
				Dir.Normalize();
				// ����� ������ �������, ��� �� ��������������� ������,��� ������� �� ��������� Y,Z 
				// � ������� ������ (��� ������ 0,0,0)
				// ����� ��������� �������, ����� � ������� ������� Rotator() �������� ������� ��� ����
				//
				// � ��������� �������,��� 0,1, � 0,0,1 - �� �����,���� � �� ������ �� ������
				// �������� �� 000 � 000 �������������
				FMatrix myMatrix(Dir, FVector(0, 0, 0), FVector(0, 0, 0), FVector::ZeroVector);
				SpawnRotation = myMatrix.Rotator();

				//Projectile Init ballistic fire
				// ���� projectile ����, �������� ��� ��������
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Owner = GetOwner();
				SpawnParams.Instigator = GetInstigator();

				AProjectileDefault* myProjectile = Cast<AProjectileDefault>(GetWorld()->SpawnActor(ProjectileInfo.Projectile, &SpawnLocation, &SpawnRotation, SpawnParams));
				if (myProjectile /* && myProjectile != nullptr*/ )
				{
					//ToDo Init Projectile settings by id in table row(or keep in weapon table)
					// 
					// ��������� �������, ��� ���� ������ ���� 20 ������
					//myProjectile->InitialLifeSpan = 20.0f;
					//Projectile->BulletProjectileMovement->InitialSpeed = 2500.0f;

					 myProjectile->InitProjectile(WeaponSetting.ProjectileSetting);
				}
			}
			else
			{
				// ���� ���� ���,����������� ������ �������� ��������

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

					UGameplayStatics::ApplyDamage(Hit.GetActor(),
						WeaponSetting.ProjectileSetting.ProjectileDamage,
						GetInstigatorController(),
						this,
						NULL);
				}




				////////////////////////////////////////////////////////////////////////////////////////////////////
				//MyTemplate

				/*FRotator Rot;
				FHitResult ResultHit;
				FCollisionQueryParams TraceParams;
				bool bIsHit;
				bIsHit = GetWorld()->LineTraceSingleByChannel(
						ResultHit,
						SpawnLocation,
						EndLocation,
						ECC_Visibility,
						TraceParams);

				DrawDebugLine(GetWorld(), SpawnLocation, EndLocation, FColor::Red, false, 10.0f);
				DrawDebugPoint(GetWorld(), ResultHit.ImpactPoint, 3.0f, FColor::Green, false, 10.0f);
				
				

				if (bIsHit)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Trace Hit"));
					
				}*/
				////////////////////////////////////////////////////////////////////////////////////////////////////
			}
		}
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
	// ����� ������ ������� - ������� ����� �������� �� �������� �� �������
	// ChangeDispersionByShoot ���������� � Fire();
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
	// ��� ������� ������ ����������� ������� � ������������ ���� , � �����������
	// �� ���������� GetCurrentDispersion (������� ������ �����- �� �������) �������� �� PI / 180, ����� �������� �������
	// � �������� ����� ������, ������� ����� ������� ������ �� ������. 
	// 
	// GetCurrentDispersion ����� �������� � ����������� �� ��������� ������.
	return FMath::VRandCone(DirectionShoot, GetCurrentDispersion() * PI / 180.f);
}

FVector AWeaponDefault::GetFireEndLocation() const
{
	bool bShootDirection = false;
	FVector EndLocation = FVector(0.f);
	// �������� ����� ���� �� �����������, ���� ����� ����������, �������� ������ tmpV
	FVector tmpV = (ShootLocation->GetComponentLocation() - ShootEndLocation);
	// ����� ���� ��� ������ � ���������� � ���������� ������ �����
	if (tmpV.Size() > SizeVectorToChangeShootDirectionLogic)
	{
		//EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot((ShootLocation->GetComponentLocation() - ShootEndLocation).GetSafeNormal()) * -20000.0f;
		EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot((ShootLocation->GetComponentLocation() - ShootEndLocation).GetSafeNormal()) * -20000.0f;
	}
	else
	{
		// �������� �������� ����� ����, ���� ��� ����� ��������(�������� ������)
			// ����� ����� ArrowComponent, ������� ��������� � ����  
			// � �������� �-� ApplyDispersionToShoot, �� �������� �������� �������� GetForwardVector �� ������ ArrowComponent
		//EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot(ShootLocation->GetForwardVector()) * 20000.0f;
		EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot(ShootLocation->GetForwardVector()) * 20000.0f;
	}
	return EndLocation;
}

int8 AWeaponDefault::GetNumberProjectileByShot() const
{
	// ���������� ����� ����, ������� ������ ��������
	return WeaponSetting.NumberProjectileByShot;
}

int32 AWeaponDefault::GetWeaponRound()
{
	return WeaponInfo.Round;
}
// �-� �����������
void AWeaponDefault::InitReload()
{
	WeaponReloading = true;

	// ����� ����������������� ������, �������� ��������
	// �� ���������: ����� ����� ����� ������, ����� ��������������
	ReloadTimer = WeaponSetting.ReloadTime; // (2 �������)

	// ������� ��� �������� �����������
	// �������� ��� ��������, ������� Broadcast()
	// ��� ��������� ������ "����� ���� ��� ���� �������..."
	//
	// �������� ���� ���� ��������, ������ Broadcast()
	if(WeaponSetting.AnimCharReload)
		OnWeaponReloadStart.Broadcast(WeaponSetting.AnimCharReload);
}

void AWeaponDefault::FinishReload()
{
	WeaponReloading = false;
	// ����������� ������, ���-�� �������� �����������
	WeaponInfo.Round = WeaponSetting.MaxRound;

	OnWeaponReloadEnd.Broadcast();
}

