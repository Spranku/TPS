// Fill out your copyright notice in the Description page of Project Settings.


#include "/My_Projects/TPS/Source/TPS/Character/TPSInventoryComponent.h"
#include "/My_Projects/TPS/Source/TPS/Game/TPSGameInstance.h"
#pragma optimize ("", off)

// Sets default values for this component's properties
UTPSInventoryComponent::UTPSInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UTPSInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UTPSInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UTPSInventoryComponent::SwitchWeaponToIndexByNextPreviosIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo, bool bIsForward)
{
	bool bIsSuccess = false;
	int8 CorrectIndex = ChangeToIndex;
	if (ChangeToIndex > WeaponSlots.Num() - 1)
		CorrectIndex = 0;
	else
		if (ChangeToIndex < 0)
			CorrectIndex = WeaponSlots.Num() - 1;

	FName NewIdWeapon;
	FAdditionalWeaponInfo NewAdditionalInfo;
	int32 NewCurrentIndex = 0;

	if (WeaponSlots.IsValidIndex(CorrectIndex))
	{
		if (!WeaponSlots[CorrectIndex].NameItem.IsNone())
		{
			if (WeaponSlots[CorrectIndex].AdditionalInfo.Round > 0)
			{
				//good weapon have ammo start change
				bIsSuccess = true;
			}
			else
			{
				UTPSGameInstance* myGI = Cast<UTPSGameInstance>(GetWorld()->GetGameInstance());
				if (myGI)
				{
					//check ammoSlots for this weapon
					FWeaponInfo myInfo;
					myGI->GetWeaponInfoByName(WeaponSlots[CorrectIndex].NameItem, myInfo);

					bool bIsFind = false;
					int8 j = 0;
					while (j < AmmoSlots.Num() && !bIsFind)
					{
						if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
						{
							//good weapon have ammo start change
							bIsSuccess = true;
							bIsFind = true;
						}
						j++;
					}
				}
			}
			if (bIsSuccess)
			{
				NewCurrentIndex = CorrectIndex;
				NewIdWeapon = WeaponSlots[CorrectIndex].NameItem;
				NewAdditionalInfo = WeaponSlots[CorrectIndex].AdditionalInfo;
			}
		}
	}
	if (!bIsSuccess)
	{
		int8 iteration = 0;
		int8 Seconditeration = 0;
		int8 tmpIndex = 0;
		while (iteration < WeaponSlots.Num() && !bIsSuccess)
		{
			iteration++;

			if (bIsForward)
			{
				//Seconditeration = 0;

				tmpIndex = ChangeToIndex + iteration;
			}
			else
			{
				Seconditeration = WeaponSlots.Num() - 1;

				tmpIndex = ChangeToIndex - iteration;
			}

			if (WeaponSlots.IsValidIndex(tmpIndex))
			{
				if (!WeaponSlots[tmpIndex].NameItem.IsNone())
				{
					if (WeaponSlots[tmpIndex].AdditionalInfo.Round > 0)
					{
						//WeaponGood
						bIsSuccess = true;
						NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
						NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
						NewCurrentIndex = tmpIndex;
					}
					else
					{
						FWeaponInfo myInfo;
						UTPSGameInstance* myGI = Cast<UTPSGameInstance>(GetWorld()->GetGameInstance());

						myGI->GetWeaponInfoByName(WeaponSlots[tmpIndex].NameItem, myInfo);

						bool bIsFind = false;
						int8 j = 0;
						while (j < AmmoSlots.Num() && !bIsFind)
						{
							if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
							{
								//WeaponGood
								bIsSuccess = true;
								NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
								NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
								NewCurrentIndex = tmpIndex;
								bIsFind = true;
							}
							j++;
						}
					}
				}
			}
			else
			{
				//go to end of LEFT of array weapon slots
				if (OldIndex != Seconditeration)
				{
					if (WeaponSlots.IsValidIndex(Seconditeration))
					{
						if (!WeaponSlots[Seconditeration].NameItem.IsNone())
						{
							if (WeaponSlots[Seconditeration].AdditionalInfo.Round > 0)
							{
								//WeaponGood
								bIsSuccess = true;
								NewIdWeapon = WeaponSlots[Seconditeration].NameItem;
								NewAdditionalInfo = WeaponSlots[Seconditeration].AdditionalInfo;
								NewCurrentIndex = Seconditeration;
							}
							else
							{
								FWeaponInfo myInfo;
								UTPSGameInstance* myGI = Cast<UTPSGameInstance>(GetWorld()->GetGameInstance());

								myGI->GetWeaponInfoByName(WeaponSlots[Seconditeration].NameItem, myInfo);

								bool bIsFind = false;
								int8 j = 0;
								while (j < AmmoSlots.Num() && !bIsFind)
								{
									if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
									{
										//WeaponGood
										bIsSuccess = true;
										NewIdWeapon = WeaponSlots[Seconditeration].NameItem;
										NewAdditionalInfo = WeaponSlots[Seconditeration].AdditionalInfo;
										NewCurrentIndex = Seconditeration;
										bIsFind = true;
									}
									j++;
								}
							}
						}
					}
				}
				else
				{
					//go to same weapon when start
					if (WeaponSlots.IsValidIndex(Seconditeration))
					{
						if (!WeaponSlots[Seconditeration].NameItem.IsNone())
						{
							if (WeaponSlots[Seconditeration].AdditionalInfo.Round > 0)
							{
								//WeaponGood, it same weapon do nothing
							}
							else
							{
								FWeaponInfo myInfo;
								UTPSGameInstance* myGI = Cast<UTPSGameInstance>(GetWorld()->GetGameInstance());

								myGI->GetWeaponInfoByName(WeaponSlots[Seconditeration].NameItem, myInfo);

								bool bIsFind = false;
								int8 j = 0;
								while (j < AmmoSlots.Num() && !bIsFind)
								{
									if (AmmoSlots[j].WeaponType == myInfo.WeaponType)
									{
										if (AmmoSlots[j].Cout > 0)
										{
											//WeaponGood, it same weapon do nothing
										}
										else
										{
											//Not find weapon with ammo need init Pistol with infinity ammo
											UE_LOG(LogTemp, Error, TEXT("UTPSInventoryComponent::SwitchWeaponToIndex - Init PISTOL - NEED"));
										}
									}
									j++;
								}
							}
						}
					}
				}
				if (bIsForward)
				{
					Seconditeration++;
				}
				else
				{
					Seconditeration--;
				}

			}
		}
	}
	if (bIsSuccess)
	{
		SetAdditionalInfoWeapon(OldIndex, OldInfo);
		OnSwitchWeapon.Broadcast(NewIdWeapon, NewAdditionalInfo, NewCurrentIndex);
	}

	return bIsSuccess;
}

