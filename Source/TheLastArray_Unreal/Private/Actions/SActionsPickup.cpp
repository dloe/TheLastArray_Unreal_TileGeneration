// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionsPickup.h"
#include "SActionComponent.h"


ASActionsPickup::ASActionsPickup()
{
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;
	// disable collision, instead we use sphere comp to handle interaction queries
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASActionsPickup::Interact_Implementation(APawn* InstigatorPawn)
{
	//only if we dont have that specific action class

	//UE_LOG(LogTemp, Log, TEXT("Sees coin"));
	//make sure our instiator
	if (!ensure(InstigatorPawn))// or InstigatorPawn && InstigatorPawn != GetInstigator())
	{
		return;
	}

	//set thorns indefinately
	USActionComponent* ActionComp = Cast<USActionComponent>(InstigatorPawn->GetComponentByClass(USActionComponent::StaticClass()));
	if (!ActionComp->CheckAction(ActionClass)) {
		ActionComp->AddAction(this, ActionClass);
		Destroy();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Could not find action!"));
	}
}
