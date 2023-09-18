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

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	TSubclassOf<ASTile> TileBase;
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	TSubclassOf<ASTileDoor> TileDoor;
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	int Height = 5;
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	int Width = 5;

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	bool DoorsActive = false;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* TilesRoot;

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	FName TileSubFolderName;

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	FName DoorSubFolderName;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


#pragma region Tile Generation
	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void Create2DTileArray();

	UFUNCTION(Category = "ArrayCreation")
	void LinkTile(ASTile* Tile, FMultiTileStruct Col);

	//2D array to hold all tiles
	TArray <FMultiTileStruct*> MyArray;
	//TArray<TArray<AActor*>> test;

	

#pragma endregion


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
