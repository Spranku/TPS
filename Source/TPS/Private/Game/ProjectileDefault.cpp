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
	BulletProjectileMovement->InitialSpeed = 1.f;
	BulletProjectileMovement->MaxSpeed = 0.f;

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
	BulletProjectileMovement->InitialSpeed = InitParam.ProjcetileInitSpeed;
	//  Выставляем сразу максимальную скорость, чтобы была возможность сделать гранаты
	BulletProjectileMovement->MaxSpeed = InitParam.ProjcetileInitSpeed;
	this->SetLifeSpan(InitParam.ProjectileLifeTime);   

	// Эта часть кода проверяет используется ли статик меш или FX
	// и если что удаляет не нужные компоненты, если они не используются00
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
				// Спавн декали. В аргументы передается: декаль, размер декали,
				// то к чему аттачим, без костей(noname), ротацию (разворачиваем по нормали)
				// В конце условие аттача:: и секунды (сколько живет декаль)
				UGameplayStatics::SpawnDecalAttached(myMaterial, FVector(20.0f), OtherComp, NAME_None, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), EAttachLocation::KeepWorldPosition, 10.0f);
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
				// Спавним, как обычные частицы
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), myParticle, FTransform(Hit.ImpactNormal.Rotation(), Hit.ImpactPoint, FVector(1.0f)));
			}
		}

		// PlaySound, если есть какой-то звук
		if (ProjectileSetting.HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ProjectileSetting.HitSound, Hit.ImpactPoint);
		}
		// Теперь обычные пули могут добавлять эффект
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
	// Когда включено - пули не рикошетят
	//this->Destroy();
}
