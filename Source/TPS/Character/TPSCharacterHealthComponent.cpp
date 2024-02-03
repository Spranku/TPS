// Fill out your copyright notice in the Description page of Project Settings.


#include </My_Projects/TPS/Source/TPS/Character/TPSCharacterHealthComponent.h>

void UTPSCharacterHealthComponent::ChangeHealthValue_OnServer(float ChangeValue)
{
	//������ ����

	// ������� ����� ���������� �� ����.������
	float CurrentDamage = ChangeValue * CoefDamage;

	if (Shield > 0.0f && ChangeValue < 0.0f)
	{
		// ������� ����� ���������� � �-� ��������� �������� ����
		ChangeShieldValue(ChangeValue);
		if (Shield < 0.0f)
		{
			// FX
			UE_LOG(LogTemp, Warning, TEXT("UTPSCharacterHealthComponent::ChangeHealthValue - Shield < 0"));
		}
	}
	else
	{
		Super::ChangeHealthValue_OnServer(ChangeValue);
	}
}

float UTPSCharacterHealthComponent::GetCurrentShield()
{
	return Shield;
}

void UTPSCharacterHealthComponent::ChangeShieldValue(float ChangeValue)
{
	// ���������� ���� �������� �������� ������
	Shield += ChangeValue;

	ShieldChangeEvent_Multicast(Shield, ChangeValue);
	//OnShieldChange.Broadcast(Shield, ChangeValue);
	
	// ���� ��� ����� �� ������� 100, �� ������� ������� 100
	if (Shield > 100.0f)
	{
		Shield = 100.0f;
	}
	else // ���� ��� ������ 0, �� �������� 0.
	{
		if (Shield < 0.0f)
		{
			Shield = 0.0f;
		}
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_CoolDownShieldTimer,
			this,
			&UTPSCharacterHealthComponent::CoolDownShieldEnd,
			CoolDownShieldRecoveryTime,
			false);

		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ShieldRecoveryRateTimer);
	}
}

void UTPSCharacterHealthComponent::CoolDownShieldEnd()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ShieldRecoveryRateTimer,
			this,
			&UTPSCharacterHealthComponent::RecovryShield,
			ShieldRecoveryRate,
			true);
	}
}

void UTPSCharacterHealthComponent::RecovryShield()
{
	float tmp = Shield;
	tmp = tmp + ShieldRecoveryValue;
	if (tmp > 100.0f)
	{
		Shield = 100.0f;
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ShieldRecoveryRateTimer);
		}
	}
	else
	{
		Shield = tmp;
	}
	ShieldChangeEvent_Multicast(Shield, ShieldRecoveryValue);
	//OnShieldChange.Broadcast(Shield, ShieldRecoveryValue); 
}

float UTPSCharacterHealthComponent::GetShieldValue()
{
	return Shield;
}

void UTPSCharacterHealthComponent::ShieldChangeEvent_Multicast_Implementation(float newShield, float Damage)
{
	OnShieldChange.Broadcast(newShield, Damage);
}
