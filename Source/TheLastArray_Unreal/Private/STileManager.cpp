// Fill out your copyright notice in the Description page of Project Settings.


#include "STileManager.h"
#include <Engine/World.h>
#include <string>
#include <Math/UnrealMathUtility.h>
#include <Kismet/KismetMathLibrary.h>
#include "../TheLastArray_Unreal.h"

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
			T->ShadeNull();
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
	int side = GameStream.RandRange(0, 3);
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

			//UE_LOG(LogTemp, Log, TEXT("Compare1 BEFORE: %d > %d"), startX, (LevelWidth - 1) / 2);
			if(startX > (LevelWidth - 1)/2) { //more than half AFTER startX
				for (int extra = 0; extra < (startX)-((LevelHeight) / 2); extra++) {
					for (int index3 = 0; index3 < LevelHeight - 1; index3++)
					{
						//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), LevelHeight - 1 - index3, extra);
						PossibleStartingTiles.Add(Grid2DArray[LevelHeight - 1 - index3]->TileColumn[extra]);
						Grid2DArray[LevelHeight - 1 - index3]->TileColumn[extra]->ShadeTestRoom();
					}
				}
			}
			//UE_LOG(LogTemp, Log, TEXT("Compare2 AFTER: %d < %d"), startX, (LevelWidth - 1) / 2);
			if (startX < ((LevelWidth - 1)/2)) { //more than after BEFORE startX
				for (int extra = 0; extra < (LevelHeight - startX - 1) - ((LevelHeight) / 2); extra++) {
					for (int index4 = 0; index4 < LevelHeight - 1; index4++)
					{
						//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), LevelHeight - 1 - index4, LevelWidth - 1 - extra);
						PossibleStartingTiles.Add(Grid2DArray[LevelHeight - 1 - index4]->TileColumn[LevelWidth - 1 - extra]);
						Grid2DArray[LevelHeight - 1 - index4]->TileColumn[LevelWidth - 1 - extra]->ShadeTestRoom();
					}
				}
			}


			break;
		case 1:
			startY = GameStream.RandRange(0, LevelWidth - 1);
			startX = 0;
			//LEFT;
			
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


			//UE_LOG(LogTemp, Log, TEXT("Compare1 BEFORE: %d < %d"), startY, (LevelHeight - 1) / 2);
			if (startY > (LevelHeight - 1) / 2) { //more than half BEFORE startX
				//UE_LOG(LogTemp, Log, TEXT("Check: %d"), (startY)-((LevelWidth) / 2));
				for (int extra = 0; extra < (startY)-((LevelWidth) / 2); extra++) {
					for (int index3 = 0; index3 < LevelWidth - 1; index3++)
					{
						//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), extra, LevelWidth - 1 - index3);
						PossibleStartingTiles.Add(Grid2DArray[extra]->TileColumn[LevelWidth - 1 - index3]);
						Grid2DArray[extra]->TileColumn[LevelWidth - 1 - index3]->ShadeTestRoom();
					}
				}
			}
			//UE_LOG(LogTemp, Log, TEXT("Compare2 AFTER: %d > %d"), startY, (LevelHeight - 1) / 2);
			if (startY < ((LevelHeight - 1) / 2)) { //more than after AFTER startX
				//UE_LOG(LogTemp, Log, TEXT("Check: %d"), (LevelWidth - startY - 1) - ((LevelWidth) / 2));
				for (int extra = 0; extra < (LevelWidth - startY - 1) - ((LevelWidth) / 2); extra++) {
					for (int index4 = 0; index4 < LevelWidth - 1; index4++)
					{
						//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), LevelHeight - 1 - extra, LevelWidth - 1 - index4);
						PossibleStartingTiles.Add(Grid2DArray[LevelHeight - 1 - extra]->TileColumn[LevelWidth - 1 - index4]);
						Grid2DArray[LevelHeight - 1 - extra]->TileColumn[LevelWidth - 1 - index4]->ShadeTestRoom();
					}
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

			//UE_LOG(LogTemp, Log, TEXT("Compare1 BEFORE: %d > %d"), startX, (LevelWidth - 1) / 2);
			if (startX > (LevelWidth - 1) / 2) { //more than half AFTER startX
				for (int extra = 0; extra < (startX)-((LevelHeight) / 2); extra++) {
					for (int index3 = 0; index3 < LevelHeight - 1; index3++)
					{
						//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), index3, extra);
						PossibleStartingTiles.Add(Grid2DArray[index3]->TileColumn[extra]);
						Grid2DArray[index3]->TileColumn[extra]->ShadeTestRoom();
					}
				}
			}
			//UE_LOG(LogTemp, Log, TEXT("Compare2 AFTER: %d < %d"), startX, (LevelWidth - 1) / 2);
			if (startX < ((LevelWidth - 1) / 2)) { //more than after BEFORE startX
				for (int extra = 0; extra < (LevelHeight - startX - 1) - ((LevelHeight) / 2); extra++) {
					for (int index4 = 0; index4 < LevelHeight - 1; index4++)
					{
						//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), index4, LevelWidth - 1 - extra);
						PossibleStartingTiles.Add(Grid2DArray[index4]->TileColumn[LevelWidth - 1 - extra]);
						Grid2DArray[index4]->TileColumn[LevelWidth - 1 - extra]->ShadeTestRoom();
					}
				}
			}

			break;
		case 3:
			startY = GameStream.RandRange(0, LevelHeight - 1);
			startX = LevelWidth - 1;
			//RIGHT;

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
					//UE_LOG(LogTemp, Log, TEXT("Compare1 BEFORE: %d > %d"), startY, (LevelHeight - 1) / 2);
				
					for (int index3 = 0; index3 < LevelHeight - 1; index3++)
					{
						//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), extra, index3);
						PossibleStartingTiles.Add(Grid2DArray[extra]->TileColumn[index3]);
						Grid2DArray[extra]->TileColumn[index3]->ShadeTestRoom();
					}
				}
			}
			if (startY < ((LevelHeight - 1) / 2)) { //more than after AFTER startX
				for (int extra = 0; extra < (LevelWidth - startY - 1) - ((LevelWidth) / 2); extra++) {
					//UE_LOG(LogTemp, Log, TEXT("Compare2 AFTER: %d < %d"), startY, (LevelHeight - 1) / 2);
					for (int index4 = 0; index4 < LevelHeight - 1; index4++)
					{
						//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"),  LevelWidth - 1 - extra, index4);
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
			for (int index2 = 0; index2 < (LevelWidth - 1) / 2; index2++) {
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

			//UE_LOG(LogTemp, Log, TEXT("Compare1 BEFORE: %d > %d"), startX, (LevelWidth - 1) / 2);
			if (startX > (LevelWidth - 1) / 2) { //more than half AFTER startX
				for (int extra = 0; extra < (startX)-((LevelHeight) / 2); extra++) {
					for (int index3 = 0; index3 < LevelHeight - 1; index3++)
					{
						//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), LevelHeight - 1 - index3, extra);
						PossibleStartingTiles.Add(Grid2DArray[LevelHeight - 1 - index3]->TileColumn[extra]);
						Grid2DArray[LevelHeight - 1 - index3]->TileColumn[extra]->ShadeTestRoom();
					}
				}
			}
			//UE_LOG(LogTemp, Log, TEXT("Compare2 AFTER: %d < %d"), startX, (LevelWidth - 1) / 2);
			if (startX < ((LevelWidth - 1) / 2)) { //more than after BEFORE startX
				for (int extra = 0; extra < (LevelHeight - startX - 1) - ((LevelHeight) / 2); extra++) {
					for (int index4 = 0; index4 < LevelHeight - 1; index4++)
					{
						//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), LevelHeight - 1 - index4, LevelWidth - 1 - extra);
						PossibleStartingTiles.Add(Grid2DArray[LevelHeight - 1 - index4]->TileColumn[LevelWidth - 1 - extra]);
						Grid2DArray[LevelHeight - 1 - index4]->TileColumn[LevelWidth - 1 - extra]->ShadeTestRoom();
					}
				}
			}

			break;
	}
	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Starting Tile is designated as [%d,%d]"), startY, startX);

	StartingTile = Grid2DArray[startY]->TileColumn[startX];
	StartingTile->ShadeStartingRoom();

	//end room is picked randomly from array of possible rooms
	EndTile = PossibleStartingTiles[GameStream.RandRange(0, PossibleStartingTiles.Num() - 1)];
	EndTile->ShadeEndRoom();

	GeneratePath();
}

//Use recursive backtracking maze algorithm
void ASTileManager::GeneratePath()
{
	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("=================== Genearating Path =============================="));

	//add starting room to be start of list
	AddTileToPath(StartingTile);

	//TArray<ASTile*> CurrentPath;
	CheckTile(StartingTile, LevelPath);
	


	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("=================== Finished Path =============================="));
}

