// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "Kismet/GameplayStatics.h"
#include <TheLastArray_Unreal/Public/AI/SAICharacter.h>
#include <TheLastArray_Unreal/Public/Attributes/SAttributeComponent.h>
#include "EngineUtils.h"
#include "SSaveGame.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include <TheLastArray_Unreal/Public/Player/SCharacter.h>
#include <TheLastArray_Unreal/Public/SGameplayInterface.h>
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>
#include "ScriptableObjects/SMonsterData.h"
#include <TheLastArray_Unreal/TheLastArray_Unreal.h>
#include <Runtime/Engine/Classes/Engine/AssetManager.h>
//dont think the eenvqueryrunmode enum is needed to include header

//cvar
static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable spawning of bots via timer."), ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;
	MaxBotCount = 1.0f; //10.0f;
	WorldSpawnSet = false;
	//assign PlayerStateClass
	PlayerStateClass = ASPlayerState::StaticClass();
	SlotName = "SaveGame01";
}

void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	FString SelectedSaveSlot = UGameplayStatics::ParseOption(Options, "SaveGame");

	if (SelectedSaveSlot.Len() > 0)
	{
		SlotName = SelectedSaveSlot;

	}

	LoadSaveGame();
	
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	if(EnemysCanSpawn) {
		SpawnPowerups();
		GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);
	}

}

void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	
	//calling before super so we set the BeginPlayingState Asap
	ASPlayerState* PS = NewPlayer->GetPlayerState<ASPlayerState>();
	if (PS)
	{
		PS->LoadPlayerState(CurrentSaveGame);
	}

	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}

void ASGameModeBase::SpawnBotTimerElapsed()
{
	//console var
	if (!CVarSpawnBots.GetValueOnGameThread())
	{
		UE_LOG(LogTemp, Warning, TEXT("Bot Spawning disabled via cvar 'CVarSpawnBots'."));
		return;
	}

	int32 NrOfAliveBots = 0;
	//
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;

		//will eventually as is alive function
		//if (Bot->Alive())
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);
		//we added this ensure while debugging to show that when we immedtately play we see this hit becasue the attributecomp is still null
		// never actually added teh attrubute comp to our AI yet
		// never would increase our count
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			NrOfAliveBots++;
		}
	}

	//UE_LOG(LogTemp, Log, TEXT("Found %i alive bots."), NrOfAliveBots);

	//float MaxBotCount = 1.0f; //10.0f;
	if (DifficultyCurve)
	{
		MaxBotCount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}

	if (NrOfAliveBots >= MaxBotCount)
	{
		UE_LOG(LogTemp, Log, TEXT("At max bot count, skipping bot spawn"));
		return;
	}


	//eqs query for spawning bots
	//using old style of enums (dont really use this much anymore)
	//random best 5%
	//never really use the last param
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);

	QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnQueryBotSpawnCompleted);

}

void ASGameModeBase::OnQueryPowerupSpawnCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	//not using ensure because it would be triggering every 2 frames and having this check is instantatious (more useful this way)
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn Powerups EQS Query Failed!"));
		return;
	}

	//give us the result and location for where we want to spawn
	//status tells us if it fails, etc
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	//keep used locations to easily check distance btween points
	TArray<FVector> UsedLocations;

	int32 SpawnCounter = 0;

	//break out if we reached the disired count or if we have no more potential positons remaining
	while (SpawnCounter < DesiredPowerupCount && Locations.Num() > 0)
	{
		int32 RandomLocationIndex = FMath::RandRange(0, Locations.Num() - 1);

		FVector PickedLocation = Locations[RandomLocationIndex];

		//check min distance requirement
		bool bValidLocation = true;

		for (FVector OtherLocation : UsedLocations)
		{
			float DistanceTo = (PickedLocation - OtherLocation).Size();

			if (DistanceTo < RequiredPowerupDistance)
			{
				//show skipped locations due to distance
				//DrawDebugSphere(GetWorld(), PickedLocation, 50.0f, 20, FColor::Red, false, 10.0f);

				//too close, skip to next attempt
				bValidLocation = false;
				break;
			}
		}


		//Failed the distance test
		if (!bValidLocation)
		{
			//go back to start of while loop and try again (assuming we got that false in correct location that is to close to each other
			//helpful bit of logic to have in my backpocket for future projects
			continue;
		}


		//Pick a random powerup class
		int32 RandomClassIndex = FMath::RandRange(0, PowerupClasses.Num() - 1);
		TSubclassOf<AActor> RandomPowerupClass = PowerupClasses[RandomClassIndex];

		//GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);

		//keep for distance checks
		UsedLocations.Add(PickedLocation);
		SpawnCounter++;
	}

}

