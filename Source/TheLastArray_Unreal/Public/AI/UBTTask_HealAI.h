// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "UBTTask_HealAI.generated.h"

/**
 * 
 */
UCLASS()
class THELASTARRAY_UNREAL_API UUBTTask_HealAI : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector MaxHealthKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
