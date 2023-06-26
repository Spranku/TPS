// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include <TPS/Character/TPSCharacter.h>
#include "Engine/World.h"

ATPSPlayerController::ATPSPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ATPSPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		//MoveToMouseCursor();
	}

	//if(bInputPressed)
	//{
	//	FollowTime += DeltaTime;

	//	// Look for the touch location
	//	FVector HitLocation = FVector::ZeroVector;
	//	FHitResult Hit;
	//	if(bIsTouch)
	//	{
	//		GetHitResultUnderFinger(ETouchIndex::Touch1, ECC_Visibility, true, Hit);
	//	}
	//	else
	//	{
	//		GetHitResultUnderCursor(ECC_Visibility, true, Hit);
	//	}
	//	HitLocation = Hit.Location;

	//	// Direct the Pawn towards that location
	//	APawn* const MyPawn = GetPawn();
	//	if(MyPawn)
	//	{
	//		FVector WorldDirection = (HitLocation - MyPawn->GetActorLocation()).GetSafeNormal();
	//		MyPawn->AddMovementInput(WorldDirection, 1.f, false);
	//	}
	//}
	//else
	//{
	//	FollowTime = 0.f;
	//}
}

void ATPSPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ATPSPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ATPSPlayerController::OnSetDestinationReleased);

	// support touch devices 
	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ATPSPlayerController::OnTouchPressed);
	//InputComponent->BindTouch(EInputEvent::IE_Released, this, &ATPSPlayerController::OnTouchReleased);

	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ATPSPlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ATPSPlayerController::MoveToTouchLocation);
}

void ATPSPlayerController::MoveToMouseCursor()
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		if (ATPSCharacter* MyPawn = Cast<ATPSCharacter>(GetPawn()))
		{
			if (MyPawn->GetCursorToWorld())
			{
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, MyPawn->GetCursorToWorld()->GetComponentLocation());
			}
		}
	}
	else
	{
		// Trace to see what is under the mouse cursor
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		if (Hit.bBlockingHit)
		{
			// We hit something, move there
			SetNewMoveDestination(Hit.ImpactPoint);
		}
	}
}

void ATPSPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}

}

void ATPSPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if ((Distance > 120.0f))
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void ATPSPlayerController::OnSetDestinationPressed()
{
	bMoveToMouseCursor = true;

	//// We flag that the input is being pressed
	//bInputPressed = true;
	// Just in case the character was moving because of a previous short press we stop it
	//StopMovement();
}

void ATPSPlayerController::OnSetDestinationReleased()
{
	bMoveToMouseCursor = false;
	// Player is no longer pressing the input
	//bInputPressed = false;

	// If it was a short press
	//if(FollowTime <= ShortPressThreshold)
	//{
		// We look for the location in the world where the player has pressed the input
	//	FVector HitLocation = FVector::ZeroVector;
	//	FHitResult Hit;
	//	GetHitResultUnderCursor(ECC_Visibility, true, Hit);
	//	HitLocation = Hit.Location;

	// We move there and spawn some particles
	//	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, HitLocation);
	//	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, HitLocation, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	//}
}

void ATPSPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
}

//void ATPSPlayerController::OnTouchPressed(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	bIsTouch = true;
//	OnSetDestinationPressed();
//}

//void ATPSPlayerController::OnTouchReleased(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	bIsTouch = false;
//	OnSetDestinationReleased();
//}
