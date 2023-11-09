// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/SThornsPickup.h"
#include "Actions/SActionComponent.h"

/// <summary>
/// The Last Array - Unreal
/// Effect Thorns Pickup Script
/// Dylan Loe
/// 
/// Last Updated: 10/8/23
/// 
/// Notes:
///  - 
///     
/// </summary>

ASThornsPickup::ASThornsPickup()
{
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;
	// disable collision, instead we use sphere comp to handle interaction queries
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ThornDuration = 15;
}

void ASThornsPickup::Interact_Implementation(APawn* InstigatorPawn)
{
	//UE_LOG(LogTemp, Log, TEXT("Sees coin"));
	//make sure our instiator
	if (!ensure(InstigatorPawn) && Triggered)// or InstigatorPawn && InstigatorPawn != GetInstigator())
	{
		return;
	}

	//set thorns indefinately
	USActionComponent* ActionComp = Cast<USActionComponent>(InstigatorPawn->GetComponentByClass(USActionComponent::StaticClass()));
	ActionComp->AddAction(this, ThornActionClass);
	Destroy();

}