bool ASTileManager::AddTileToPath(ASTile* TileToAdd)
{
	//if (TileToAdd->TileStatus != ETileStatus::ETile_STARTINGROOM)
	//{
		
	//}
	//else {
		//adding the starting tile to the list to begin
		LevelPath.Add(TileToAdd);
		TileToAdd->CheckForPath = true;
		TileToAdd->PathNumber = PathNumber;
		if(TileToAdd->TileStatus != ETileStatus::ETile_BOSSROOM && TileToAdd->TileStatus != ETileStatus::ETile_STARTINGROOM)
			TileToAdd->ShadePath();
		PathNumber++;

	//}

	return true;
}

void ASTileManager::CheckTile(ASTile* CurrentTile, TArray<ASTile*> CurrentPath)
{
	//if(CurrentTile) {
	//	UE_LOG(LogTemp, Log, TEXT("Currently on Tile: %d,%d"), CurrentTile->XIndex, CurrentTile->ZIndex);
	//}
	//else {
	//	UE_LOG(LogTemp, Log, TEXT("NULL TILE DETECTED. PLEASE INVESTIGATE"));
	//}

		//first check if all neighbors are unavailable
		//if so, make this one as checked and call on previous tiles

		if ((!CurrentTile->UpNeighbor || CurrentTile->UpNeighbor->CheckForPath) && (!CurrentTile->DownNeighbor || CurrentTile->DownNeighbor->CheckForPath)
			&& (!CurrentTile->RightNeighbor || CurrentTile->RightNeighbor->CheckForPath) && (!CurrentTile->LeftNeighbor || CurrentTile->LeftNeighbor->CheckForPath))
		{
			//UE_LOG(LogTemp, Log, TEXT("Dead end found at %d,%d"), CurrentTile->XIndex, CurrentTile->ZIndex);
			CurrentTile->CheckForPath = true;
			CurrentTile->ShadeNull();

			//eventually all of these tiles will be unchecked so a different path may go through them (dead ends,etc)
			BackTrackHistory.Add(CurrentTile);

			CurrentPath.Remove(CurrentTile);
			CheckTile(CurrentTile->PreviousTile, CurrentPath);
		} // boss room checks
		else if ((CurrentTile->UpNeighbor != NULL && CurrentTile->UpNeighbor->TileStatus == ETileStatus::ETile_BOSSROOM))
		{
			//UE_LOG(LogTemp, Log, TEXT("Found Boss Room! at %d,%d"), CurrentTile->UpNeighbor->XIndex, CurrentTile->UpNeighbor->ZIndex);
			CurrentTile->CheckForPath = true;
			AddTileToPath(CurrentTile);
			AddTileToPath(CurrentTile->UpNeighbor);

		}
		else if ((CurrentTile->DownNeighbor != NULL && CurrentTile->DownNeighbor->TileStatus == ETileStatus::ETile_BOSSROOM))
		{
			//UE_LOG(LogTemp, Log, TEXT("Found Boss Room! at %d,%d"), CurrentTile->DownNeighbor->XIndex, CurrentTile->DownNeighbor->ZIndex);
			CurrentTile->CheckForPath = true;
			AddTileToPath(CurrentTile);
			AddTileToPath(CurrentTile->DownNeighbor);
		}
		else if ((CurrentTile->RightNeighbor != NULL && CurrentTile->RightNeighbor->TileStatus == ETileStatus::ETile_BOSSROOM))
		{
			//UE_LOG(LogTemp, Log, TEXT("Found Boss Room! at %d,%d"), CurrentTile->RightNeighbor->XIndex, CurrentTile->RightNeighbor->ZIndex);
			CurrentTile->CheckForPath = true;
			AddTileToPath(CurrentTile);
			AddTileToPath(CurrentTile->RightNeighbor);
		}
		else if ((CurrentTile->LeftNeighbor != NULL && CurrentTile->LeftNeighbor->TileStatus == ETileStatus::ETile_BOSSROOM))
		{
			//UE_LOG(LogTemp, Log, TEXT("Found Boss Room! at %d,%d"), CurrentTile->LeftNeighbor->XIndex, CurrentTile->LeftNeighbor->ZIndex);
			CurrentTile->CheckForPath = true;
			AddTileToPath(CurrentTile);
			AddTileToPath(CurrentTile->LeftNeighbor);
		}
		else {
			//now that we know theres valid neighbors and none of them are the boss room, lets check our neighbors
		
			//UE_LOG(LogTemp, Log, TEXT("Path Checking: %d,%d"), CurrentTile->XIndex, CurrentTile->ZIndex);
			//direction
			TArray <int> DirectionsToCheck = { 1, 2, 3, 4 };

			DirectionsToCheck = Reshuffle2(DirectionsToCheck);
			//DirectionsToCheck.

			//pick direction and begin CheckTile
			for (int DirectionCount = 0; DirectionCount < DirectionsToCheck.Num(); DirectionCount++)
			{
				switch (DirectionsToCheck[DirectionCount])
				{
				case 1:
					//UP
					if (CurrentTile->UpNeighbor && !CurrentTile->UpNeighbor->CheckForPath && CurrentTile->UpNeighbor->TileStatus != ETileStatus::ETile_STARTINGROOM && 
					CurrentTile->UpNeighbor->TileStatus == ETileStatus::ETile_NULLROOM)
					{
						//add this tile to path, go to up neighbor
						//UE_LOG(LogTemp, Log, TEXT("Up Neighbor Valid: %d,%d - going there"), CurrentTile->UpNeighbor->XIndex, CurrentTile->UpNeighbor->ZIndex);
						CurrentTile->UpNeighbor->PreviousTile = CurrentTile;
						AddTileToPath(CurrentTile);
						//no need to keep going through other directions directions
						DirectionCount = 5;
						CheckTile(CurrentTile->UpNeighbor, CurrentPath);
					}
					break;
				case 2:
					//DOWN
					if (CurrentTile->DownNeighbor && !CurrentTile->DownNeighbor->CheckForPath && CurrentTile->DownNeighbor->TileStatus != ETileStatus::ETile_STARTINGROOM && 
					CurrentTile->DownNeighbor->TileStatus == ETileStatus::ETile_NULLROOM)
					{
						//UE_LOG(LogTemp, Log, TEXT("Down Neighbor Valid: %d,%d - going there"), CurrentTile->DownNeighbor->XIndex, CurrentTile->DownNeighbor->ZIndex);
						CurrentTile->DownNeighbor->PreviousTile = CurrentTile;
						AddTileToPath(CurrentTile);
						DirectionCount = 5;
						CheckTile(CurrentTile->DownNeighbor, CurrentPath);
					}
					break;
				case 3:
					//LEFT
					if (CurrentTile->LeftNeighbor && !CurrentTile->LeftNeighbor->CheckForPath && CurrentTile->LeftNeighbor->TileStatus != ETileStatus::ETile_STARTINGROOM && 
					CurrentTile->LeftNeighbor->TileStatus == ETileStatus::ETile_NULLROOM)
					{
						//UE_LOG(LogTemp, Log, TEXT("Left Neighbor Valid: %d,%d - going there"), CurrentTile->LeftNeighbor->XIndex, CurrentTile->LeftNeighbor->ZIndex);
						CurrentTile->LeftNeighbor->PreviousTile = CurrentTile;
						AddTileToPath(CurrentTile);
						DirectionCount = 5;
						CheckTile(CurrentTile->LeftNeighbor, CurrentPath);
					}
					break;
				case 4:
					//RIGHT
					if (CurrentTile->RightNeighbor && !CurrentTile->RightNeighbor->CheckForPath && CurrentTile->RightNeighbor->TileStatus != ETileStatus::ETile_STARTINGROOM && 
					CurrentTile->RightNeighbor->TileStatus == ETileStatus::ETile_NULLROOM)
					{
						//UE_LOG(LogTemp, Log, TEXT("Right Neighbor Valid: %d,%d - going there"), CurrentTile->RightNeighbor->XIndex, CurrentTile->RightNeighbor->ZIndex);
						CurrentTile->RightNeighbor->PreviousTile = CurrentTile;
						AddTileToPath(CurrentTile);
						DirectionCount = 5;
						CheckTile(CurrentTile->RightNeighbor, CurrentPath);
					}
					break;
				}
			}
		//}
	}
}