bool UTPSInventoryComponent::SwitchWeaponByIndex(int32 IndexWeaponToChange, int32 PreviosIndex, FAdditionalWeaponInfo PreviosWeaponInfo)
{
	bool bIsSuccess = false;
	FName ToSwitchIdWeapon;
	FAdditionalWeaponInfo ToSwitchAdditionalInfo;

	ToSwitchIdWeapon = GetWeaponNameBySlotIndex(IndexWeaponToChange);
	ToSwitchAdditionalInfo = GetAdditionalInfoWeapon(IndexWeaponToChange);

	if (!ToSwitchIdWeapon.IsNone())
	{
		SetAdditionalInfoWeapon(PreviosIndex, PreviosWeaponInfo);
		OnSwitchWeapon.Broadcast(ToSwitchIdWeapon, ToSwitchAdditionalInfo, IndexWeaponToChange);

		//check ammo slot for event to player		
		EWeaponType ToSwitchWeaponType;
		if (GetWeaponTypeByNameWeapon(ToSwitchIdWeapon, ToSwitchWeaponType))
		{
			int8 AviableAmmoForWeapon = -1;
			if (CheckAmmoForWeapon(ToSwitchWeaponType, AviableAmmoForWeapon))
			{

			}
		}
		bIsSuccess = true;
	}
	return bIsSuccess;
}

