// Fill out your copyright notice in the Description page of Project Settings.

#include "Pickup/SHealthPotion.h"
#include "GameFramework/Actor.h"
#include "Attributes/SAttributeComponent.h"
#include "Player/SPlayerState.h"

#define LOCTEXT_NAMESPACE "InteractableActors"

/// <summary>
/// The Last Array - Unreal
/// Health Potion Pickup Script
/// Dylan Loe
/// 
/// Last Updated: 10/8/23
/// 
/// Notes:
///  - 
///     
/// </summary>

// Sets default values
ASHealthPotion::ASHealthPotion()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthIncrease = 10;
	UseCost = 20;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;
	// disable collision, instead we use sphere comp to handle interaction queries
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASHealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	UE_LOG(LogTemp, Log, TEXT("Sees health potion"));
	//make sure our instiator
	if (!ensure(InstigatorPawn))// or InstigatorPawn && InstigatorPawn != GetInstigator())
	{
		return;
	}
		USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass()));

		if (ASPlayerState* PS = Cast<ASPlayerState>(InstigatorPawn->GetPlayerState())) {
			//UE_LOG(LogTemp, Log, TEXT("Player Credits before addition: %d"), PS->GetCredits());
			//if (PS->GetCredits() >= UseCost) {

				if (PS->RemoveCredits(UseCost) && AttributeComp->IsUnderMaxHealth() && !Triggered && AttributeComp->ApplyHealthChange(this, HealthIncrease))
				{
					//if players health is below max AND hasnt already been triggered within 10 sec

					//UE_LOG(LogTemp, Log, TEXT("Health Potion: Triggered!"));
					//start timer for 10 seconds, trigger = true (once timer ends, set bool and mesh)
					HideAndCooldownPowerup();
					//UE_LOG(LogTemp, Log, TEXT("Player Credits after potion: %d"), PS->GetCredits());
				}
				else {
					UE_LOG(LogTemp, Log, TEXT("Health Potion: CANT TRIGGER!"));
				}
			//}
			//else { UE_LOG(LogTemp, Log, TEXT("Health Potion: to expensive!")); }

		}
}

FText ASHealthPotion::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass()));
	if (AttributeComp && !AttributeComp->IsUnderMaxHealth())
	{
		return LOCTEXT("HealthPotion_FullHealthWarning", "Already at Full Health.");


	}

	return FText::Format(LOCTEXT("HealthPotion_InteractMessage", "Cost {0} Credits. Restore health to maximum. "), UseCost);
}

#undef LOCTEXT_NAMESPACE