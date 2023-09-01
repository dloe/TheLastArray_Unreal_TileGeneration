// Fill out your copyright notice in the Description page of Project Settings.


#include "STileManager.h"
#include <Engine/World.h>
#include <string>

// Sets default values
ASTileManager::ASTileManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}


/// <summary>
/// - on begin play, 2d array is populated and established. For now we will use hard coded sizes. 
/// TO DO: Switch to scriptable objects to determine difficulty and sizing
/// 
/// </summary>
void ASTileManager::BeginPlay()
{
	Super::BeginPlay();

	Create2DTileArray();

}

void ASTileManager::Create2DTileArray()
{
	UE_LOG(LogTemp, Log, TEXT("===================Creating 2D array!=============================="));
	for (int32 XIndex = 0; XIndex < Width; XIndex++)
	{
		UE_LOG(LogTemp, Log, TEXT("Column Number: %f"), XIndex);
		//for each row, make each column
		FMultiTileStruct* Col = new FMultiTileStruct();

		//const FTransform SpawnTM = FTransform(this->GetActorRotation(), this->GetActorLocation());
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		//Populate TileColumn array with Tiles for Height size
		for (int32 ZIndex = 0; ZIndex < Height; ZIndex++)
		{
			//spawn in a Tile
			//UE_LOG(LogTemp, Log, TEXT("--Tile: %f, %f"), XIndex, ZIndex);
			FString TileName = "Tile_Row" + FString::FromInt(XIndex) + "_Col" + FString::FromInt(ZIndex);
			//UE_LOG(LogTemp, Log, TEXT("--Tile: %s"), *TileName);
			//UE_LOG(LogTemp, Log, TEXT("--Tile: %f, %f"), XIndex, ZIndex);
			ASTile* T = GetWorld()->SpawnActor<ASTile>(TileBase, this->GetActorLocation(), this->GetActorRotation(), SpawnParams);
			T->SetActorLabel(TileName);
			FVector NewLocal = FVector((this->GetActorLocation().X + (100 * XIndex)), (this->GetActorLocation().Z + (100 * ZIndex)), this->GetActorLocation().Y);
			T->SetActorLocation(NewLocal);
			T->XIndex = XIndex;
			T->ZIndex = ZIndex;

			//add tile to array
			Col->TileColumn.Add(T);
			LinkTile(T, *Col);
		}


		MyArray.Add(Col);
	}

}

void ASTileManager::LinkTile(ASTile* Tile, FMultiTileStruct Col)
{
	//UE_LOG(LogTemp, Log, TEXT("Linking Number: %f : %f"), Tile->XIndex, Tile->ZIndex);
	//connect this tile with the tiles to the left and below
	//must have a HeightIndex less than us and greater than or = to 0
	//must have a WidthIndex less than us and greater than or = to 0

	//for now I'm going to make right direction the positive one and left is negative

	if (Tile->ZIndex > 0)
	{
		Tile->DownNeighbor = Col.TileColumn[Tile->ZIndex - 1];
		Col.TileColumn[Tile->ZIndex - 1]->UpNeighbor = Tile;
	}
	if (Tile->XIndex > 0)
	{
		Tile->LeftNeighbor = MyArray[Tile->XIndex - 1]->TileColumn[Tile->ZIndex];
		MyArray[Tile->XIndex - 1]->TileColumn[Tile->ZIndex]->RightNeighbor = Tile;
	}
}

// Called every frame
void ASTileManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