FAdditionalWeaponInfo UTPSInventoryComponent::GetAdditionalInfoWeapon(int32 IndexWeapon)
{
	FAdditionalWeaponInfo result;
	if (WeaponSlots.IsValidIndex(IndexWeapon))
	{
		bool bIsFind = false;
		int8 i = 0;
		while (i < WeaponSlots.Num() && !bIsFind)
		{
			// Ищем нужный индекс в слоту
			if (/*WeaponSlots[i].IndexSlot*/ i == IndexWeapon)
			{
				result = WeaponSlots[i].AdditionalInfo;
				bIsFind = true;
			}
			i++;
		}
		if (!bIsFind)
			UE_LOG(LogTemp, Warning, TEXT("UTPSInventoryComponent::GetAdditionalInfoWeapon - No Found Weapon with index - %d"), IndexWeapon);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("UTPSInventoryComponent::GetAdditionalInfoWeapon - Not correct Index weapon - %d"), IndexWeapon);
	return result;
}

int32 UTPSInventoryComponent::GetWeaponIndexSlotByName(FName IdWeaponName)
{
	int32 result = -1;
	int8 i = 0;
	bool bIsFind = false;
	while (i < WeaponSlots.Num() && !bIsFind)
	{
		if (WeaponSlots[i].NameItem == IdWeaponName)
		{
			bIsFind = true;
			result = i /*WeaponSlots[i].IndexSlot*/;
		}
		i++;
	}
	return result;
}

bool UTPSInventoryComponent::GetWeaponTypeByIndexSlot(int32 IndexSlot, EWeaponType& WeaponType)
{
	bool bIsFind = false;
	FWeaponInfo OutInfo;
	WeaponType = EWeaponType::RifleType;
	UTPSGameInstance* myGI = Cast<UTPSGameInstance>(GetWorld()->GetGameInstance());
	if (myGI)
	{
		if (WeaponSlots.IsValidIndex(IndexSlot))
		{
			myGI->GetWeaponInfoByName(WeaponSlots[IndexSlot].NameItem, OutInfo);
			WeaponType = OutInfo.WeaponType;
			bIsFind = true;
		}
	}
	return bIsFind;
}

bool UTPSInventoryComponent::GetWeaponTypeByNameWeapon(FName IdWeaponName, EWeaponType& WeaponType)
{
	bool bIsFind = false;
	FWeaponInfo OutInfo;
	WeaponType = EWeaponType::RifleType;
	UTPSGameInstance* myGI = Cast<UTPSGameInstance>(GetWorld()->GetGameInstance());
	if (myGI)
	{
		myGI->GetWeaponInfoByName(IdWeaponName, OutInfo);
		WeaponType = OutInfo.WeaponType;
		bIsFind = true;
	}
	return bIsFind;
}

FName UTPSInventoryComponent::GetWeaponNameBySlotIndex(int32 IndexSlot)
{
	FName result;
	
	if (WeaponSlots.IsValidIndex(IndexSlot))
	{
		result = WeaponSlots[IndexSlot].NameItem;
	}
	return result;
}

