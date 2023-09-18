// Fill out your copyright notice in the Description page of Project Settings.


#include "STileManager.h"
#include <Engine/World.h>
#include <string>

// Sets default values
ASTileManager::ASTileManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TilesRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TilesRoot"));
	RootComponent = TilesRoot;
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
		//UE_LOG(LogTemp, Log, TEXT("Column Number: %f"), XIndex);
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
			UE_LOG(LogTemp, Log, TEXT("--Tile: %s"), *TileName);
			//UE_LOG(LogTemp, Log, TEXT("--Tile: %f, %f"), XIndex, ZIndex);
			//FVector NewLocal = FVector((this->GetActorLocation().X + (T->TileLength* 100 * XIndex)), (this->GetActorLocation().Z + (T->TileLength * 100 * ZIndex)), this->GetActorLocation().Y);
			ASTile* T = GetWorld()->SpawnActor<ASTile>(TileBase, FVector((this->GetActorLocation().X + (5 * 100 * XIndex)), (this->GetActorLocation().Z + (5 * 100 * ZIndex)), this->GetActorLocation().Y), this->GetActorRotation(), SpawnParams);
			T->SetActorLabel(TileName);
			T->SetOwner(this);
#if WITH_EDITOR
			T->SetFolderPath(TileSubFolderName);
#endif
			
			T->XIndex = XIndex;
			T->ZIndex = ZIndex;

			//add tile to array
			Col->TileColumn.Add(T);
			//T->SetUpDoorTransforms();
			LinkTile(T, *Col);
		}


		MyArray.Add(Col);
	}
	UE_LOG(LogTemp, Log, TEXT("===================2D array CREATED!=============================="));
}

void ASTileManager::LinkTile(ASTile* ThisTile, FMultiTileStruct Col)
{
	//UE_LOG(LogTemp, Log, TEXT("Linking Number: %f : %f"), Tile->XIndex, Tile->ZIndex);
	//connect this tile with the tiles to the left and below
	//must have a HeightIndex less than us and greater than or = to 0
	//must have a WidthIndex less than us and greater than or = to 0

	//for now I'm going to make right direction the positive one and left is negative

	if (ThisTile->ZIndex > 0)
	{
		ASTile* DownNeighbor = ThisTile->DownNeighbor;
		DownNeighbor = Col.TileColumn[ThisTile->ZIndex - 1];
		Col.TileColumn[ThisTile->ZIndex - 1]->UpNeighbor = ThisTile;

		//if doors are active, spawn a door at the tile placeholder
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		//if our lower neighbor doesn't already have a door connecting up
		//UE_LOG(LogTemp, Log, TEXT("In script spawning doors"));
		if (!DownNeighbor->UpDoor)
		{
			//FVector DownDoorLocation = new FVector(0, -250, 120);
			//FRotator DownDoorRotation = new FRotator(270, 0, 180);
			const FString TileDownDoorName = "TileDoorConnecting_" + FString::FromInt(ThisTile->XIndex) + "_" + FString::FromInt(ThisTile->ZIndex) + "_to_" + FString::FromInt(DownNeighbor->XIndex) + "_" + FString::FromInt(DownNeighbor->ZIndex);
			const FVector DownDoorSpawnLocation = ThisTile->DownDoorSpawnPoint.GetLocation() + ThisTile->GetActorLocation();
			const FTransform Spawm = FTransform(ThisTile->DownDoorSpawnPoint.GetRotation(), DownDoorSpawnLocation);
			ThisTile->DownDoor = GetWorld()->SpawnActor<ASTileDoor>(TileDoor, Spawm, SpawnParams);
			ThisTile->DownDoor->SetActorLabel(TileDownDoorName);
			ThisTile->DownDoor->SetOwner(ThisTile);
#if WITH_EDITOR
			ThisTile->DownDoor->SetFolderPath(DoorSubFolderName);
#endif
			DownNeighbor->UpDoor = ThisTile->DownDoor;
		}

		//if (ThisTile->ZIndex < Height)
		//{
			//ASTile* UpNeighbor = ThisTile->UpNeighbor;
			//if (!UpNeighbor->DownDoor)
			//{
				//FString TileDownDoorName = "TileDoorConnecting_" + FString::FromInt(ThisTile->XIndex) + "_" + FString::FromInt(ThisTile->ZIndex) + "_to_" + FString::FromInt(UpNeighbor->XIndex) + "_" + FString::FromInt(UpNeighbor->ZIndex);
				//ThisTile->UpDoor = GetWorld()->SpawnActor<ASTileDoor>(TileDoor, ThisTile->UpDoorSpawnPoint->GetActorLocation(), ThisTile->UpDoorSpawnPoint->GetActorRotation(), SpawnParams);

				//UpNeighbor->UpDoor = ThisTile->UpDoor;
			//}
		//}
	}
	if (ThisTile->XIndex > 0)
	{
		ASTile* LeftNeighbor = ThisTile->LeftNeighbor = MyArray[ThisTile->XIndex - 1]->TileColumn[ThisTile->ZIndex];
		MyArray[ThisTile->XIndex - 1]->TileColumn[ThisTile->ZIndex]->RightNeighbor = ThisTile;

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		//if our lower neighbor doesn't already have a door connecting up
		if (!LeftNeighbor->RightDoor)
		{
			FString TileRightDoorName = "TileDoorConnecting_" + FString::FromInt(ThisTile->XIndex) + "_" + FString::FromInt(ThisTile->ZIndex) + "_to_" + FString::FromInt(LeftNeighbor->XIndex) + "_" + FString::FromInt(LeftNeighbor->ZIndex);
			
			//add LeftDoorSpawnPoint location to ThisTiles location to give world location
			FVector LeftDoorSpawnLocation = ThisTile->LeftDoorSpawnPoint.GetLocation() + ThisTile->GetActorLocation();
			//FRotator LeftDoorSpawnRotation = ThisTile->LeftDoorSpawnPoint.GetRotation() + ThisTile->GetActorRotation();
			const FTransform Spawm = FTransform(ThisTile->LeftDoorSpawnPoint.GetRotation(), LeftDoorSpawnLocation);
			ThisTile->RightDoor = GetWorld()->SpawnActor<ASTileDoor>(TileDoor, Spawm, SpawnParams);
			ThisTile->RightDoor->SetActorLabel(TileRightDoorName);
			ThisTile->RightDoor->SetOwner(ThisTile);
#if WITH_EDITOR
			ThisTile->RightDoor->SetFolderPath(DoorSubFolderName);
#endif
			LeftNeighbor->RightDoor = ThisTile->RightDoor;
		}
	}
}

// Called every frame
void ASTileManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

