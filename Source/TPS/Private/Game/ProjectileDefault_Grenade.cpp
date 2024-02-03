// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectileDefault_Grenade.h"
#include <Kismet/GameplayStatics.h> 

AProjectileDefault_Grenade::AProjectileDefault_Grenade()
{
	//SetReplicates(true);
}

void AProjectileDefault_Grenade::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectileDefault_Grenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimerExplose(DeltaTime);
}

void AProjectileDefault_Grenade::TimerExplose(float DeltaTime)
{
	// Логика таймера взрывателя гранаты
	if (TimerEnabled)
	{
		if (TimerToExplose > TimeToExlose)
		{
			// Как только прошло 5 сек, идет логика взрыва
			Explose();
		}
		else
		{
			TimerToExplose += DeltaTime;
		}
	}
}

void AProjectileDefault_Grenade::BulletCollisionSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!TimerEnabled)
	{
		Explose();
	}

	Super::BulletCollisionSphereHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

void AProjectileDefault_Grenade::ImpactProjectile()
{
	// Иниц. таймер взрывателя гранаты (5с)
	TimerEnabled = true;
}

void AProjectileDefault_Grenade::Explose()
{
	TimerEnabled = false;
	// Проверка на эффект
	if (ProjectileSetting.ExploseFX)
	{
		// Спавн эффектa
		//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileSetting.ExploseFX, GetActorLocation(), GetActorRotation(), FVector(1.0f));

		/// /////////////////////////////// Это я писал сам, не факт что верно
		ExploseFX_Multicast(ProjectileSetting.ExploseFX,GetActorLocation(),GetActorRotation());
	}
	// Проверка на звук
	if (ProjectileSetting.ExploseSound)
	{
		//UGameplayStatics::PlaySoundAtLocation(GetWorld(), ProjectileSetting.ExploseSound, GetActorLocation());

		ExploseSound_Multicast(ProjectileSetting.ExploseSound, GetActorLocation());
		/// /////////////////////////////// Это я писал сам, не факт что верно
	}
	// Функция ApplyRadialDamageWithFalloff - серверная функция, которая позволяет общаться со всеми
	// В аргументах:
	// Максимальный дамаг, 
	// минимальный дамаг, 
	// центр взрыва,
	// внутренний радиус(1000),
	// крайний внешний радиус (2000),коэф.уменьшения урона(5)
	// DamageType = NULL,далее массив который заигнорит этот взрыв,
	// последнее - инстигейтор и дамаг каузер
	//
	TArray<AActor*> IgnoreActor;
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(),
		ProjectileSetting.ExploseMaxDamage,
		ProjectileSetting.ExploseMaxDamage*0.2f,
		GetActorLocation(),
		ProjectileSetting.ProjectileMinRadiusDamage,
		ProjectileSetting.ProjectileMaxRadiusDamage,
		5,
		NULL, IgnoreActor, this, nullptr);
	
	// Outside radius
	//DrawDebugSphere(GetWorld(), GetActorLocation(), 500.0f, 100, FColor::Green, false, 10.0f);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Outside radius,damage = 50%"));
	// 
	// Inside radius
	//DrawDebugSphere(GetWorld(), GetActorLocation(), 300.0f, 100, FColor::Red, false, 10.0f);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Inside radius,damage = Max"));
	// 
	// Save zone
	//DrawDebugSphere(GetWorld(), GetActorLocation(), 1000.0f, 100, FColor::White, false, 10.0f);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Save zone"));

	// После того как граната иниц. взрыв - она уничтожается
	this->Destroy();
}
/// /////////////////////////////// Это я писал сам, не факт что верно
void AProjectileDefault_Grenade::ExploseFX_Multicast_Implementation(UParticleSystem* FxTemplate, FVector Loc, FRotator Rot)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), 
		FxTemplate,
		Loc,
		Rot,
		FVector(1.0f));
}

void AProjectileDefault_Grenade::ExploseSound_Multicast_Implementation(USoundBase* HitSound, FVector HitLoc)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, HitLoc);
}
/// /////////////////////////////// Это я писал сам, не факт что верно