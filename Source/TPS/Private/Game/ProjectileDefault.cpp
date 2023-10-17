// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectileDefault.h"
#include <Kismet/GameplayStatics.h> 
#include <PhysicalMaterials/PhysicalMaterial.h>
#include "/My_Projects/TPS/Source/TPS/FunctionLibrary/Type.h"

// Sets default values
AProjectileDefault::AProjectileDefault()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// �������� ������ (���� ���������)
	BulletCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));

	BulletCollisionSphere->SetSphereRadius(16.f);

	// ����� ������ ���������� ��������, ����� ���������� Hit Event
	BulletCollisionSphere->bReturnMaterialOnMove = true;//hit event return physMaterial

	// ����������� ����� ����� �� ������ ������ �� ���������
	BulletCollisionSphere->SetCanEverAffectNavigation(false);//collision not affect navigation (P keybord on editor)

	// ��� ����������� ����������� ��� �����
	RootComponent = BulletCollisionSphere;

	// ������ ��� ���������, ��������� � ���� � ���� ����� �� ������ �� ��������� 
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bullet Projectile Mesh"));
	BulletMesh->SetupAttachment(RootComponent);
	BulletMesh->SetCanEverAffectNavigation(false);

	// ��������� �������� �������, ��������� � ����
	BulletFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Bullet FX"));
	BulletFX->SetupAttachment(RootComponent);

	//BulletSound = CreateDefaultSubobject<UAudioComponent>(TEXT("Bullet Audio"));
	//BulletSound->SetupAttachment(RootComponent);

	// �����������. ������ ��������� �������� ��� �����������. ��������� � ������������ �������� 
	BulletProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Bullet ProjectileMovement"));
	BulletProjectileMovement->UpdatedComponent = RootComponent;
	BulletProjectileMovement->InitialSpeed = 1.f;
	BulletProjectileMovement->MaxSpeed = 0.f;

	BulletSound = CreateDefaultSubobject<UAudioComponent>(TEXT("BulletAudio"));
	BulletSound->SetupAttachment(RootComponent);

	// ������ �� �� ��������������� � ������� ��������� (velocity)
	// ������ �� �� ����������� �� ����
	BulletProjectileMovement->bRotationFollowsVelocity = true;
	BulletProjectileMovement->bShouldBounce = true;
}

// Called when the game starts or when spawned
void AProjectileDefault::BeginPlay()
{
	Super::BeginPlay();

	// �������� ������� ������ 
	// � ����������� Hit / Begin ��� Hit ������� ��������� �������,������� ���������� � ProjectileDefault
	// � �� ������� ������� BulletCollision...
	BulletCollisionSphere->OnComponentHit.AddDynamic(this, &AProjectileDefault::BulletCollisionSphereHit);
	BulletCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AProjectileDefault::BulletCollisionSphereBeginOverlap);
	BulletCollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AProjectileDefault::BulletCollisionSphereEndOverlap);
	
}

// Called every frame
void AProjectileDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileDefault::InitProjectile(FProjectileInfo InitParam)
{
	// ����������, ����� ���������������� �������
	// ������������� �������� � ����� ����� ����
	BulletProjectileMovement->InitialSpeed = InitParam.ProjcetileInitSpeed;
	//  ���������� ����� ������������ ��������, ����� ���� ����������� ������� �������
	BulletProjectileMovement->MaxSpeed = InitParam.ProjcetileInitSpeed;
	this->SetLifeSpan(InitParam.ProjectileLifeTime);   

	// ��� ����� ���� ��������� ������������ �� ������ ��� ��� FX
	// � ���� ��� ������� �� ������ ����������, ���� ��� �� ������������00
	if (InitParam.ProjectileStaticMesh)
	{
		BulletMesh->SetStaticMesh(InitParam.ProjectileStaticMesh);
		BulletMesh->SetRelativeTransform(InitParam.ProjectileStaticMeshOffset);
	}
	else
	{
		BulletMesh->DestroyComponent();
	}

	if (InitParam.ProjectileTrialFX)
	{
		BulletFX->SetTemplate(InitParam.ProjectileTrialFX);
		BulletFX->SetRelativeTransform(InitParam.ProjectileTrialFxOffset);
	}
	else
		BulletFX->DestroyComponent();

	ProjectileSetting = InitParam;
}

void AProjectileDefault::BulletCollisionSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// ����� �������� �����-���� ����� �� ���,���� �������� �� ��� ������
	// ���� ��  � ���� ���.��������
	//  
	//
	//
	if (OtherActor && Hit.PhysMaterial.IsValid())
	{
		// ����� ��� ��������� � ������� �-��� GetSurfaceType
		EPhysicalSurface mySurfaceType = UGameplayStatics::GetSurfaceType(Hit);

		// ��������� �������� �� TMap ����� SurfaceType(��� �����������)
		if (ProjectileSetting.HitDecals.Contains(mySurfaceType))
		{
			// ���� ��������,�� �������� ����� ��������, ������� �������� ����  TMap �� mySurfaceType
			UMaterialInterface* myMaterial = ProjectileSetting.HitDecals[mySurfaceType];
			// ����� �������� �� ��������: ���� �� ���������� ������ ����, �� ��� �� ������
			if (myMaterial && OtherComp)
			{
				// ����� ������. � ��������� ����������: ������, ������ ������,
				// �� � ���� �������, ��� ������(noname), ������� (������������� �� �������)
				// � ����� ������� ������:: � ������� (������� ����� ������)
				UGameplayStatics::SpawnDecalAttached(myMaterial, FVector(20.0f), OtherComp, NAME_None, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), EAttachLocation::KeepWorldPosition, 10.0f);
			}
		}
		// �� �� ������ � HitFXs
		// ��������� mySurfaceType
		if (ProjectileSetting.HitFXs.Contains(mySurfaceType))
		{
			UParticleSystem* myParticle = ProjectileSetting.HitFXs[mySurfaceType];
			// ����� mySurfaceType
			if (mySurfaceType)
			{
				// �������, ��� ������� �������
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), myParticle, FTransform(Hit.ImpactNormal.Rotation(), Hit.ImpactPoint, FVector(1.0f)));
			}
		}

		// PlaySound, ���� ���� �����-�� ����
		if (ProjectileSetting.HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ProjectileSetting.HitSound, Hit.ImpactPoint);
		}
		// ������ ������� ���� ����� ��������� ������
		UType::AddEffecttBySurfaceType(Hit.GetActor(),Hit.BoneName, ProjectileSetting.Effect, mySurfaceType);
	}
	 
	UGameplayStatics::ApplyPointDamage(OtherActor, ProjectileSetting.ProjectileDamage, Hit.TraceStart, Hit, GetInstigatorController(), this, NULL);
	
	ImpactProjectile();
}

void AProjectileDefault::BulletCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 
}

void AProjectileDefault::BulletCollisionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 
}

void AProjectileDefault::ImpactProjectile()
{
	// ����� �������� - ���� �� ���������
	//this->Destroy();
}
