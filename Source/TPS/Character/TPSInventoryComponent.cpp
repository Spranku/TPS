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

	// ...���� ���� ����� ������
	//	������ ���� ������ ������������� ����� � ������� ��.
	// ��� �� �� ��������� AdditionalInfo � WeaponSlots �� MaxRound ��������
	// ����� ����� ����������� ����. ���������, � ��������� ���������� ���������� ������
	// � ������� ��������. ���� ��� �� ��������� �� ��������� - ��� ���������
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

	// ����� ���������� �������
	// ������ �������� ������ ������� ��.������� ��� �� �� ������,�� �������
	// � ���������� ��������� � ����������� IdName � AdditionalInfo
	// ���� ��������� ����� � Character
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
	// ����� ������������ ���������� ������
	bool bIsSuccess = false; // ���������� ����� ����� �� ����� while
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
		// ���������� ���� ������ � ��� ��� ����� ��������. ���� ������ ������ �������,��� �� ������,
		// �� ���������� ��� ��� � ��� additional info
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
		// ���� �� �������
	}

	if (bIsSuccess)
	{
		// ����� �� ���������� �������, ��������� ������ ������ ���� ������ ������� ��������.
		// ����� ������ ����������, ���������� Broadcast
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
			// ���� ������ ������ � �����
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

// ������ �-� ����������� ���� �������� ���� �������� ���� � ������������, ����� ������� ����� ��������� �������
// ����� ����� ����� �������� ������, �������� ������� ����� ���������.
// 
void UTPSInventoryComponent::SetAdditionalInfoWeapon(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo)
{
	// ����������� �� �������
	// ��������� ������� �� ������
	if (WeaponSlots.IsValidIndex(IndexWeapon))
	{
		bool bIsFind = false;
		int8 i = 0;
		while (i < WeaponSlots.Num() && !bIsFind)
		{
			// ���� ������ ������ � �����
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
	// ������� ����, ���������� �
	// ������� ���� �� ���� For Each ��� � ��
	// 
	bool bIsFind = false;
	int8 i = 0;
	
	while (i<AmmoSlots.Num() && !bIsFind)
	{
		// ������ ����� ����� ����� ������ ���� �� ���� ������,
		// �������� � ����� ������� �������� �� ���������� 
		// ����� � AmmoSlots ����� ��� Cout � ������� �� ��� ����� (AmmoTaken) 
		//

		if (AmmoSlots[i].WeaponType == TypeWeapon)
		{
			AmmoSlots[i].Cout += AmmoTaken;
			// ����� ��� ��������
			// ���� ���� ���� ����� ������ ���� (����� 101), �� ����� �����������
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

