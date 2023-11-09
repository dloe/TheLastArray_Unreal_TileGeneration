// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckAttackRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

/// <summary>
/// The Last Array - Unreal
/// BT Service Check Attack Range Script
/// Dylan Loe
/// 
/// Last Updated: 10/8/23
/// 
/// Notes:
///  - 
///     
/// </summary>

void USBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	//check distance between ai pawn and target actor

	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (ensure(BlackBoardComp))
	{
		//will redo this to not be hardcoded
		AActor* TargetActor = Cast <AActor>(BlackBoardComp->GetValueAsObject("TargetActor"));
		if (TargetActor)
		{
			AAIController* MyController = OwnerComp.GetAIOwner();
			if (ensure(MyController))
			{
				APawn* AIPawn = MyController->GetPawn();

				if (ensure(AIPawn))
				{
					float DistanceTo = FVector::Distance(TargetActor->GetActorLocation(), AIPawn->GetActorLocation());

					bool bWithinRange = DistanceTo < 2000.0f;

					bool bHasLOS = false;
					if (bWithinRange) {
						//does a line trace by ECC_Visibility
						bHasLOS = MyController->LineOfSightTo(TargetActor);
					}

					BlackBoardComp->SetValueAsBool(AttackRangeKey.SelectedKeyName,bHasLOS);
				}

			}
		}
	}
}
