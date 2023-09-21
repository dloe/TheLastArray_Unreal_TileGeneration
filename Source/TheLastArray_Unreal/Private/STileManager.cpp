// Fill out your copyright notice in the Description page of Project Settings.


#include "STileManager.h"
#include <Engine/World.h>
#include <string>
#include <Math/UnrealMathUtility.h>
#include <Kismet/KismetMathLibrary.h>

// Sets default values
ASTileManager::ASTileManager()
{
	

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TilesRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TilesRoot"));
	RootComponent = TilesRoot;

	//SeedSetup();
}

void ASTileManager::SeedSetup()
{
	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Setting Up Seed..."));

	GameStream.Initialize("GameSeed");
	GameStream.GenerateNewSeed();
	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Seed: %d"), GameStream.GetCurrentSeed());
}

/// <summary>
/// - on begin play, 2d array is populated and established. For now we will use hard coded sizes. 
/// TO DO: Switch to scriptable objects to determine difficulty and sizing
/// 
/// </summary>
void ASTileManager::BeginPlay()
{
	Super::BeginPlay();
	if (DebugPrints) {
		UE_LOG(LogTemp, Log, TEXT("==========================================================="));
		UE_LOG(LogTemp, Log, TEXT("================= TILE GENERATION ========================="));
		UE_LOG(LogTemp, Log, TEXT("==========================================================="));
	}
	SeedSetup();
	TileGeneration();

}

void ASTileManager::TileGeneration()
{
	


	//create and link tiles into grid
	//this includes establishment of doors if we need them
	Create2DTileArray();

	//once tiles are established, we now pick the starting tile
	ChooseStartEndRooms();

	//
}



/// <summary>
/// Dylan Loe
/// 
/// Creation of 2d array for tiles
/// Creates tiles
/// </summary>
void ASTileManager::Create2DTileArray()
{
	
	if(DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("===================Creating 2D array!=============================="));
		
	for (int32 XIndex = 0; XIndex < LevelWidth; XIndex++)
	{
		//UE_LOG(LogTemp, Log, TEXT("Column Number: %f"), XIndex);
		//for each row, make each column
		FMultiTileStruct* Col = new FMultiTileStruct();

		//const FTransform SpawnTM = FTransform(this->GetActorRotation(), this->GetActorLocation());
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		//Populate TileColumn array with Tiles for Height size
		for (int32 ZIndex = 0; ZIndex < LevelHeight; ZIndex++)
		{
			//spawn in a Tile
			//UE_LOG(LogTemp, Log, TEXT("--Tile: %f, %f"), XIndex, ZIndex);
			FString TileName = "Tile_Row" + FString::FromInt(XIndex) + "_Col" + FString::FromInt(ZIndex);
			//if (DebugPrints)
				//UE_LOG(LogTemp, Log, TEXT("--Tile: %s"), *TileName);
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
			T->ShadeActiveRoom();
		}


		Grid2DArray.Add(Col);
	}
	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("===================2D array CREATED!=============================="));
}

