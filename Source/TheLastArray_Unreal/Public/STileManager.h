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
		UE_LOG(LogTemp, Log, TEXT("In FMultiTileStruct Constructor..."));
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

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASTile> TileBase;
	UPROPERTY(EditAnywhere)
	int Height = 5;
	UPROPERTY(EditAnywhere)
	int Width = 5;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void Create2DTileArray();

	//2D array to hold all tiles
	TArray <FMultiTileStruct*> MyArray;
	//TArray<TArray<AActor*>> test;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
