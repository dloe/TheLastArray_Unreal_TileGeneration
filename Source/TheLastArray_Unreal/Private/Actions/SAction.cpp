// Fill out your copyright notice in the Description page of Project Settings.
#include "Actions/SAction.h"
#include "Actions/SActionComponent.h"
#include <TheLastArray_Unreal/TheLastArray_Unreal.h>
#include <Runtime/Engine/Public/Net/UnrealNetwork.h>

void USAction::Initialize(USActionComponent* NewActionComp)
{
	ActionComp = NewActionComp;

}

void USAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Running: %s"), *GetNameSafe(this));
	//LogOnScreen(this, FString::Printf(TEXT("Started: %s"), *ActionName.ToString()), FColor::Green);

	USActionComponent* Comp = GetOwningComponent();

	//add them all
	Comp->ActiveGameplayTags.AppendTags(GrantsTags);

	RepData.bIsRunning = true;
	RepData.Instigator = Instigator;
	
	if (GetOwningComponent()->GetOwnerRole() == ROLE_Authority)
	{
		TimeStarted = GetWorld()->TimeSeconds;
	}
	

	GetOwningComponent()->OnActionStarted.Broadcast(GetOwningComponent(), this);
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Stopping: %s"), *GetNameSafe(this));
	//LogOnScreen(this, FString::Printf(TEXT("Stopped: %s"), *ActionName.ToString()), FColor::White);

	//check if boolean even on as santity
	//this will have issues when running on client and server so we will remove it 
	//ensureAlways(bIsRunning);

	USActionComponent* Comp = GetOwningComponent();

	//remove them all
	Comp->ActiveGameplayTags.RemoveTags(GrantsTags);

	RepData.bIsRunning = false;
	RepData.Instigator = Instigator;

	GetOwningComponent()->OnActionStopped.Broadcast(GetOwningComponent(), this);
}

bool USAction::CanStart_Implementation(AActor* Instigator)
{
	//to avoid running this when we have already started action
	if (IsRunning())
	{
		return false;
	}

	USActionComponent* Comp = GetOwningComponent();
	if (Comp->ActiveGameplayTags.HasAny(BlockedTags))
	{
		//if it contains any, return false
		return false;
	}
	else //we are allowed to do this action
		return true;

	
}

UWorld* USAction::GetWorld() const
{
	//this outer is set when creating action via newobject<>
	AActor* Actor = Cast<AActor>(GetOuter());
	if (Actor)
	{
		return Actor->GetWorld();
	}
	return nullptr;
}


USActionComponent* USAction::GetOwningComponent() const
{
	//not optimal way
	//AActor* Actor = Cast<AActor>(GetOuter());
	//we end up calling this alot and having to iteration though an entire list of components is inefficient,
	//lets go with other way (making a new function Initialize)
	//return Actor->GetComponentByClass(USActionComponent::StaticClass());


	//gotta fix this to work with replciation
	//can fix in 2 ways
	//return Cast<USActionComponent>(GetOuter());

	return ActionComp;
}


void USAction::OnRep_RepData()
{
	if (RepData.bIsRunning)
	{
		//TO DO: Fix Instigator
		StartAction(RepData.Instigator);
	}
	else {
		StopAction(RepData.Instigator);
	}

}

bool USAction::IsRunning() const
{
	return RepData.bIsRunning;
}


void USAction::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAction, RepData);
	DOREPLIFETIME(USAction, ActionComp);
	DOREPLIFETIME(USAction, TimeStarted);
}