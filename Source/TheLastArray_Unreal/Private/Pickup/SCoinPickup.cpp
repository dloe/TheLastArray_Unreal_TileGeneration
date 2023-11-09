// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup/SCoinPickup.h"
#include "GameFramework/Actor.h"
#include "Attributes/SAttributeComponent.h"
#include "Player/SPlayerState.h"

/// <summary>
/// The Last Array - Unreal
/// Coin Pickup Script
/// Dylan Loe
/// 
/// Last Updated: 10/8/23
/// 
/// Notes:
///  - 
///     
/// </summary>

ASCoinPickup::ASCoinPickup()
{
	PointValue = 10;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;
	// disable collision, instead we use sphere comp to handle interaction queries
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASCoinPickup::Interact_Implementation(APawn* InstigatorPawn)
{
	//UE_LOG(LogTemp, Log, TEXT("Sees coin"));
	//make sure our instiator
	if (!ensure(InstigatorPawn) && Triggered)// or InstigatorPawn && InstigatorPawn != GetInstigator())
	{
		return;
	}

	if(ASPlayerState* PS = Cast<ASPlayerState>(InstigatorPawn->GetPlayerState()))
	{
		PS->AddCredits(PointValue);
			//add points to player
		HideAndCooldownPowerup();

			//UE_LOG(LogTemp, Log, TEXT("value to add: %d"), PointValue);
			//debug print out our credit amount
		UE_LOG(LogTemp, Log, TEXT("Player Credits before addition: %d"), PS->GetCredits());
			//PS->Credits += PointValue;
			//int Credits = PState->AddCredits(PointValue);
		
	}
	
}
