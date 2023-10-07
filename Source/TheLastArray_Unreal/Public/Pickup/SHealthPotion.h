// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPickupBase.h"
#include "Components/StaticMeshComponent.h"
#include "SHealthPotion.generated.h"

UCLASS()
class THELASTARRAY_UNREAL_API ASHealthPotion : public ASPickupBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASHealthPotion();

protected:
	UPROPERTY(EditDefaultsOnly)
	float HealthIncrease;
	UPROPERTY(EditDefaultsOnly)
	float UseCost;


public:	

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	FText GetInteractText_Implementation(APawn* InstigatorPawn) override;
};
