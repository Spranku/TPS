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