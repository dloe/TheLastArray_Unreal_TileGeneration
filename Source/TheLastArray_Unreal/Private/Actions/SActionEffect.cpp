// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SActionEffect.h"
#include "GameFramework/GameStateBase.h"
#include "Actions/SActionComponent.h"

/// <summary>
/// The Last Array - Unreal
/// Action Effect Script
/// - Inherits from SAction
/// Dylan Loe
/// 
/// Last Updated: 10/8/23
/// 
/// Notes:
///  - effects management attatched to actions
///     
/// </summary>

USActionEffect::USActionEffect()
{
	bAutoStart = true;
}

void USActionEffect::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	if (Duration > 0.0f)
	{
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "StopAction", Instigator);

		GetWorld()->GetTimerManager().SetTimer(DurationHandle, Delegate, Duration, false);
	}

	if (Period > 0.0f)
	{
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "ExecutePeriodicEffect", Instigator);

		GetWorld()->GetTimerManager().SetTimer(PeriodHandle, Delegate, Period, true);
	}
}

float USActionEffect::GetTimeRemaining()
{
	AGameStateBase* GS = GetWorld()->GetGameState<AGameStateBase>();
	
	if (GS) {
		float EndTime = TimeStarted + Duration;
		return EndTime - GS->GetServerWorldTimeSeconds();;
	}

	return Duration;
}

void USActionEffect::StopAction_Implementation(AActor* Instigator)
{
	//if period is more than duration
	if (GetWorld()->GetTimerManager().GetTimerRemaining(PeriodHandle) < KINDA_SMALL_NUMBER)
	{
		ExecutePeriodicEffect(Instigator);
	}

	Super::StopAction_Implementation(Instigator);

	GetWorld()->GetTimerManager().ClearTimer(PeriodHandle);
	GetWorld()->GetTimerManager().ClearTimer(DurationHandle);


	USActionComponent* Comp = GetOwningComponent();
	if (Comp)
	{
		Comp->RemoveAction(this);
	}
	
}

void USActionEffect::ExecutePeriodicEffect_Implementation(AActor* Instigator)
{


}
