// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_RangedAttack.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include <TheLastArray_Unreal/Public/SAttributeComponent.h>



USBTTask_RangedAttack::USBTTask_RangedAttack()
{
	MaxBulletSpread = 2.0f;

}

EBTNodeResult::Type USBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* MyController = OwnerComp.GetAIOwner();

	if (ensure(MyController))
	{
		ACharacter* MyPawn = Cast<ACharacter>(MyController->GetPawn());
		if (MyPawn == nullptr)
		{
			return EBTNodeResult::Failed;
		}

		FVector MuzzleLocation = MyPawn->GetMesh()->GetSocketLocation("Muzzle_01");

		AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));

		//if we cant see our player just null out for now
		if (TargetActor == nullptr)
		{
			return EBTNodeResult::Failed;
		}

		//is this still a valid target? still alive?
		if (!USAttributeComponent::IsActorAlive(TargetActor))
		{
			return EBTNodeResult::Failed;
		}

		//we will then need to get a direciton if we have a target (aka our player)
		FVector Direction = TargetActor->GetActorLocation() - MuzzleLocation;
		//turn it into a rotation
		//we should have a little bit or random offset (aka weapon spread), less accurate
		FRotator MuzzleRotation = Direction.Rotation();

		MuzzleRotation.Pitch += FMath::RandRange(0.0f, MaxBulletSpread);
		MuzzleRotation.Yaw += FMath::RandRange(0.0f, MaxBulletSpread);

		FActorSpawnParameters Params;
		//this might be causing a warning, make sure its always
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Params.Instigator = MyPawn;


		AActor* NewProj = GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, MuzzleRotation, Params);

		return NewProj ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;

	}

	return EBTNodeResult::Failed;
}

