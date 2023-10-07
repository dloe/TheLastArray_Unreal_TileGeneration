// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/UBTTask_HealAI.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include <TheLastArray_Unreal/Public/SAttributeComponent.h>

EBTNodeResult::Type UUBTTask_HealAI::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyAIController = OwnerComp.GetAIOwner();
	APawn* AIPawn = MyAIController->GetPawn();
	//check if we are at max health, and update the maxhealth bool in BT
	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (ensure(BlackBoardComp))
	{
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(AIPawn);
		if (ensure(AttributeComp))
		{
			if (AttributeComp->ApplyHealthChange(MyAIController, AttributeComp->GetHealthMax()))
			{
				UE_LOG(LogTemp, Log, TEXT("Enemy Healed!"));
				BlackBoardComp->SetValueAsBool(MaxHealthKey.SelectedKeyName, AttributeComp->IsUnderMaxHealth());
				return EBTNodeResult::Succeeded;
			}
		}
	}

	return EBTNodeResult::Failed;
}