void ASGameModeBase::OnQueryBotSpawnCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	//not using ensure because it would be triggering every 2 frames and having this check is instantatious (more useful this way)
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn Bot EQS Query Failed!"));
		return;
	}

	//give us the result and location for where we want to spawn
	//status tells us if it fails, etc
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	
	//check our locations
	if (Locations.IsValidIndex(0))
	{
		if (MonsterTable)
		{
			TArray<FMonsterInfoRow*> Rows;
			MonsterTable->GetAllRows("", Rows);

			//get random enemy
			int32 RandomIndex = FMath::RandRange(0, Rows.Num() - 1);
			FMonsterInfoRow* SelectedRow = Rows[RandomIndex];

			//SelectedRow->SpawnCost

			UAssetManager* Manager = UAssetManager::GetIfValid();
			if (Manager)
			{
				LogOnScreen(this, "Loading Monster...", FColor::Green);
				//do them all since we didnt specific
				TArray<FName> Bundles;

				//calls OnMonsterLoad when loaded, pass along AssetData and FVector to this OnMonsterLoaded function
				FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ASGameModeBase::OnMonsterLoaded, SelectedRow->MonsterId, Locations[0]);


				Manager->LoadPrimaryAsset(SelectedRow->MonsterId, Bundles, Delegate);
			}
		}

		//GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);

		//track all used spawn locations
		DrawDebugSphere(GetWorld(), Locations[0], 50.0f, 20, FColor::Blue, false, 60.0f);
	}
}

void ASGameModeBase::OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation)
{
	LogOnScreen(this, "Finished Loading Monster...", FColor::Green);

	UAssetManager* Manager = UAssetManager::GetIfValid();
	if (Manager)
	{
		USMonsterData* MonsterData = Cast<USMonsterData>(Manager->GetPrimaryAssetObject(LoadedId));
		if (MonsterData)
		{
			AActor* NewBot = GetWorld()->SpawnActor<AActor>(MonsterData->MonsterClass, SpawnLocation, FRotator::ZeroRotator);
			if (NewBot)
			{
				LogOnScreen(this, FString::Printf(TEXT("Spawned Enemy: %s (%s)"), *GetNameSafe(NewBot), *GetNameSafe(MonsterData)));

				USActionComponent* ActionComp = Cast<USActionComponent>(NewBot->GetComponentByClass(USActionComponent::StaticClass()));
				if (ActionComp)
				{
					for (TSubclassOf<USAction>ActionClass : MonsterData->Actions)
					{
						ActionComp->AddAction(NewBot, ActionClass);
					}
				}
			}
		}
	}
}

void ASGameModeBase::SpawnPowerups()
{
	if (ensure(PowerupClasses.Num() > 0))
	{
		UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, PowerupSpawnQuery, this, EEnvQueryRunMode::AllMatching, nullptr);
		if (ensure(QueryInstance))
		{
			QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnQueryPowerupSpawnCompleted);
		}
	}
}

//cheat
void ASGameModeBase::KillAll()
{
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			AttributeComp->Kill(this); //@fixme pass in player? for kill credit
		}
	}
}