/// <summary>
/// Dylan Loe
/// 
/// Choosing the start and end room
/// 
/// TO DO: Do i really need to pick the end room?
/// 
/// </summary>
void ASTileManager::ChooseStartEndRooms()
{
	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("=================== Choosing start and end rooms =============================="));

	int startX = 0, startY = 0;
	//will pick a random side and random tile on side to start
	int side = 3;//GameStream.RandRange(0, 3);
	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Side Picked: %f"), side);



	switch (side)
	{
		case 0:
		//starting
			startY = 0;
			startX = GameStream.RandRange(0, LevelHeight - 1);
			//UE_LOG(LogTemp, Log, TEXT("num Picked: %d"), startX);
			//DOWN;

		//ending possible 
		//opposite of startX (0) would be LevelWidth - 1
		//for choosing the end tile, we will pick the opposite 2 rows/columns
		//must be a distance of height/2 and/or column/2 to be added to a list to be randomly picked as end room
		//cant pick tiles that are on the same row or column (no straight shots)
		
			for(int index2 = 0; index2 < (LevelWidth - 1)/2; index2++) {
			//take every tile less than startY
				for (int index = 0; index < startX; index++)
				{
					//ASTile* Possible = Grid2DArray[LevelWidth - 1]->TileColumn[index];
					PossibleStartingTiles.Add(Grid2DArray[LevelWidth - 1 - index2]->TileColumn[index]);
					Grid2DArray[LevelWidth - 1 - index2]->TileColumn[index]->ShadeTestRoom();
				}

				//take every tile greater than startY
				for (int index = LevelHeight - 1; index > startX; index--)
				{
					PossibleStartingTiles.Add(Grid2DArray[LevelWidth - 1 - index2]->TileColumn[index]);
					Grid2DArray[LevelWidth - 1 - index2]->TileColumn[index]->ShadeTestRoom();
				}
			}

			UE_LOG(LogTemp, Log, TEXT("Compare1 BEFORE: %d > %d"), startX, (LevelWidth - 1) / 2);
			if(startX > (LevelWidth - 1)/2) { //more than half AFTER startX
				
				for (int index3 = 0; index3 < LevelHeight - 1; index3++)
				{
						UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), 0, LevelHeight - 1 - index3);
						PossibleStartingTiles.Add(Grid2DArray[LevelHeight - 1 - index3]->TileColumn[0]);
						Grid2DArray[0]->TileColumn[LevelHeight - 1 - index3]->ShadeTestRoom();
				}
			}
			UE_LOG(LogTemp, Log, TEXT("Compare2 AFTER: %d < %d"), startX, (LevelWidth - 1) / 2);
			if (startX < ((LevelWidth - 1)/2)) { //more than after BEFORE startX
				
				for (int index4 = 0; index4 < LevelHeight - 1; index4++)
				{
						UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), LevelHeight - 1 - index4, LevelWidth - 1);
						PossibleStartingTiles.Add(Grid2DArray[LevelWidth - 1]->TileColumn[LevelHeight - 1 - index4]);
						Grid2DArray[LevelHeight - 1 - index4]->TileColumn[LevelWidth - 1]->ShadeTestRoom();
				}
			}


			break;
		case 1:
			startY = GameStream.RandRange(0, LevelWidth - 1);
			startX = 0;
			//RIGHT;
			
			for (int index2 = 0; index2 < (LevelHeight - 1) / 2; index2++) {
				//take every tile less than startY
				for (int index = 0; index < startY; index++)
				{
					//ASTile* Possible = Grid2DArray[LevelWidth - 1]->TileColumn[index];
					PossibleStartingTiles.Add(Grid2DArray[index]->TileColumn[LevelHeight - 1 - index2]);
					Grid2DArray[index]->TileColumn[LevelHeight - 1 - index2]->ShadeTestRoom();
				}

				//take every tile greater than startY
				for (int index = LevelHeight - 1; index > startY; index--)
				{
					PossibleStartingTiles.Add(Grid2DArray[index]->TileColumn[LevelHeight - 1 - index2]);
					Grid2DArray[index]->TileColumn[LevelHeight - 1 - index2]->ShadeTestRoom();
				}
			}

			UE_LOG(LogTemp, Log, TEXT("Compare1 BEFORE: %d < %d"), startY, (LevelHeight - 1) / 2);
			if (startY < (LevelHeight - 1) / 2) { //more than half AFTER startX

				for (int index3 = 0; index3 < LevelWidth - 1; index3++)
				{
					UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), 0, LevelWidth - 1 - index3);
					PossibleStartingTiles.Add(Grid2DArray[LevelWidth - 1 - index3]->TileColumn[0]);
					Grid2DArray[0]->TileColumn[LevelWidth - 1 - index3]->ShadeTestRoom();
				}
			}
			UE_LOG(LogTemp, Log, TEXT("Compare2 AFTER: %d ? %d"), startY, (LevelHeight - 1) / 2);
			if (startY > ((LevelHeight - 1) / 2)) { //more than after BEFORE startX

				for (int index4 = 0; index4 < LevelWidth - 1; index4++)
				{
					UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), LevelWidth - 1 - index4, LevelHeight - 1);
					PossibleStartingTiles.Add(Grid2DArray[LevelHeight - 1]->TileColumn[LevelWidth - 1 - index4]);
					Grid2DArray[LevelWidth - 1 - index4]->TileColumn[LevelHeight - 1]->ShadeTestRoom();
				}
			}
			
			
			break;
		case 2:
			startY = LevelWidth - 1;
			startX = GameStream.RandRange(0, LevelHeight - 1);
			//UP;

			for (int index2 = 0; index2 < (LevelWidth - 1) / 2; index2++) {
				//take every tile less than startY
				for (int index = 0; index < startX; index++)
				{
					PossibleStartingTiles.Add(Grid2DArray[0 + index2]->TileColumn[index]);
					Grid2DArray[0 + index2]->TileColumn[index]->ShadeTestRoom();
				}

				//take every tile greater than startY
				for (int index = LevelHeight - 1; index > startX; index--)
				{
					PossibleStartingTiles.Add(Grid2DArray[0 + index2]->TileColumn[index]);
					Grid2DArray[0 + index2]->TileColumn[index]->ShadeTestRoom();
				}
			}

			UE_LOG(LogTemp, Log, TEXT("Compare1 BEFORE: %d > %d"), startX, (LevelWidth - 1) / 2);
			if (startX > (LevelWidth - 1) / 2) { //more than half AFTER startX

				for (int index3 = 0; index3 < LevelHeight - 1; index3++)
				{
					UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), index3, 0);
					PossibleStartingTiles.Add(Grid2DArray[index3]->TileColumn[0]);
					Grid2DArray[index3]->TileColumn[0]->ShadeTestRoom();
				}
			}
			UE_LOG(LogTemp, Log, TEXT("Compare2 AFTER: %d < %d"), startX, (LevelWidth - 1) / 2);
			if (startX < ((LevelWidth - 1) / 2)) { //more than after BEFORE startX

				for (int index4 = 0; index4 < LevelHeight - 1; index4++)
				{
					UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), index4, LevelWidth - 1);
					PossibleStartingTiles.Add(Grid2DArray[index4]->TileColumn[LevelWidth - 1]);
					Grid2DArray[index4]->TileColumn[LevelWidth - 1]->ShadeTestRoom();
				}
			}

			break;
		case 3:
			startY = GameStream.RandRange(0, LevelHeight - 1);
			startX = LevelWidth - 1;
			//LEFT;

			for (int index2 = 0; index2 < (LevelHeight - 1) / 2; index2++) {
				//take every tile less than startY
				for (int index = 0; index < startY; index++)
				{
					PossibleStartingTiles.Add(Grid2DArray[index]->TileColumn[index2]);
					Grid2DArray[index]->TileColumn[index2]->ShadeTestRoom();
				}

				//take every tile greater than startY
				for (int index = LevelWidth - 1; index > startY; index--)
				{
					PossibleStartingTiles.Add(Grid2DArray[index]->TileColumn[index2]);
					Grid2DArray[index]->TileColumn[index2]->ShadeTestRoom();
				}
			}
			
			if (startY > (LevelHeight - 1) / 2) { //more than half BEFORE startX
				for (int extra = 0; extra < (startY)-((LevelWidth) / 2); extra++) {
					UE_LOG(LogTemp, Log, TEXT("Compare1 BEFORE: %d > %d"), startY, (LevelHeight - 1) / 2);
				
					for (int index3 = 0; index3 < LevelHeight - 1; index3++)
					{
						UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), extra, index3);
						PossibleStartingTiles.Add(Grid2DArray[extra]->TileColumn[index3]);
						Grid2DArray[extra]->TileColumn[index3]->ShadeTestRoom();
					}
				}
			}
			
			if (startY < ((LevelHeight - 1) / 2)) { //more than after AFTER startX
				for (int extra = 0; extra < (LevelWidth - startY) - ((LevelWidth) / 2); extra++) {
					UE_LOG(LogTemp, Log, TEXT("Compare2 AFTER: %d < %d"), startY, (LevelHeight - 1) / 2);
					for (int index4 = 0; index4 < LevelHeight - 1; index4++)
					{
						UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"),  LevelWidth - 1 - extra, index4);
						PossibleStartingTiles.Add(Grid2DArray[LevelWidth - 1 - extra]->TileColumn[index4]);
						Grid2DArray[LevelWidth - 1 - extra]->TileColumn[index4]->ShadeTestRoom();
					}
				}
			}

			break;
		default:
			if (DebugPrints)
				UE_LOG(LogTemp, Log, TEXT("Issue here -> picked weird starting side in ChooseStartEndRooms"));
			//startY = 0;
			//startX = GameStream.RandRange(0, LevelHeight - 1);
			//DOWN	


			break;
	}
	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Starting Tile is designated as [%d,%d]"), startY, startX);

	StartingTile = Grid2DArray[startY]->TileColumn[startX];
	StartingTile->ShadeStartingRoom();

	
	

}

int ASTileManager::RandomInt2_Implementation(int MaxInt)
{
	return NULL;
}

/// <summary>
/// Dylan Loe
/// 
/// ASSIGNING THE NEIGHBORS AND LINKING DOORS PER TILE
/// connect this tile with the tiles to the left and below
/// must have a HeightIndex less than us and greater than or = to 0
/// must have a WidthIndex less than us and greater than or = to 0
/// 
/// </summary>
/// <param name="ThisTile"></param>
/// <param name="Col"></param>
void ASTileManager::LinkTile(ASTile* ThisTile, FMultiTileStruct Col)
{
	//if (DebugPrints)
	//UE_LOG(LogTemp, Log, TEXT("Linking Number: %f : %f"), Tile->XIndex, Tile->ZIndex);
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
	}
	if (ThisTile->XIndex > 0)
	{
		ASTile* LeftNeighbor = ThisTile->LeftNeighbor = Grid2DArray[ThisTile->XIndex - 1]->TileColumn[ThisTile->ZIndex];
		Grid2DArray[ThisTile->XIndex - 1]->TileColumn[ThisTile->ZIndex]->RightNeighbor = ThisTile;

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

