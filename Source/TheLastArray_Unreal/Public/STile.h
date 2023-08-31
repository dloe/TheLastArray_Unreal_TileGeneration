// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STile.generated.h"

UCLASS()
class THELASTARRAY_UNREAL_API ASTile : public AActor
{
	/// <summary>
	/// Tile
	/// Dylan Loe
	/// 
	/// Notes:
	/// - Core building block of tile map
	/// - will contain references to doors connecting each other
	/// 
	/// </summary>
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASTile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "Tile Stats")
	float TileLength;


};
