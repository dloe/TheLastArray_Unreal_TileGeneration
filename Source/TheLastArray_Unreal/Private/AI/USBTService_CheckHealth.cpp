// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/USBTService_CheckHealth.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include <TheLastArray_Unreal/Public/Attributes/SAttributeComponent.h>



UUSBTService_CheckHealth::UUSBTService_CheckHealth()
{
	LowHealthFraction = 0.35f;
}

void UUSBTService_CheckHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();
	//check if we are at max health, and update the maxhealth bool in BT
	if (ensure(AIPawn))
	{
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(AIPawn);
		if (ensure(AttributeComp))
		{
			bool belowHealth = (AttributeComp->GetHealthMax() / AttributeComp->GetHealthMax()) < LowHealthFraction;
			UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
			BlackBoardComp->SetValueAsBool(MaxHealthKey.SelectedKeyName, belowHealth);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("No Attrubute Comp Present!"));
		}
	}
}
