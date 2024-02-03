// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectileDefault.h"
#include <Kismet/GameplayStatics.h> 
#include <PhysicalMaterials/PhysicalMaterial.h>
#include "/My_Projects/TPS/Source/TPS/FunctionLibrary/Type.h"
#include "/UE/UE_5.0/Engine/Source/Runtime/AIModule/Classes/Perception/AISense_Damage.h"

// Sets default values
AProjectileDefault::AProjectileDefault()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);

	// Создаётся объект (сфер компонент)
	BulletCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));

	BulletCollisionSphere->SetSphereRadius(16.f);

	// Сфера должна возвращать материал, когда происходит Hit Event
	BulletCollisionSphere->bReturnMaterialOnMove = true;//hit event return physMaterial

	// Создаваемая сфера никак не должна влиять на навигацию
	BulletCollisionSphere->SetCanEverAffectNavigation(false);//collision not affect navigation (P keybord on editor)

	// Рут компонентом назначается эта сфера
	RootComponent = BulletCollisionSphere;

	// Буллет меш создается, аттачится к руту и тоже никак не влияет на навигацию 
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bullet Projectile Mesh"));
	BulletMesh->SetupAttachment(RootComponent);
	BulletMesh->SetCanEverAffectNavigation(false);

	// Создается партикал система, аттачится к руту
	BulletFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Bullet FX"));
	BulletFX->SetupAttachment(RootComponent);

	//BulletSound = CreateDefaultSubobject<UAudioComponent>(TEXT("Bullet Audio"));
	//BulletSound->SetupAttachment(RootComponent);

	// Проджектайл. Апдейт компонент является рут компонентом. Начальная и максимальная скорость 
	BulletProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Bullet ProjectileMovement"));
	BulletProjectileMovement->UpdatedComponent = RootComponent;
	//BulletProjectileMovement->InitialSpeed = 1.f;
	//BulletProjectileMovement->MaxSpeed = 0.f;

	BulletSound = CreateDefaultSubobject<UAudioComponent>(TEXT("BulletAudio"));
	BulletSound->SetupAttachment(RootComponent);

	// Должен ли он разворачиваться в сторону ускорения (velocity)
	// Должен ли он отскакивать от стен
	BulletProjectileMovement->bRotationFollowsVelocity = true;
	BulletProjectileMovement->bShouldBounce = true;
}

// Called when the game starts or when spawned
void AProjectileDefault::BeginPlay()
{
	Super::BeginPlay();

	// Описание оверлап евента 
	// В зависимости Hit / Begin или Hit оверлап создается делегат,который отностится к ProjectileDefault
	// и он вызывет функцию BulletCollision...
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
	// Вызывается, когда инициализируется выстрел
	// Устанавливает скорость и время жизни пули
	//BulletProjectileMovement->InitialSpeed = InitParam.ProjcetileInitSpeed;
	//  Выставляем сразу максимальную скорость, чтобы была возможность сделать гранаты
	//BulletProjectileMovement->MaxSpeed = InitParam.ProjcetileInitSpeed;
	this->SetLifeSpan(InitParam.ProjectileLifeTime);   

	// Эта часть кода проверяет используется ли статик меш или FX
	// и если что удаляет не нужные компоненты, если они не используются00
	if (InitParam.ProjectileStaticMesh)
	{
		InitVisualMeshProjectile_Multicast(InitParam.ProjectileStaticMesh, InitParam.ProjectileStaticMeshOffset);
	}
	else
	{
		BulletMesh->DestroyComponent();
	}

	if (InitParam.ProjectileTrialFX)
	{
		InitVisualTrailProjectile_Multicast(InitParam.ProjectileTrialFX, InitParam.ProjectileTrialFxOffset);
	}
	else
		BulletFX->DestroyComponent();

	InitVelocity_Multicast(InitParam.ProjcetileInitSpeed, BulletProjectileMovement->MaxSpeed = InitParam.ProjcetileInitSpeed);

	ProjectileSetting = InitParam;
}

