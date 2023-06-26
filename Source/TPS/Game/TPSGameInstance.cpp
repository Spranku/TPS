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

bool UTPSGameInstance::GetDropItemInfoByWeaponName(FName NameItem, FDropItem& OutInfo)
{
	bool bIsFind = false;

	if (DropItemInfoTable)
	{
		// �-� GetRowNames ���������� ��� ������ � ������� 
		FDropItem* DropItemInfoRow;
		TArray<FName>RowNames = DropItemInfoTable->GetRowNames();

		int8 i = 0;
		// ���������� ��� RowNames, ������� �������� ����, ���� �� ������ ������
		// ���� ��� � ��������� FDropItem,� ��������� ������ ������ ��� ������,�������� ����� �������� ������
		while (i < RowNames.Num() && !bIsFind)
		{
			DropItemInfoRow = DropItemInfoTable->FindRow<FDropItem>(RowNames[i], "");
			// ����� ����� �� ��� ��������, ��� �������� � ���� �� ��������� � ������ ������� ����� ��������
			// OutInfo - ��� ���������, � �.�. �� �������� �� ������������ ������� � ���� �������
			// FindRow ���������� ���� �� ������ ������,��� ���� ���������� ����� ���������. 
			// ����� ����������� OutInfo = (*DropItemInfoRow) �� ����������� � � �������� ����������
			// � ���� ������� ���������.
			if (DropItemInfoRow->WeaponInfo.NameItem == NameItem)
			{
				OutInfo = (*DropItemInfoRow);
				bIsFind = true;
			}
			i++;//fix
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTPSGameInstance::GetDropItemInfoByWeaponName - DropItemInfoTable -NULL"));
	}
	return bIsFind;
}

bool UTPSGameInstance::GetDropItemInfoByName(FName NameItem, FDropItem& OutInfo)
{
	bool bIsFind = false;
	FDropItem* DropItemInfoRow;

	if (DropItemInfoTable)
	{
		DropItemInfoRow = DropItemInfoTable->FindRow<FDropItem>(NameItem, "", false);
		if (DropItemInfoRow)
		{
			bIsFind = true;
			OutInfo = *DropItemInfoRow;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTPSGameInstance::GetWeaponInfoByName - WeaponTable -NULL"));
	}
	return bIsFind;
}
