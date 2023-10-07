// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup/SPickupBase.h"
#include "Actions/SActionEffect.h"
#include "SThornsPickup.generated.h"

/**
 * 
 */
UCLASS()
class THELASTARRAY_UNREAL_API ASThornsPickup : public ASPickupBase
{
	GENERATED_BODY()
	
private:
	ASThornsPickup();

public:
	UPROPERTY(VisibleAnywhere)
	int ThornDuration;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<USActionEffect> ThornActionClass;

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
};
