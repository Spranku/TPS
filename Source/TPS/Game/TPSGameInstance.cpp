// Fill out your copyright notice in the Description page of Project Settings.


#include "/My_Projects/TPS/Source/TPS/Game/TPSGameInstance.h"

bool UTPSGameInstance::GetWeaponInfoByName(FName NameWeapon, FWeaponInfo& OutInfo)
{
	bool bIsFind = false;
	FWeaponInfo* WeaponInfoRow;

	// ѕроверка на наличие таблицы
	if (WeaponInfoTable)
	{
		// »щем таблицу (FindRow), на выходе нужна структура FWeaponInfo, дальше им€ таблицы
		// »щем строку с NameWeapon именем, без какого-либо контекста,игнорируем  warning`и
		// ѕолучаем ссылку на структуру, FinRow даЄт €чейку в пам€ти,где находитс€ эта структура
		// а в ней уже переменные
		WeaponInfoRow = WeaponInfoTable->FindRow<FWeaponInfo>(NameWeapon, "", false);
		// ѕроверка нашли ли мы эту строку
		if (WeaponInfoRow)
		{
			//  ≈сли нашли, становитс€ true
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
		// ‘-€ GetRowNames возвращает все строки в таблице 
		FDropItem* DropItemInfoRow;
		TArray<FName>RowNames = DropItemInfoTable->GetRowNames();

		int8 i = 0;
		// ѕеребираем все RowNames, которые получили выше, пока не найдем нужный
		// »щем р€д в структуре FDropItem,в аргументы кидаем каждое им€ строки,контекст можно оставить пустым
		while (i < RowNames.Num() && !bIsFind)
		{
			DropItemInfoRow = DropItemInfoTable->FindRow<FDropItem>(RowNames[i], "");
			// ƒалее берем то что получили, им€ предмета и если он совпадает с именем которые хотим получить
			// OutInfo - это структура, а т.к. мы работаем со структурными данными в виде таблицы
			// FindRow возвращает путь на €чейку пам€ти,где есть информаци€ нашей структуры. 
			// “аким синтаксисом OutInfo = (*DropItemInfoRow) мы преобразуем еЄ в выходную информацию
			// в виде простой структуры.
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
