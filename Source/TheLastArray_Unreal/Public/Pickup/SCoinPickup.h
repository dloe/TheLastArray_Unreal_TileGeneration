// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPickupBase.h"
#include "SCoinPickup.generated.h"

/**
 * 
 */
UCLASS()
class THELASTARRAY_UNREAL_API ASCoinPickup : public ASPickupBase
{
	GENERATED_BODY()
	

private:
	ASCoinPickup();

	

public:
	UPROPERTY(VisibleAnywhere)//editdefaultsonly?
	int PointValue;

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

};
