// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup/SPickupBase.h"
#include "Attributes/SAttributeComponent.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include <TheLastArray_Unreal/TheLastArray_Unreal.h>
#include "SGameplayInterface.h"

/// <summary>
/// The Last Array - Unreal
/// Base Pickup Script
/// Dylan Loe
/// 
/// Last Updated: 10/8/23
/// 
/// Notes:
///  - 
///     
/// </summary>

// Sets default values
ASPickupBase::ASPickupBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionProfileName("Powerup");
	RootComponent = SphereComp;

	

	//HealthIncrease
	Triggered = false;
	TriggerDelay = 10;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASPickupBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASPickupBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//when player interacts with our health potion
void ASPickupBase::Interact_Implementation(APawn* InstigatorPawn)
{
	//LogOnScreen(this, FString::Printf(TEXT("Interact")), FColor::Green);
	//make sure our instiator
	if (InstigatorPawn && InstigatorPawn != GetInstigator())
	{
		USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass()));
		if (AttributeComp)
		{
			//base logic (could branch this off into a new function our children can override, then we dont have to do this few line setup
			HideAndCooldownPowerup();
			//Triggered = true;
		}
	}
}

FText ASPickupBase::GetInteractText_Implementation(APawn* InstigatorPawn)
{


	return FText::GetEmpty();
}

void ASPickupBase::OnTriggerTimer()
{
	UE_LOG(LogTemp, Log, TEXT("No longer triggered..."));
	//set bool and mesh
	Triggered = false;
	//OnRep_Triggered();
	//set mesh invisible and turn off interaction!
	//BaseMesh->SetStaticMesh(NormalPotionMesh);
	BaseMesh->SetVisibility(true, true);

	//turn on overlap events
	SetActorEnableCollision(true);

	GetWorldTimerManager().ClearTimer(TimerHandle);
}

void ASPickupBase::ShowPowerup()
{
	//LogOnScreen(this, FString::Printf(TEXT("In show powerup: %d"), Triggered), FColor::Green);
	//set mesh invisible and turn off interaction!
	//BaseMesh->SetStaticMesh(NormalPotionMesh);
	BaseMesh->SetVisibility(!Triggered, true);

	//turn on overlap events
	SetActorEnableCollision(!Triggered);

}

void ASPickupBase::HideAndCooldownPowerup()
{
	Triggered = true;
	//ShowPowerup();
	OnRep_Triggered();

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASPickupBase::OnTriggerTimer, TriggerDelay);
}

void ASPickupBase::OnRep_Triggered()
{
	LogOnScreen(this, FString::Printf(TEXT("Got Coin...")), FColor::Green);
	ShowPowerup();
}

void ASPickupBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//gotta set the rules
	DOREPLIFETIME(ASPickupBase, Triggered);


}