// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STile.h"
#include "LocalLevel.generated.h"

UCLASS()
class THELASTARRAY_UNREAL_API ALocalLevel : public AActor
{
	/// <summary>
	/// Local Level
	/// Dylan Loe
	/// 
	/// 
	/// Notes:
	/// - Keeps track of local level info and setup
	/// - sets one of the objective types 
	/// - sets up what type of tile assets we use
	///		- IE StartingTile preset
	///		- pulls and assigns level assets from Scriptable objects
	///		- pulls and sets types or variants of level objectives
	/// 
	/// Potentially could include:
	/// - 
	/// 
	/// </summary>
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALocalLevel();

	UPROPERTY(EditAnywhere, Category = "Tile Configuration")
	TSubclassOf<ASTile> PresetStartingTile;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TSubclassOf<ASTile> GetPresetStartingTile();

};
