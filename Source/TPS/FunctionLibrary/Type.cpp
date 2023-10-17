// Fill out your copyright notice in the Description page of Project Settings.


#include "Type.h"
#include "Game/TPS_StateEffect.h"
#include <TPS/TPS.h>
#include "/My_Projects/TPS/Source/TPS/Interface/TPS_IGameActor.h"

void UType::AddEffecttBySurfaceType(AActor* TakeEffectActor, FName NameBoneHit, TSubclassOf<UTPS_StateEffect> AddEffectClass, EPhysicalSurface SurfaceType)
{
	if (SurfaceType != EPhysicalSurface::SurfaceType_Default && TakeEffectActor && AddEffectClass)
	{
		// �������� ������, ������� �������� ��� ������
		//
		// ���� ��� ����������� �� ���������,�� �������� ������
		// ���� �� ����, �� ���������� � ���� ����� ���� �� ���������� ����� � ������� GetDefaultObject()
			UTPS_StateEffect* myEffect = Cast<UTPS_StateEffect>(AddEffectClass->GetDefaultObject());
			if (myEffect)
			{
				// ��� ��� � ��� ������ �������� - ����� ����
				bool bIsHavePossibleSurface = false;
				int8 i = 0;
				while (i < myEffect->PossibleInteractSurface.Num() && !bIsHavePossibleSurface)
				{
					// ��������� ������ � ���������� � ����� �����������
					if (myEffect->PossibleInteractSurface[i] == SurfaceType)
					{
						bIsHavePossibleSurface = true;
						bool bIsCanAddEffect = false;
						if (!myEffect->bIsStakable)
						{
							int8 j = 0;
							TArray<UTPS_StateEffect*> CurrentEffects;
							ITPS_IGameActor* myInterface = Cast<ITPS_IGameActor>(TakeEffectActor);
							if (myInterface)
							{
								CurrentEffects = myInterface->GetAllCurrentEffects();
							}
							if (CurrentEffects.Num() > 0)
							{
								while (j < CurrentEffects.Num() && !bIsCanAddEffect)
								{
									if (CurrentEffects[j]->GetClass() != AddEffectClass)
									{
										bIsCanAddEffect = true;
									}
									j++;
								}
							}
							else
							{
								bIsCanAddEffect = true;
							}
						}
						else
						{
							bIsCanAddEffect = true;
						}

						if (bIsCanAddEffect)
						{
							//bIsCanAdd = true; // ������� ������ �� �����
							// �������� ������� ����� NewObject, ������� ����������� Hit.GetActor()
							UTPS_StateEffect* NewEffect = NewObject<UTPS_StateEffect>(TakeEffectActor, AddEffectClass /*,FName("Effect")*/);
							// �������� � �������������
							if (NewEffect)
							{
								NewEffect->InitObject(TakeEffectActor, NameBoneHit);
							}
						}
					}
					i++;
				}
			}
		
	}
}
