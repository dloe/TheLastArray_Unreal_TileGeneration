// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SPlayerController.h"
#include "Blueprint/UserWidget.h"

/// <summary>
/// The Last Array - Unreal
/// Player Controller Script
/// Dylan Loe
/// 
/// Last Updated: 10/8/23
/// 
/// Notes:
///  - 
///     
/// </summary>

void ASPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	OnPawnChanged.Broadcast(InPawn);

}

void ASPlayerController::BeginPlayingState()
{
	//Super::BeginPlayingState();

	BlueprintBeginPlayingState();
}

void ASPlayerController::TogglePauseMenu()
{
	if (PauseMenuInstance && PauseMenuInstance->IsInViewport())
	{
		PauseMenuInstance->RemoveFromParent();
		PauseMenuInstance = nullptr;

		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());
		return;
	}

	PauseMenuInstance = CreateWidget<UUserWidget>(this, PauseMenuClass);
	if (PauseMenuInstance)
	{
		PauseMenuInstance->AddToViewport(100);

		bShowMouseCursor = true;
		SetInputMode(FInputModeUIOnly());


	}
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("PauseMenu", IE_Pressed, this, &ASPlayerController::TogglePauseMenu);

}
