// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAIController.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "BehaviorTree/BlackboardComponent.h"

void ASAIController::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(BehaviorTree, TEXT("Behavior Tree is Null PTR - Please Assign BehaviorTree in your AI Controller...")))
	{
		RunBehaviorTree(BehaviorTree);
	}

	
	//APawn* MyPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	//if (MyPawn)
	//{
		//blackboard wrapper
		//need the include for this
	//	GetBlackboardComponent()->SetValueAsVector("MoveToLocation",MyPawn->GetActorLocation());

	//	GetBlackboardComponent()->SetValueAsObject("TargetActor", MyPawn);
	//}
}
