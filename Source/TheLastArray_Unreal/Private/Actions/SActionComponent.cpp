// Fill out your copyright notice in the Description page of Project Settings.
#include "Actions/SActionComponent.h"
#include "Actions/SAction.h"
#include <TheLastArray_Unreal/TheLastArray_Unreal.h>
#include "Engine/ActorChannel.h"
#include <Runtime/Engine/Public/Net/UnrealNetwork.h>

DECLARE_CYCLE_STAT(TEXT("StartActionByName"), STAT_StartActionByName, STATGROUP_STANFORD)

/// <summary>
/// The Last Array - Unreal
/// Action Component Script
/// Dylan Loe
/// 
/// Last Updated: 10/8/23
/// 
/// Notes:
///  - Deals with actions and action management
///     
/// </summary>

// Sets default values for this component's properties
USActionComponent::USActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

// Called when the game starts
void USActionComponent::BeginPlay()
{
	Super::BeginPlay();


	//make sure this only runs on server (server only)
	if (GetOwner()->HasAuthority())
	{
		for (TSubclassOf<USAction> ActionClass : DefaultActions)
		{
			AddAction(GetOwner(), ActionClass);
		}
	}


}

void USActionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//make sure nothing is still running
	TArray<USAction*> ActionsCopy = Actions;
	for (USAction* Action : ActionsCopy)
	{
		if (Action && Action->IsRunning())
		{
			Action->StopAction(GetOwner());
		}
	}


	Super::EndPlay(EndPlayReason);
}

// Called every frame
void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//debug for helping illustrate tags
	//to string simple has less markup and more efficient
	//FString DebugMessage = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, DebugMessage);

	//draw all actions
	//for (USAction* Action : Actions)
	//{
	//	FColor TextColor = Action->IsRunning() ? FColor::Blue : FColor::White;
	//	FString ActionMsg = FString::Printf(TEXT("[%s] Action: %s"), *GetNameSafe(GetOwner()), *GetNameSafe(Action));
	//	LogOnScreen(this, ActionMsg, TextColor, 0.0f);
	//}
}



void USActionComponent::AddAction(AActor* Instigator, TSubclassOf<USAction> ActionClass)
{
	if (!ensure(ActionClass))
	{
		return;
	}

	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Client attempting to AddAction [Class: %s]"), *GetNameSafe(ActionClass));
		return;
	}

	//instantiate an object
	//outer, whoever owns this object in a way, pass in this because the action component will own this
	//next is action class
	USAction* NewAction = NewObject<USAction>(GetOwner(), ActionClass);
	//add the action to our array
	if (ensure(NewAction))
	{
		NewAction->Initialize(this);
		Actions.Add(NewAction);

		if (NewAction->bAutoStart && ensure(NewAction->CanStart(Instigator)))
			NewAction->StartAction(Instigator);
	}
}

bool USActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	SCOPE_CYCLE_COUNTER(STAT_StartActionByName);


	for (USAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			if (!Action->CanStart(Instigator))
			{
				FString FailedMsg = FString::Printf(TEXT("Failed to run: %s"), *ActionName.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FailedMsg);

				//could come back to this if another action needs to be checked and run instead
				continue;
			}

			//blackholes cost rage

			//to avoid an infi loop
			if (!GetOwner()->HasAuthority()) {
				ServerStartAction(Instigator, ActionName);
			}
			

			TRACE_BOOKMARK(TEXT("StartAction::%s"), *GetNameSafe(Action));

			Action->StartAction(Instigator);
			return true;
		}
	}
	return false;
}

bool USActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	for (USAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			if (Action->IsRunning()) {

				if (!GetOwner()->HasAuthority()) {
					ServerStopAction(Instigator, ActionName);
				}

				Action->StopAction(Instigator);
				return true;
			}
		}
	}
	return false;
}

void USActionComponent::RemoveAction(USAction* ActionToRemove)
{
	if (!ensure(ActionToRemove && !ActionToRemove->IsRunning()))
	{
		return;
	}
	Actions.Remove(ActionToRemove);
}

bool USActionComponent::CheckAction(TSubclassOf<USAction> ActionClass)
{
	return DefaultActions.Contains(ActionClass);
	//or
	//for (USAction* Action : Actions)
	//{
		//if (Action && Action->IsA(ActionClass))
		//{
			//return ActionClass;
		//}
	//}
	//return nullptr;
}

bool USActionComponent::CheckActionName(FName ActionName)
{
	for (USAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			return true;
		}
	}

	return false;
}

void USActionComponent::ServerStartAction_Implementation(AActor* Instigator, FName ActionName)
{
	StartActionByName(Instigator, ActionName);

}

void USActionComponent::ServerStopAction_Implementation(AActor* Instigator, FName ActionName)
{
	StopActionByName(Instigator, ActionName);

}

//we shouldnt need to add the header file since we marked a var as replciated. Its only generated h should include it
void  USActionComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USActionComponent, Actions);
}


bool USActionComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	for (USAction* Action : Actions)
	{
		if (Action)
		{
			//using bitwise to write to WroteSomthing
			//make sure that it keeps track of wheather or not it wrote somehting
			WroteSomething |= Channel->ReplicateSubobject(Action, *Bunch, *RepFlags);

		}
	}

	return WroteSomething;
}