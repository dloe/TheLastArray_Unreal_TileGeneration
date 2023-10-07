// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "UBTService_CheckIfHidden.generated.h"

/**
 * 
 */
UCLASS()
class THELASTARRAY_UNREAL_API UUBTService_CheckIfHidden : public UBTService
{
	GENERATED_BODY()
		
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector IsHiddenKey;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
