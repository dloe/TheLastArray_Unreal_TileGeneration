// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STile.h"
#include "LocalLevel.h"
#include "STileManager.generated.h"


USTRUCT()
struct FMultiTileStruct
{
	GENERATED_BODY()

public:
	//each element of our array will contain this array of AActor*
	UPROPERTY()
	//TArray<TSubclassOf<ASTile>> TileColumn;
	TArray<ASTile*> TileColumn;

	//Constructor 
	FMultiTileStruct()
	{
		//UE_LOG(LogTemp, Log, TEXT("In FMultiTileStruct Constructor..."));
		TileColumn.Empty();
	}

};

USTRUCT()
struct FTileInfoStruct
{
	GENERATED_BODY()

public:

	UPROPERTY()
	ASTile* tile;
	UPROPERTY()
	TArray<int> n;


};

UCLASS()
class THELASTARRAY_UNREAL_API ASTileManager : public AActor
{
	/// <summary>
	/// Tile Manager Script
	/// Dylan Loe
	/// 
	/// Notes:
	/// - Main Manager for Map creation and Tiles
	/// - Holds all linked tiles in 2d array
	/// - Runs the initial random tile generation implementation
	/// - Receives info for map size and criteria from scriptable objects
	/// 
	/// 
	/// 2D arrays are difficult but we can try the struct approach
	/// 
	/// </summary>
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASTileManager();

#pragma region Tile Generation

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	bool DebugPrints = false;

	//attempt at managing random numbers and seeds
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	FRandomStream GameStream;


	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	TSubclassOf<ASTile> TileBase;
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	TSubclassOf<ASTileDoor> TileDoor;
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	int LevelHeight = 5;
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	int LevelWidth = 5;

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	ALocalLevel* MyLocalLevel;

	//are doors to be used in levels, some might not need them
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	bool DoorsActive = false;

	//root component
	UPROPERTY(VisibleAnywhere)
	USceneComponent* TilesRoot;

	//folder name for where tiles are placed when spawned
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	FName TileSubFolderName;

	//folder name for where doors are placed when spawned
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	FName DoorSubFolderName;

	//folder name for where doors are placed when spawned
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	FName TileGenRootFolder;

	//starting tile reference - TO DO: PROTECT THIS LATER
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	ASTile* StartingTile;

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	ASTile* choosen;

	//starting tile reference - TO DO: PROTECT THIS LATER
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	ASTile* EndTile;

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	ASTile* SecretRoom;

	//list of possible starting tiles - DO DO: PROTECT THIS LATER
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	TArray<ASTile*> PossibleStartingTiles;

	//list of possible starting tiles - DO DO: PROTECT THIS LATER
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	TArray<ASTile*> LevelPath;

	//list of possible starting tiles - DO DO: PROTECT THIS LATER
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	TArray<ASTile*> BackTrackHistory;

	//list of possible starting tiles - DO DO: PROTECT THIS LATER
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	int PathNumber = 0;

	//list of current active tiles - DO DO: PROTECT THIS LATER
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	TArray<ASTile*>	AllActiveTiles;

	//list of possible branching tiles, they can be used off main path or branches - DO DO: PROTECT THIS LATER
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	TArray<ASTile*>	AvailableTiles;

	//single rooms
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	int FillerRooms = 0;


	
#pragma endregion

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


#pragma region Tile Generation
	//2D array to hold all tiles
	TArray <FMultiTileStruct*> Grid2DArray;

	//Spawned in from LocalLevel
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	ASTile* PlayerSpawnPresentTile;

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	ASTile* PlayerStartingTileBase;

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	TArray<FTileInfoStruct> OutskirtTiles;

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void TileGeneration();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void SeedSetup();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void Create2DTileArray();

	UFUNCTION(Category = "ArrayCreation")
	void LinkTile(ASTile* ThisTile, FMultiTileStruct Col);

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void ChooseStartEndRooms();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void GeneratePath();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	bool AddTileToPath(ASTile* TileToAdd);

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void CheckTile(ASTile* TileToAdd, TArray<ASTile*> CurrentPath);

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void ClearHistory();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void AddRandomRooms();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void AddSingleRooms();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void CreateSpawnRoom();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void CreateSecretRoom();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void CheckBranchTile(ASTile* TileToAdd, TArray<ASTile*> CurrentPath, int Length);

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void MakeAvailableTiles();

	UFUNCTION(BlueprintCallable, Category = "Door Setup")
	void ActivateAllDoors();

	UFUNCTION(BlueprintCallable, Category = "Door Setup")
	void DeactiveInactiveRooms();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	TArray <int> Reshuffle2(TArray <int> ar);

	//For debug if we want to hard code a specific side to test, we set to 0 -3 otherwise it will get overridden
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	int StartRoomSide = -1;
#pragma endregion


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
