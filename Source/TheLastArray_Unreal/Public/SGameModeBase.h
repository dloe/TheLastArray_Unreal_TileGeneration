// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/PlayerState.h"
#include "Engine/DataTable.h"
#include "SGameModeBase.generated.h"

class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;
class UCurveFloat;
class USSaveGame;
class UDataTable;
class USMonsterData;


//datatable row for spawning monsters in gamemode
USTRUCT(BlueprintType)
struct FMonsterInfoRow : public FTableRowBase
{
	GENERATED_BODY()

public:

	FMonsterInfoRow()
	{
		Weight = 1.0f;
		SpawnCost = 5.0f;
		KillReward = 20.0f;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPrimaryAssetId MonsterId; //USMonsterData* MonsterData;

	//TSubclassOf<AActor> MonsterClass;

	//change to pick this monster
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight;

	//points required by gamemode to spawn this unit
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnCost;

	//amount of credits awarded to killer of this unit
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KillReward;
};


//DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnCreditChange, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewCredits, float, Delta);

UCLASS()
class THELASTARRAY_UNREAL_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:

	FString SlotName;

	UPROPERTY()
	USSaveGame* CurrentSaveGame;

	//all available monsters
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UDataTable* MonsterTable;

	//ai
	//UPROPERTY(EditDefaultsOnly, Category = "AI")
	//TSubclassOf<AActor> MinionClass;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UEnvQuery* SpawnBotQuery;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UCurveFloat* DifficultyCurve;

	FTimerHandle TimerHandle_SpawnBots;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SpawnTimerInterval;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float MaxBotCount;

	//env
	UPROPERTY(EditDefaultsOnly, Category = "Env")
	TArray<TSubclassOf<AActor>> PowerupClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Env")
	UEnvQuery* PowerupSpawnQuery;

	UPROPERTY(EditDefaultsOnly, Category = "Env")
	float MaxHealthPotionSpawnCount;

	UPROPERTY(EditDefaultsOnly, Category = "Env")
	float MaxCoinSpawnCount;

	UPROPERTY(EditDefaultsOnly, Category = "Env")
	float DesiredPowerupCount;
	UPROPERTY(EditDefaultsOnly, Category = "Env")
	float RequiredPowerupDistance;

	bool WorldSpawnSet;

	//UPROPERTY(EditDefaultsOnly, Category = "Player")
	//PlayerStateClass PlayerState;

	UFUNCTION()
	void SpawnBotTimerElapsed();

	UFUNCTION()
	void OnQueryPowerupSpawnCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	UFUNCTION()
	void OnQueryBotSpawnCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	void OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation);

	UFUNCTION()
	void RespawnPlayerElasped(AController* Controller);

	UFUNCTION()
	void SpawnPowerups();

public:
	ASGameModeBase();

	virtual void OnActorKilled(AActor* VictimActor, AActor* Killer);

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void StartPlay() override;

	//UFUNCTION(BlueprintNativeEvent, Category = Game)
	void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	UFUNCTION(exec)
	void KillAll();

	//when player kills minion
	UFUNCTION()
	void KillMinionEvent(AActor* InstigatorActor, int MinionCost);


	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void WriteSaveGame();

	void LoadSaveGame();
};
