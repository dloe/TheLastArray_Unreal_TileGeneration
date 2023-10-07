// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "USBTService_CheckHealth.generated.h"

/**
 * 
 */
UCLASS()
class THELASTARRAY_UNREAL_API UUSBTService_CheckHealth : public UBTService
{
	GENERATED_BODY()

		//only returns true if we are under this fraction of health
	UPROPERTY(EditAnywhere, Category = "AI")
	float LowHealthFraction;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector MaxHealthKey;

	UUSBTService_CheckHealth();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	
};
