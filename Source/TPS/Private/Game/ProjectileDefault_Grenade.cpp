// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectileDefault_Grenade.h"
#include <Kismet/GameplayStatics.h> 

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
	// ������ ������� ���������� �������
	if (TimerEnabled)
	{
		if (TimerToExplose > TimeToExlose)
		{
			// ��� ������ ������ 5 ���, ���� ������ ������
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
	// ����. ������ ���������� ������� (5�)
	TimerEnabled = true;
}

void AProjectileDefault_Grenade::Explose()
{
	TimerEnabled = false;
	// �������� �� ������
	if (ProjectileSetting.ExploseFX)
	{
		// ����� ������a
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileSetting.ExploseFX, GetActorLocation(), GetActorRotation(), FVector(1.0f));
	}
	// �������� �� ����
	if (ProjectileSetting.ExploseSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ProjectileSetting.ExploseSound, GetActorLocation());
	}
	// ������� ApplyRadialDamageWithFalloff - ��������� �������, ������� ��������� �������� �� �����
	// � ����������:
	// ������������ �����, 
	// ����������� �����, 
	// ����� ������,
	// ���������� ������(1000),
	// ������� ������� ������ (2000),����.���������� �����(5)
	// DamageType = NULL,����� ������ ������� ��������� ���� �����,
	// ��������� - ����������� � ����� ������
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

	// ����� ���� ��� ������� ����. ����� - ��� ������������
	this->Destroy();
}
