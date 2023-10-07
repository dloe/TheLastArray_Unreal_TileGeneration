// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPickupBase.h"
#include "SActionEffect.h"
#include "SActionsPickup.generated.h"

/**
 * 
 */
UCLASS()
class THELASTARRAY_UNREAL_API ASActionsPickup : public ASPickupBase
{
	GENERATED_BODY()

	ASActionsPickup();
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Action")
	TSubclassOf<USAction> ActionClass;

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
};