//History is mostly for debug
void ASTileManager::ClearHistory()
{
	//for each(ASTile* Tile in BackTrackHistory)
	for(int BIndex = 0; BIndex < BackTrackHistory.Num(); BIndex++)
	{
		BackTrackHistory[BIndex]->CheckForPath = false;
	}
}


TArray <int> ASTileManager::Reshuffle2(TArray <int> ar)
{
	// Knuth shuffle algorithm :: courtesy of Wikipedia :)
	for (int t = 0; t < ar.Num(); t++)
	{
		//int tmp = ar[t];
		int r = GameStream.RandRange(t, ar.Num() - 1);
		//UE_LOG(LogTemp, Log, TEXT("Swapping: %d and %d"), t, r);
		ar.Swap(t, r);
	}
	return ar;
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
	//UE_LOG(LogTemp, Log, TEXT("Linking Number: %f : %f"), ThisTile->XIndex, ThisTile->ZIndex);
	//for now I'm going to make right direction the positive one and left is negative

	if (ThisTile->ZIndex > 0)
	{
		ThisTile->DownNeighbor = Col.TileColumn[ThisTile->ZIndex - 1];
		ASTile* DownNeighbor = ThisTile->DownNeighbor;
		Col.TileColumn[ThisTile->ZIndex - 1]->UpNeighbor = ThisTile;
		//UE_LOG(LogTemp, Log, TEXT("Down neighbor: %f : %f"), DownNeighbor->XIndex, DownNeighbor->ZIndex);

		//if doors are active, spawn a door at the tile placeholder
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		//if our lower neighbor doesn't already have a door connecting up
		//UE_LOG(LogTemp, Log, TEXT("In script spawning doors"));
		if (!DownNeighbor->UpDoor)
		{
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
		//UE_LOG(LogTemp, Log, TEXT("Left neighbor: %f : %f"), LeftNeighbor->XIndex, LeftNeighbor->ZIndex);

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

