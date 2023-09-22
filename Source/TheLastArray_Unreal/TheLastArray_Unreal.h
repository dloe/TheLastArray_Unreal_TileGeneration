// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


DECLARE_STATS_GROUP(TEXT("STANFORD_Game"), STATGROUP_STANFORD, STATCAT_Advanced)
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

//for reshuffling arrays
//TO DO: For some reason this function was having issue with the GameStream keeping things consistently random. Random values weren't random and 
//kept picking same index to swap, moving it into TileManager fixed this.
static TArray <int> Reshuffle3(TArray <int> ar, FRandomStream GameStream)
{
	// Knuth shuffle algorithm :: courtesy of Wikipedia :)
	for (int t = 0; t < ar.Num(); t++)
	{
		//int tmp = ar[t];
		int r = GameStream.RandRange(t, ar.Num() - 1);
		UE_LOG(LogTemp, Log, TEXT("Swapping: %d and %d"), t, r);
		ar.Swap(t, r);
	}
	return ar;
}