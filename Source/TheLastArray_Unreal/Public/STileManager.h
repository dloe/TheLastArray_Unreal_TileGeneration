// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STile.h"
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

	//starting tile reference - TO DO: PROTECT THIS LATER
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	ASTile* StartingTile;

	//starting tile reference - TO DO: PROTECT THIS LATER
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	ASTile* EndTile;

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

#pragma endregion

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


#pragma region Tile Generation
	//2D array to hold all tiles
	TArray <FMultiTileStruct*> Grid2DArray;
	//TArray<TArray<AActor*>> test;

	

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void TileGeneration();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void SeedSetup();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void Create2DTileArray();

	UFUNCTION(Category = "ArrayCreation")
	void LinkTile(ASTile* Tile, FMultiTileStruct Col);

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

#pragma endregion


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
