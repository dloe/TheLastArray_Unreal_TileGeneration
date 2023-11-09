// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/UBTService_CheckIfHidden.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

/// <summary>
/// The Last Array - Unreal
/// BT Service CheckIfHidden Script
/// Dylan Loe
/// 
/// Last Updated: 10/8/23
/// 
/// Notes:
///  - 
///     
/// </summary>

void UUBTService_CheckIfHidden::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	//check if we are at max health, and update the maxhealth bool in BT
	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (ensure(BlackBoardComp))
	{
		AActor* TargetActor = Cast <AActor>(BlackBoardComp->GetValueAsObject("TargetActor"));
		AAIController* MyAIController = OwnerComp.GetAIOwner();
		if (ensure(MyAIController) && TargetActor)
		{
			bool bHasLOS = MyAIController->LineOfSightTo(TargetActor);
			BlackBoardComp->SetValueAsBool(IsHiddenKey.SelectedKeyName, bHasLOS);
		}
	}
}
