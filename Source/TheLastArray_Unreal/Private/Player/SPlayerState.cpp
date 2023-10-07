// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
#include "Net/UnrealNetwork.h"
#include <TheLastArray_Unreal/Public/SSaveGame.h>

ASPlayerState::ASPlayerState()
{
	Credits = 0;
	SetReplicates(true);
}

int ASPlayerState::GetCredits()
{
	return Credits;
}

bool ASPlayerState::RemoveCredits(int32 delta)
{
	if (!ensure(delta > 0.0f))
		return false;
	
	//UE_LOG(LogTemp, Log, TEXT("in funct - current total: %d"), Credits);
	//UE_LOG(LogTemp, Log, TEXT("in funct - value to remove: %d"), value);
	if (delta <= Credits)
	{
		//int temp = Credits - value;
		//Credits = temp;
		Credits -= delta;
		OnCreditsChanged.Broadcast(this, Credits, -delta);
		//UE_LOG(LogTemp, Log, TEXT("in funct - final val: %d"), Credits);
		return true;
	}
	return false;
}

int ASPlayerState::AddCredits(int32 delta)
{

	if (!ensure(delta > 0.0f))
		return 0;

	//UE_LOG(LogTemp, Log, TEXT("in funct - value to add: %d"), value);
	Credits += delta;
	//UE_LOG(LogTemp, Log, TEXT("in funct - new credit total: %d"), Credits);
	//we might have a cap on how many you can get so for just return true by default
	OnCreditsChanged.Broadcast(this, Credits, delta);
	return Credits;
}

//can use a param to get original val for easily finding delta
void ASPlayerState::OnRep_Credits(int32 OldCredits)
{
	OnCreditsChanged.Broadcast(this, Credits, Credits - OldCredits);

}

void ASPlayerState::LoadPlayerState_Implementation(USSaveGame* SaveObject)
{
	if (SaveObject)
	{
		//SaveObject->Credits = Credits;
		AddCredits(SaveObject->Credits);
	}

}

void ASPlayerState::SavePlayerState_Implementation(USSaveGame* SaveObject)
{
	if (SaveObject)
	{
		Credits = SaveObject->Credits;
	}

}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, Credits);
	//DOREPLIFETIME(USAttributeComponent, HealthMax);
	//DOREPLIFETIME_CONDITION(USAttributeComponent, HealthMax, COND_InitialOnly);
}