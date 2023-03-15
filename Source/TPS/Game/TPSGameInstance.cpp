// Fill out your copyright notice in the Description page of Project Settings.


#include "/My_Projects/TPS/Source/TPS/Game/TPSGameInstance.h"

bool UTPSGameInstance::GetWeaponInfoByName(FName NameWeapon, FWeaponInfo& OutInfo)
{
	bool bIsFind = false;
	FWeaponInfo* WeaponInfoRow;

	// �������� �� ������� �������
	if (WeaponInfoTable)
	{
		// ���� ������� (FindRow), �� ������ ����� ��������� FWeaponInfo, ������ ��� �������
		// ���� ������ � NameWeapon ������, ��� ������-���� ���������,����������  warning`�
		// �������� ������ �� ���������, FinRow ��� ������ � ������,��� ��������� ��� ���������
		// � � ��� ��� ����������
		WeaponInfoRow = WeaponInfoTable->FindRow<FWeaponInfo>(NameWeapon, "", false);
		// �������� ����� �� �� ��� ������
		if (WeaponInfoRow)
		{
			//  ���� �����, ���������� true
			bIsFind = true;
			OutInfo = *WeaponInfoRow;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTPSGameInstance::GetWeaponInfoByName - WeaponTable -NULL"));
	}
	return bIsFind;
}