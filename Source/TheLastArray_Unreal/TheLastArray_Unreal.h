// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_STATS_GROUP(TEXT("STANFORD_Game"), STATGROUP_STANFORD, STATCAT_Advanced)

//static function
//for printing and differentiating between client and server
//
static void LogOnScreen(UObject* WorldContext, FString Msg, FColor Color = FColor::White, float Duration = 5.0f)
{
	if (!ensure(WorldContext))
	{
		return;
	}

	UWorld* World = WorldContext->GetWorld();
	if (!ensure(World))
	{
		return;
	}

	//NM_Client is client
	//NM_DedicatedServer
	//NM_ListenServer when player is also a host
	// 
	FString NetPrefix = World->IsNetMode(NM_Client) ? "[CLIENT] " : "[SERVER] ";
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, NetPrefix + Msg);
		//UE_LOG(LogTemp, Log, TEXT("%s %s"), *FString(NetPrefix), *FString(Msg));

	}

}