//question to ponder, would the score for each minion be based in gamemodebase or in the minion
//perhaps there could be a multiplier or other details that would make this function have a better place here
//in the gamemodebase script
void ASGameModeBase::KillMinionEvent(AActor* InstigatorActor, int MinionCost)
{
	APlayerController* PC = Cast<APlayerController>(InstigatorActor->GetInstigatorController());
	ASPlayerState* PS = Cast<ASPlayerState>(PC->PlayerState);
	PS->AddCredits(MinionCost);

}

void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* Killer)
{
	ASCharacter* Player = Cast<ASCharacter>(VictimActor);
	if (Player)
	{
		FTimerHandle TimerHandle_RespawnDelay;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "RespawnPlayerElasped", Player->GetController());


		float RespawnDelay = 2.0f;
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespawnDelay, false);
	}
	UE_LOG(LogTemp, Log, TEXT("OnActorKilled: Victim %s, Killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(Killer));


	//Give credits for kill
	//may need to dive into this more as things get more complicated
	APawn* KillerPawn = Cast<APawn>(Killer);
	if (KillerPawn)
	{
		ASPlayerState* PS = KillerPawn->GetPlayerState<ASPlayerState>();
		ASAICharacter* CH = Cast<ASAICharacter>(VictimActor);
		if (PS && CH)
		{
			PS->AddCredits(CH->CreditsOnKill);
		}
	}
}



void ASGameModeBase::RespawnPlayerElasped(AController* Controller)
{
	if (ensure(Controller))
	{
		Controller->UnPossess();

		RestartPlayer(Controller);
	}

}


void ASGameModeBase::WriteSaveGame()
{
	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		ASPlayerState* PS = Cast<ASPlayerState>(GameState->PlayerArray[i]);
		if (PS)
		{
			PS->SavePlayerState(CurrentSaveGame);
			break; // single player only at this point
		}
	}


	CurrentSaveGame->SavedActors.Empty();

	//iterate the entire world of actors
	for (FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		// only interested in our 'gameplay actors'
		if (!Actor->Implements<USGameplayInterface>())
		{
			continue;
		}

		FActorSaveData ActorData;
		ActorData.ActorName = Actor->GetName();
		ActorData.Transform = Actor->GetActorTransform();

		//pass array to fill with data from actor
		FMemoryWriter MemWriter(ActorData.ByteData);

		FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
		//marks it as only looking for those saving props UPROPERTY(SaveGame)
		Ar.ArIsSaveGame = true;
		//converts actors SaveGame UPROPERTIES into binrary array
		Actor->Serialize(Ar);


		CurrentSaveGame->SavedActors.Add(ActorData);
	}

	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, 0);
}

void ASGameModeBase::LoadSaveGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		if (CurrentSaveGame == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load SaveGame data"));
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("Loaded SaveGame data"));

		for (FActorIterator It(GetWorld()); It; ++It)
		{
			AActor* Actor = *It;
			// only interested in our 'gameplay actors'
			if (!Actor->Implements<USGameplayInterface>())
			{
				continue;
			}

			for (FActorSaveData ActorData : CurrentSaveGame->SavedActors)
			{
				if (ActorData.ActorName == Actor->GetName())
				{
					Actor->SetActorTransform(ActorData.Transform);

					FMemoryReader MemReader(ActorData.ByteData);

					FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
					//marks it as only looking for those saving props UPROPERTY(SaveGame)
					Ar.ArIsSaveGame = true;
					//converts binary array abck into actors SaveGame UPROPERTIES 
					Actor->Serialize(Ar);

					ISGameplayInterface::Execute_OnActorLoaded(Actor);

					break;
				}
			}

		}
	}
	else
	{
		CurrentSaveGame = Cast < USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));

		UE_LOG(LogTemp, Log, TEXT("Created new SaveGame data"));
	}



}