// Данная ф-я понадобится если появится если появится ящик с боеприпасами, через которые можно пополнить патроны
// Тогда будем брать нынещнее оружие, вызывать функцию через инвентарь.
// 
void UTPSInventoryComponent::SetAdditionalInfoWeapon(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo)
{
	// Пробегаемся по индексу
	// Проверяем валиден ли индекс
	if (WeaponSlots.IsValidIndex(IndexWeapon))
	{
		bool bIsFind = false;
		int8 i = 0;
		while (i < WeaponSlots.Num() && !bIsFind)
		{
			// Ищем нужный индекс в слоту
			if (/*WeaponSlots[i].IndexSlot*/ i == IndexWeapon)
			{
				WeaponSlots[i].AdditionalInfo = NewInfo;
				bIsFind = true;

				OnWeaponAdditionalInfoChange.Broadcast(IndexWeapon, NewInfo);
			}
			i++;
		}
		if (!bIsFind)
			UE_LOG(LogTemp, Warning, TEXT("UTPSInventoryComponent::SetAdditionalInfoWeapon - No Found Weapon with index - %d"), IndexWeapon);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("UTPSInventoryComponent::SetAdditionalInfoWeapon - Not correct Index weapon - %d"), IndexWeapon);
}

void UTPSInventoryComponent::AmmoSlotChangeValue(EWeaponType TypeWeapon, int32 CoutChangeAmmo)
{
	// Создаем флаг, переменную ш
	// Создаем цикл по сути For Each как в ВР
	// 
	bool bIsFind = false;
	int8 i = 0;
	while (i<AmmoSlots.Num() && !bIsFind)
	{
		// Прежде всего нужно найти нужный слот по типу оружия,
		// сравнить с типом который приходит от переменной 
		// Далее в AmmoSlots берем его Cout и плюсуем то что взяли (AmmoTaken) 
		//

		if (AmmoSlots[i].WeaponType == TypeWeapon)
		{
			AmmoSlots[i].Cout += CoutChangeAmmo;
			// Нужна еще проверка
			// Если Аммо слот будет больше макс (напри 101), то нужно ограничение
			if (AmmoSlots[i].Cout > AmmoSlots[i].MaxCout)
				AmmoSlots[i].Cout = AmmoSlots[i].MaxCout;
			
			OnAmmoChange.Broadcast(AmmoSlots[i].WeaponType, AmmoSlots[i].Cout);
			bIsFind = true;
		}
		i++;
	}
}

bool UTPSInventoryComponent::CheckAmmoForWeapon(EWeaponType TypeWeapon, int8 &AviableAmmoForWeapon)
{
	AviableAmmoForWeapon = 0; 
	bool bIsFind = false;
	int8 i = 0;
	while (i < AmmoSlots.Num() && !bIsFind)
	{
		if (AmmoSlots[i].WeaponType == TypeWeapon)
		{
			bIsFind = true;
			AviableAmmoForWeapon = AmmoSlots[i].Cout;
			if (AmmoSlots[i].Cout > 0)
			{
				return true;
			}
		}
		i++;
	}

	if (AviableAmmoForWeapon <= 0)
	{
		OnWeaponAmmoEmpty.Broadcast(TypeWeapon);
	}
	else
	{
		OnWeaponAmmoAviable.Broadcast(TypeWeapon);
	}
	
	return false;
}

bool UTPSInventoryComponent::CheckCanTakeAmmo(EWeaponType AmmoType)
{
	bool result = false;
	int8 i = 0;
	while (i < AmmoSlots.Num() && !result)
	{
		if (AmmoSlots[i].WeaponType == AmmoType && AmmoSlots[i].Cout < AmmoSlots[i].MaxCout)
			result = true;
		i++;
	}
	return result;
}

bool UTPSInventoryComponent::CheckCanTakeWeapon(int32 &FreeSlot)
{
	bool bIsFreeSlot = false;
	int8 i = 0;
	while (i < WeaponSlots.Num() && !bIsFreeSlot)
	{
		if (WeaponSlots[i].NameItem.IsNone())
		{
			// Если найден хотя-бы один свободный слот,то считаем что можем поднять оружие 
			bIsFreeSlot = true;
			FreeSlot = i;
		}
		i++; 
	}
	return bIsFreeSlot;
}