void AProjectileDefault::BulletCollisionSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Когда проходит какой-либо евент на хит,идет проверка на что попали
	// есть ли  у него физ.материал
	//  
	//
	//
	if (OtherActor && Hit.PhysMaterial.IsValid())
	{
		// Берем тип материала с помощью ф-ции GetSurfaceType
		EPhysicalSurface mySurfaceType = UGameplayStatics::GetSurfaceType(Hit);

		// Проверяем содержит ли TMap такой SurfaceType(тип поверхности)
		if (ProjectileSetting.HitDecals.Contains(mySurfaceType))
		{
			// Если содержит,то пытаемся взять материал, который содержит ключ  TMap по mySurfaceType
			UMaterialInterface* myMaterial = ProjectileSetting.HitDecals[mySurfaceType];
			// Далее проверка на материал: есть ли физический объект того, во что мы попали
			if (myMaterial && OtherComp)
			{
				SpawnHitDecal_Multicast(myMaterial, OtherComp, Hit);
			}
		}
		// То же делаем с HitFXs
		// Проверяем mySurfaceType
		if (ProjectileSetting.HitFXs.Contains(mySurfaceType))
		{
			UParticleSystem* myParticle = ProjectileSetting.HitFXs[mySurfaceType];
			// Берем mySurfaceType
			if (mySurfaceType)
			{
				SpawnHitFX_Multicast(myParticle,Hit);
			}
		}

		// PlaySound, если есть какой-то звук
		if (ProjectileSetting.HitSound)
		{
			SpawnHitSound_Multicast(ProjectileSetting.HitSound, Hit);
		}
		// Теперь обычные пули могут добавлять эффект
		UType::AddEffecttBySurfaceType(Hit.GetActor(),Hit.BoneName, ProjectileSetting.Effect, mySurfaceType);
	}
	 
	UGameplayStatics::ApplyPointDamage(OtherActor, ProjectileSetting.ProjectileDamage, Hit.TraceStart, Hit, GetInstigatorController(), this, NULL);
	UAISense_Damage::ReportDamageEvent(GetWorld(), Hit.GetActor(), GetInstigator(), ProjectileSetting.ProjectileDamage, Hit.Location, Hit.Location);// to do shootgun trace,grenade
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
	// Когда включено - пули не рикошетят
	//this->Destroy();
}

void AProjectileDefault::SpawnHitDecal_Multicast_Implementation(UMaterialInterface* DecalMaterial, UPrimitiveComponent* OtherComp, FHitResult HitResult)
{
	// Спавн декали. В аргументы передается: декаль, размер декали,
	// то к чему аттачим, без костей(noname), ротацию (разворачиваем по нормали)
	// В конце условие аттача:: и секунды (сколько живет декаль)
	UGameplayStatics::SpawnDecalAttached(DecalMaterial, FVector(20.0f), OtherComp, NAME_None, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation(), EAttachLocation::KeepWorldPosition, 10.0f);
}

void AProjectileDefault::SpawnHitFX_Multicast_Implementation(UParticleSystem* FxTemplate, FHitResult HitResult)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FxTemplate, FTransform(HitResult.ImpactNormal.Rotation(), HitResult.ImpactPoint, FVector(1.0f)));
}

void AProjectileDefault::SpawnHitSound_Multicast_Implementation(USoundBase* HitSound, FHitResult HitResult)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, HitResult.ImpactPoint);
}



void AProjectileDefault::InitVisualMeshProjectile_Multicast_Implementation(UStaticMesh* newMesh, FTransform MeshRelative)
{
	BulletMesh->SetStaticMesh(newMesh);
	BulletMesh->SetRelativeTransform(MeshRelative);
}

void AProjectileDefault::InitVisualTrailProjectile_Multicast_Implementation(UParticleSystem* NewTemplate, FTransform TemplateRelative)
{
	BulletFX->SetTemplate(NewTemplate);
	BulletFX->SetRelativeTransform(TemplateRelative);
}

void AProjectileDefault::InitVelocity_Multicast_Implementation(float InitSpeed, float MaxSpeed)
{
	if (BulletProjectileMovement)
	{
		BulletProjectileMovement->Velocity = GetActorForwardVector() * InitSpeed;
		BulletProjectileMovement->MaxSpeed = MaxSpeed;
		BulletProjectileMovement->InitialSpeed = InitSpeed;
	}
}

void AProjectileDefault::PostNetReceiveVelocity(const FVector& NewVelocity)
{
	if (BulletProjectileMovement)
	{
		BulletProjectileMovement->Velocity = NewVelocity;
	}
}