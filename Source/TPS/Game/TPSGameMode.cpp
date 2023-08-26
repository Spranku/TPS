// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSGameMode.h"
#include "TPSPlayerController.h"
#include <TPS/Character/TPSCharacter.h>
#include "UObject/ConstructorHelpers.h"

ATPSGameMode::ATPSGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ATPSPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	// static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprint/Character/BP_Character"));
	// if (PlayerPawnBPClass.Class != nullptr)
	// {
	// 	DefaultPawnClass = PlayerPawnBPClass.Class;
	// }

	

	//set default controller to our Blueprinted controller
	//static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/Blueprint/Character/BP_PlayerController"));
	//if(PlayerControllerBPClass.Class != NULL)
	//{
	//	PlayerControllerClass = PlayerControllerBPClass.Class;
		//Game/Blueprint/Character/BP_TopDownPlayerController
	//}
}

void ATPSGameMode::PlayerCharacterDead()
{

}