bool UTPSInventoryComponent::SwitchWeaponToInventory(FWeaponSlot NewWeapon, int32 IndexSlot,int32 CurrentIndexWeaponChar, FDropItem& DropItemInfo)
{
	bool result = false;
	
	if (WeaponSlots.IsValidIndex(IndexSlot) && GetDropItemInfoFromInventory(IndexSlot,DropItemInfo))
	{
		WeaponSlots[IndexSlot] = NewWeapon;
		SwitchWeaponToIndexByNextPreviosIndex(CurrentIndexWeaponChar,-1,NewWeapon.AdditionalInfo,true);
		OnUpdateWeaponSlots.Broadcast(IndexSlot, NewWeapon);
		result = true;
	}
	return result;
}

bool UTPSInventoryComponent::TryGetWeaponToInventory(FWeaponSlot NewWeapon)
{
	int32 IndexSlot = -1;
	if (CheckCanTakeWeapon(IndexSlot))
	{
		if (WeaponSlots.IsValidIndex(IndexSlot))
		{
			WeaponSlots[IndexSlot] = NewWeapon;
			OnUpdateWeaponSlots.Broadcast(IndexSlot,NewWeapon);
			return true;
		}	
	}
	return false;
}

bool UTPSInventoryComponent::GetDropItemInfoFromInventory(int32 IndexSlot, FDropItem& DropItemInfo)
{
	bool result = false;
	FName DropItemName = GetWeaponNameBySlotIndex(IndexSlot);

	UTPSGameInstance* myGI = Cast<UTPSGameInstance>(GetWorld()->GetGameInstance());
	if (myGI)
	{
		result = myGI->GetDropItemInfoByWeaponName(DropItemName,DropItemInfo);
		if (WeaponSlots.IsValidIndex(IndexSlot))
		{
			DropItemInfo.WeaponInfo.AdditionalInfo = WeaponSlots[IndexSlot].AdditionalInfo;
		}
	}
	return result;
}

TArray<FWeaponSlot> UTPSInventoryComponent::GetWeaponSlots()
{
	return WeaponSlots;
}

TArray<FAmmoSlot> UTPSInventoryComponent::GetAmmoSlots()
{
	return AmmoSlots;
}

void UTPSInventoryComponent::InitInventory(TArray<FWeaponSlot> NewWeaponSlotsInfo, TArray<FAmmoSlot> NewAmmoSlotsInfo)
{
	// Записываем слоты на новые
	WeaponSlots = NewWeaponSlotsInfo;
	AmmoSlots = NewAmmoSlotsInfo;

	//	Данный цикл чистит незаполненные слоты и удаляет их.
	// Так же он заполняет AdditionalInfo в WeaponSlots по MaxRound значению
	// Чтобы когда происходила иниц. инвентаря, в инвентаре появлялось корректное оружие
	// с запасом патронов. Если что то заполнено не правильно - это удаляется
	//
	for (int i = 0; i < WeaponSlots.Num(); i++)
	{
		UTPSGameInstance* myGI = Cast<UTPSGameInstance>(GetWorld()->GetGameInstance());
		if (myGI)
		{
			if (!WeaponSlots[i].NameItem.IsNone())
			{
				//FWeaponInfo Info;
				//if (myGI->GetWeaponInfoByName(WeaponSlots[i].NameItem, Info))
					//WeaponSlots[i].AdditionalInfo.Round = Info.MaxRound;
			}
		}
	}
	MaxSlotsWeapon = WeaponSlots.Num();

	// Далее происходит делегат
	// Делаем проверку самого первого эл.массива что он не пустой,он валиден
	// и вызывается броадкаст с параметрами IdName и AdditionalInfo
	// Этот броадкаст пойдёт в Character
	if (WeaponSlots.IsValidIndex(0))
	{
		if (!WeaponSlots[0].NameItem.IsNone())
			OnSwitchWeapon.Broadcast(WeaponSlots[0].NameItem, WeaponSlots[0].AdditionalInfo, 0);
	}
}

#pragma optimize ("", on)