// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/SAnimInstance.h"
#include <Runtime/GameplayTags/Classes/GameplayTagContainer.h>
#include "Actions/SActionComponent.h"

/// <summary>
/// The Last Array - Unreal
/// Animimation Instance Script
/// Dylan Loe
/// 
/// Last Updated: 10/8/23
/// 
/// Notes:
///  - 
///     
/// </summary>

void USAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	AActor* OwningActor = GetOwningActor();
	if (OwningActor)
	{
		ActionComp = Cast<USActionComponent>(OwningActor->GetComponentByClass(USActionComponent::StaticClass()));
	}

}

void USAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	static FGameplayTag StunnedTag = FGameplayTag::RequestGameplayTag("Status.Stunned");
	if (ActionComp)
	{
		bIsStunned = ActionComp->ActiveGameplayTags.HasTag(StunnedTag);
	}
}
