// Fill out your copyright notice in the Description page of Project Settings.


#include "/My_Projects/TPS/Source/TPS/Character/TPSInventoryComponent.h"
#include "/My_Projects/TPS/Source/TPS/Game/TPSGameInstance.h"

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

	// ...Этот цикл будет удален
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
				FWeaponInfo Info;
				if (myGI->GetWeaponInfoByName(WeaponSlots[i].NameItem, Info))
					WeaponSlots[i].AdditionalInfo.Round = Info.MaxRound;
				else
				{
					WeaponSlots.RemoveAt(i);
					i--;
				}
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
			OnSwitchWeapon.Broadcast(WeaponSlots[0].NameItem, WeaponSlots[0].AdditionalInfo);
	}
}


// Called every frame
void UTPSInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UTPSInventoryComponent::SwitchWeaponToIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo)
{
	// Здесь выставляется корректный индекс
	bool bIsSuccess = false; // Переменная чтобы выйти из цикла while
	int8 CorrectIndex = ChangeToIndex;
	if (ChangeToIndex > WeaponSlots.Num() - 1)
		CorrectIndex = 0;
	else
		if (CorrectIndex < 0)
			CorrectIndex = WeaponSlots.Num() - 1;

	FName NewIdWeapon;
	FAdditionalWeaponInfo NewAdditionalInfo;

	int8 i = 0;
	while (i < WeaponSlots.Num() && !bIsSuccess)
	{
		// Сравниваем слот оружия с тем что хотим поменять. Если нужное оружие найдено,оно не пустое,
		// то записываем его имя и его additional info
		if (i == CorrectIndex)
		{
			if (!WeaponSlots[i].NameItem.IsNone())
			{
				NewIdWeapon = WeaponSlots[i].NameItem;
				NewAdditionalInfo = WeaponSlots[i].AdditionalInfo;
				bIsSuccess = true;
			}
		}
		i++;
	}

	if (!bIsSuccess)
	{
		// Если не успешно
	}

	if (bIsSuccess)
	{
		// Когда всё происходит успешно, сохраняем старые данные того оружия которое поменяли.
		// Когда оружие поменялось, вызывается Broadcast
		SetAdditionalInfoWeapon(OldIndex, OldInfo);
		OnSwitchWeapon.Broadcast(NewIdWeapon, NewAdditionalInfo);
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
			}
			i++;
		}
		if (!bIsFind)
			UE_LOG(LogTemp, Warning, TEXT("UTPSInventoryComponent::SetAdditionalInfoWeapon - No Found Weapon with index - %d"), IndexWeapon);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("UTPSInventoryComponent::SetAdditionalInfoWeapon - Not correct Index weapon - %d"), IndexWeapon);
}

void UTPSInventoryComponent::WeaponChangeAmmo(EWeaponType TypeWeapon, int32 AmmoTaken)
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
			AmmoSlots[i].Cout += AmmoTaken;
			// Нужна еще проверка
			// Если Аммо слот будет больше макс (напри 101), то нужно ограничение
			if (AmmoSlots[i].Cout > AmmoSlots[i].MaxCout)
			{
				AmmoSlots[i].Cout = AmmoSlots[i].MaxCout;
			}
			OnAmmoChange.Broadcast(AmmoSlots[i].WeaponType, AmmoSlots[i].Cout);
			bIsFind = true;
		}
		i++;
	}
}

