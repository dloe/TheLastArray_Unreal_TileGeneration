// Fill out your copyright notice in the Description page of Project Settings.


#include "STileManager.h"
#include "LocalLevel.h"
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
/// Dylan Loe
/// 
/// - on begin play, 2d array is populated and established. For now we will use hard coded sizes. 
/// TO DO: Switch to scriptable objects to determine difficulty and sizing
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

/// <summary>
/// Dylan Loe
/// 
/// - Create Tile map structure and run procedure to create game play path and populate the map
/// </summary>
void ASTileManager::TileGeneration()
{
	//create and link tiles into grid
	//this includes establishment of doors if we need them
	Create2DTileArray();

	//once tiles are established, we now pick the starting tile
	ChooseStartEndRooms();

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
		//for each row, make each column
		FMultiTileStruct* Col = new FMultiTileStruct();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		//Populate TileColumn array with Tiles for Height size
		for (int32 ZIndex = 0; ZIndex < LevelHeight; ZIndex++)
		{
			//spawn in a Tile
			FString TileName = "Tile_Row" + FString::FromInt(XIndex) + "_Col" + FString::FromInt(ZIndex);

			ASTile* T = GetWorld()->SpawnActor<ASTile>(TileBase, FVector((this->GetActorLocation().X + (500 * XIndex)), (this->GetActorLocation().Y + (5 * 100 * ZIndex)), this->GetActorLocation().Z), this->GetActorRotation(), SpawnParams);
			T->SetActorLabel(TileName);
			T->SetOwner(this);
#if WITH_EDITOR
			T->SetFolderPath(TileSubFolderName);
#endif
			
			T->XIndex = XIndex;
			T->ZIndex = ZIndex;

			//add tile to array
			Col->TileColumn.Add(T);
			//SetUpDoorTransforms will eventually be implemented here
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
/// - First run through start room logic, our beginning must be on a random side on the outskirts of the map,
/// - end room location must in turn be opposite and follow criteria to maintain it having a certain distance of possible path from start room
/// 
/// </summary>
void ASTileManager::ChooseStartEndRooms()
{
	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("=================== Choosing start and end rooms =============================="));

	int startX = 0, startY = 0;
	//will pick a random side and random tile on side to start
	if(StartRoomSide == -1)
		StartRoomSide = GameStream.RandRange(0, 3);
	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Side Picked: %d"), StartRoomSide);

	switch (StartRoomSide)
	{
		case 0:
			//starting
			startY = 0;
			startX = GameStream.RandRange(0, LevelHeight - 1);
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

			if(startX > (LevelWidth - 1)/2) { //more than half AFTER startX
				for (int extra = 0; extra < (startX)-((LevelHeight) / 2); extra++) {
					for (int index3 = 0; index3 < LevelHeight - 1; index3++)
					{
						PossibleStartingTiles.Add(Grid2DArray[LevelHeight - 1 - index3]->TileColumn[extra]);
						Grid2DArray[LevelHeight - 1 - index3]->TileColumn[extra]->ShadeTestRoom();
					}
				}
			}

			if (startX < ((LevelWidth - 1)/2)) { //more than after BEFORE startX
				for (int extra = 0; extra < (LevelHeight - startX - 1) - ((LevelHeight) / 2); extra++) {
					for (int index4 = 0; index4 < LevelHeight - 1; index4++)
					{
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

			if (startY > (LevelHeight - 1) / 2) { //more than half BEFORE startX
				//UE_LOG(LogTemp, Log, TEXT("Check: %d"), (startY)-((LevelWidth) / 2));
				for (int extra = 0; extra < (startY)-((LevelWidth) / 2); extra++) {
					for (int index3 = 0; index3 < LevelWidth - 1; index3++)
					{
						PossibleStartingTiles.Add(Grid2DArray[extra]->TileColumn[LevelWidth - 1 - index3]);
						Grid2DArray[extra]->TileColumn[LevelWidth - 1 - index3]->ShadeTestRoom();
					}
				}
			}
			if (startY < ((LevelHeight - 1) / 2)) { //more than after AFTER startX
				for (int extra = 0; extra < (LevelWidth - startY - 1) - ((LevelWidth) / 2); extra++) {
					for (int index4 = 0; index4 < LevelWidth - 1; index4++)
					{
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

			if (startX > (LevelWidth - 1) / 2) { //more than half AFTER startX
				for (int extra = 0; extra < (startX)-((LevelHeight) / 2); extra++) {
					for (int index3 = 0; index3 < LevelHeight - 1; index3++)
					{
						PossibleStartingTiles.Add(Grid2DArray[index3]->TileColumn[extra]);
						Grid2DArray[index3]->TileColumn[extra]->ShadeTestRoom();
					}
				}
			}
			if (startX < ((LevelWidth - 1) / 2)) { //more than after BEFORE startX
				for (int extra = 0; extra < (LevelHeight - startX - 1) - ((LevelHeight) / 2); extra++) {
					for (int index4 = 0; index4 < LevelHeight - 1; index4++)
					{
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
				
					for (int index3 = 0; index3 < LevelHeight - 1; index3++)
					{
						PossibleStartingTiles.Add(Grid2DArray[extra]->TileColumn[index3]);
						Grid2DArray[extra]->TileColumn[index3]->ShadeTestRoom();
					}
				}
			}
			if (startY < ((LevelHeight - 1) / 2)) { //more than after AFTER startX
				for (int extra = 0; extra < (LevelWidth - startY - 1) - ((LevelWidth) / 2); extra++) {
					for (int index4 = 0; index4 < LevelHeight - 1; index4++)
					{
						PossibleStartingTiles.Add(Grid2DArray[LevelWidth - 1 - extra]->TileColumn[index4]);
						Grid2DArray[LevelWidth - 1 - extra]->TileColumn[index4]->ShadeTestRoom();
					}
				}
			}

			break;
		default:
			if (DebugPrints)
				UE_LOG(LogTemp, Log, TEXT("Issue here -> picked weird starting side in ChooseStartEndRooms"));
			//DOWN	
			for (int index2 = 0; index2 < (LevelWidth - 1) / 2; index2++) {
				//take every tile less than startY
				for (int index = 0; index < startX; index++)
				{
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

			if (startX > (LevelWidth - 1) / 2) { //more than half AFTER startX
				for (int extra = 0; extra < (startX)-((LevelHeight) / 2); extra++) {
					for (int index3 = 0; index3 < LevelHeight - 1; index3++)
					{
						PossibleStartingTiles.Add(Grid2DArray[LevelHeight - 1 - index3]->TileColumn[extra]);
						Grid2DArray[LevelHeight - 1 - index3]->TileColumn[extra]->ShadeTestRoom();
					}
				}
			}
			if (startX < ((LevelWidth - 1) / 2)) { //more than after BEFORE startX
				for (int extra = 0; extra < (LevelHeight - startX - 1) - ((LevelHeight) / 2); extra++) {
					for (int index4 = 0; index4 < LevelHeight - 1; index4++)
					{
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
	CheckTile(StartingTile, LevelPath);

	if (DebugPrints) {
		//draw lines through path
		for (int Index = 0; Index < LevelPath.Num() - 1; Index++)
		{
			DrawDebugLine(GetWorld(), LevelPath[Index]->GetActorLocation(), LevelPath[Index + 1]->GetActorLocation(), FColor::Blue, SDPG_World, 20.0f, 150);
		}
	}

	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("=================== Finished Path - Adding Random Rooms =============================="));

	AddRandomRooms();
	

	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("=================== Finished Random Rooms - Adding Spawn Room =============================="));

	CreateSpawnRoom();


	if(MyLocalLevel->CurrentLevelTier >= ELevelTier::ELevel_2) {
		if (DebugPrints)
			UE_LOG(LogTemp, Log, TEXT("=================== Finished Spawn Room - Adding Secret Room =============================="));
		CreateSecretRoom();

	}


	if(DoorsActive){
		if (DebugPrints)
			UE_LOG(LogTemp, Log, TEXT("=================== Finished Secret Room - Activating All Doors =============================="));

	}


	//if (DebugPrints)
	//	UE_LOG(LogTemp, Log, TEXT("=================== Finished All Doors - Implementing Final Tile Setup =============================="));


	//if (DebugPrints)
	//	UE_LOG(LogTemp, Log, TEXT("=================== Finished Implementing Final Tile Setup - Beginning Populating Grid =============================="));
}

bool ASTileManager::AddTileToPath(ASTile* TileToAdd)
{
	PathNumber++;
	LevelPath.AddUnique(TileToAdd);
	TileToAdd->CheckForPath = true;
	TileToAdd->PathNumber = PathNumber;
	if(TileToAdd->TileStatus != ETileStatus::ETile_BOSSROOM && TileToAdd->TileStatus != ETileStatus::ETile_STARTINGROOM)
		TileToAdd->ShadePath();
		
	return true;
}

void ASTileManager::CheckTile(ASTile* CurrentTile, TArray<ASTile*> CurrentPath)
{
	bool CheckingTileDebug = false;

	// For Debug Check, for now will be off since this check is no longer critical
	if (CheckingTileDebug && CurrentTile) {
		UE_LOG(LogTemp, Log, TEXT("Currently on Tile: %d,%d"), CurrentTile->XIndex, CurrentTile->ZIndex);
	}
	else {
		UE_LOG(LogTemp, Log, TEXT("NULL TILE DETECTED. PLEASE INVESTIGATE"));
	}

	//first check if all neighbors are unavailable
	//if so, make this one as checked and call on previous tiles
	if ((!CurrentTile->UpNeighbor || CurrentTile->UpNeighbor->CheckForPath) && (!CurrentTile->DownNeighbor || CurrentTile->DownNeighbor->CheckForPath)
		&& (!CurrentTile->RightNeighbor || CurrentTile->RightNeighbor->CheckForPath) && (!CurrentTile->LeftNeighbor || CurrentTile->LeftNeighbor->CheckForPath))
	{
		CurrentTile->CheckForPath = true;
		CurrentTile->ShadeNull();

		//eventually all of these tiles will be unchecked so a different path may go through them (dead ends,etc)
		BackTrackHistory.Add(CurrentTile);

		CurrentPath.Remove(CurrentTile);
		CheckTile(CurrentTile->PreviousTile, CurrentPath);
	} // boss room checks
	else if ((CurrentTile->UpNeighbor != NULL && CurrentTile->UpNeighbor->TileStatus == ETileStatus::ETile_BOSSROOM))
	{
		CurrentTile->CheckForPath = true;
		AddTileToPath(CurrentTile);
		AddTileToPath(CurrentTile->UpNeighbor);

	}
	else if ((CurrentTile->DownNeighbor != NULL && CurrentTile->DownNeighbor->TileStatus == ETileStatus::ETile_BOSSROOM))
	{
		CurrentTile->CheckForPath = true;
		AddTileToPath(CurrentTile);
		AddTileToPath(CurrentTile->DownNeighbor);
	}
	else if ((CurrentTile->RightNeighbor != NULL && CurrentTile->RightNeighbor->TileStatus == ETileStatus::ETile_BOSSROOM))
	{
		CurrentTile->CheckForPath = true;
		AddTileToPath(CurrentTile);
		AddTileToPath(CurrentTile->RightNeighbor);
	}
	else if ((CurrentTile->LeftNeighbor != NULL && CurrentTile->LeftNeighbor->TileStatus == ETileStatus::ETile_BOSSROOM))
	{
		CurrentTile->CheckForPath = true;
		AddTileToPath(CurrentTile);
		AddTileToPath(CurrentTile->LeftNeighbor);
	}
	else {
		//now that we know theres valid neighbors and none of them are the boss room, lets check our neighbors

		//direction
		TArray <int> DirectionsToCheck = { 1, 2, 3, 4 };

		DirectionsToCheck = Reshuffle2(DirectionsToCheck);

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
					CurrentTile->RightNeighbor->PreviousTile = CurrentTile;
					AddTileToPath(CurrentTile);
					DirectionCount = 5;
					CheckTile(CurrentTile->RightNeighbor, CurrentPath);
				}
				break;
			}
		}
	}
}


/// <summary>
/// Dylan Log
/// History is mostly for debug, removes previous backtrack history so we can reuse same variable
/// </summary>
void ASTileManager::ClearHistory()
{
	for(int BIndex = 0; BIndex < BackTrackHistory.Num(); BIndex++)
	{
		BackTrackHistory[BIndex]->CheckForPath = false;
	}
}

/// <summary>
/// Dylan Loe
/// 
/// - Adding Single random and branches to grid
/// </summary>
void ASTileManager::AddRandomRooms()
{
	if (DebugPrints)
			UE_LOG(LogTemp, Log, TEXT("Adding Branches"));

	AllActiveTiles.Append(LevelPath);

	MakeAvailableTiles();

	//pick random index that isn't boss tile
	//pick a valid neighbor that isn't part of path or outside grid, thats not a boss tile
	//this tile is now the start of a branch

	//TO DO: how long will branch be?
	int BranchCount = GameStream.RandRange(1,(LevelWidth - LevelPath.Num() / LevelWidth) + 1);
	UE_LOG(LogTemp, Log, TEXT("Branch Length: %d"), BranchCount);

	for (int Branch = 0; Branch < BranchCount; Branch++)
	{
		//for now using length of level, might change this later, not sure how else but not a super important detail
		int BranchLength = GameStream.RandRange(1, LevelWidth);
		UE_LOG(LogTemp, Log, TEXT("Making Branch: %d with length %d"), Branch, BranchLength);

		ASTile* StartingBranchTile = AvailableTiles[GameStream.RandRange(0, AvailableTiles.Num() - 1)];
		TArray<ASTile*>	BranchArray;

		AvailableTiles.Remove(StartingTile);

		CheckBranchTile(StartingBranchTile, BranchArray, BranchLength);

		//TO DO: door logic goes here
		if (DoorsActive)
		{
			BranchArray[0]->ActivateDoorsBranch();
			BranchArray[0]->TileDescription += "_StartofBranch";
		}

		//run through branch
		for (int BranchIndex = 0; BranchIndex < BranchArray.Num(); BranchIndex++)
		{
			BranchArray[BranchIndex]->TileDescription += "Branch_" + FString::FromInt(Branch) + "";
			BranchArray[BranchIndex]->PathNumber = BranchIndex;

			if (!AllActiveTiles.Contains(BranchArray[BranchIndex]))
			{
				AllActiveTiles.AddUnique(BranchArray[BranchIndex]);
			}
			else if (AvailableTiles.Contains(BranchArray[BranchIndex]))
			{
				AvailableTiles.Remove(BranchArray[BranchIndex]);
			}

			if (BranchIndex == BranchArray.Num() - 1)
			{
				BranchArray[BranchIndex]->TileDescription += "ENDBRANCH";
				BranchArray[BranchIndex]->EndOfBranchPath = true;
			}
		}

		// MORE DOOR LOGIC HERE
		//ActiveDoorBranch stuff
		if (DoorsActive)
		{
			for (int Index = 1; Index < BranchArray.Num(); Index++)
			{
				if (BranchArray[Index]->TileStatus == ETileStatus::ETile_ROOM)
				{
					BranchArray[Index]->ActivateDoorsBranch();
				}
			}
		}

		//start of this branch will have the door connecting to previous tile, the rest of the path will not have a door connecting to anything part of the path
		//tiles on this branch can only connect to existing tiles in the list

		//once we make branch, we go back through and remake the available tile spots
		MakeAvailableTiles();

		//Debug draw branch
		if (DebugPrints) {
			//draw lines through path
			for (int Index = 0; Index < BranchArray.Num() - 1; Index++)
			{
				DrawDebugLine(GetWorld(), BranchArray[Index]->GetActorLocation(), BranchArray[Index + 1]->GetActorLocation(), FColor::Emerald, SDPG_World, 20.0f, 150);
			}
		}
	}

	AddSingleRooms();
}

/// <summary>
/// Dylan Loe
/// 
/// - Implementing single chosen rooms to available tiles
/// </summary>
void ASTileManager::AddSingleRooms()
{
	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Adding Single Rooms..."));
	//when we add a room, remove it from AvailableTiles, add to AllActiveTiles

	//default to half the rooms left over
	FillerRooms = GameStream.RandRange(1, (LevelWidth - AvailableTiles.Num() - 1) - ((LevelHeight - AvailableTiles.Num() - 1)/4));
	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Total Random Room: %d"), FillerRooms);

	for (int STileCount = 0; STileCount < FillerRooms; STileCount++)
	{
		if (LevelWidth - AllActiveTiles.Num() >= LevelHeight / (LevelWidth * 2))
		{
			ASTile* Current = AvailableTiles[GameStream.RandRange(0, AvailableTiles.Num() - 1)];
			UE_LOG(LogTemp, Log, TEXT("Room selected: %d:%d"), Current->XIndex, Current->ZIndex);
			if (Current->TileStatus == ETileStatus::ETile_NULLROOM)
			{
				Current->ShadeActiveRoom();
				AvailableTiles.Remove(Current);
				AllActiveTiles.AddUnique(Current);
				Current->TileDescription = "Random Single Room";

				//Activate Doors
				//Remake new available list (with this currents neighbors now added
				//TO DO - Size small: Optimize the remake so that we only add the new tiles rooms instead of having to go through entire list

				if (DoorsActive)
				{
					Current->ActivateDoorsRandom();
				}
				MakeAvailableTiles();
			}
		} 
		else
		{
			//stop adding random tiles
			return;
		}
	}
}

/// <summary>
/// Dylan Loe
/// 
/// - Add spawn room, connected to  the start room (this will be outside of the grid
/// </summary>
void ASTileManager::CreateSpawnRoom()
{
	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Creating Spawn Room..."));

	FVector SpawnPos;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	//There are going to be 2 tiles basically spawned, one is the base, the base structure of the tile
	// The other tile (which i don't think needs to be a tile at all), is the environment to be populated on the tile
	// Since the environment will be rotated, its neighbors will be changed theretofore for now it will be faster to have its own base that stays static)
	switch (StartRoomSide)
	{
		case 0:
			//RIGHT
			SpawnPos = FVector(StartingTile->GetActorLocation().X - (StartingTile->TileLength), StartingTile->GetActorLocation().Y, StartingTile->GetActorLocation().Z );
			PlayerStartingTileBase = GetWorld()->SpawnActor<ASTile>(TileBase, SpawnPos, StartingTile->GetActorRotation(), SpawnParams);
			PlayerSpawnPresentTile = GetWorld()->SpawnActor<ASTile>(MyLocalLevel->PresetStartingTile, SpawnPos, StartingTile->GetActorRotation(), SpawnParams);
			StartingTile->LeftNeighbor = PlayerStartingTileBase;
			PlayerStartingTileBase->RightNeighbor = StartingTile;
			PlayerSpawnPresentTile->SetActorRotation(FRotator(PlayerSpawnPresentTile->GetActorRotation().Euler().X, -90.0f, PlayerSpawnPresentTile->GetActorRotation().Euler().Z));
			break;
		case 1:
			//UP
			SpawnPos = FVector(StartingTile->GetActorLocation().X, StartingTile->GetActorLocation().Y - (StartingTile->TileLength), StartingTile->GetActorLocation().Z);
			PlayerStartingTileBase = GetWorld()->SpawnActor<ASTile>(TileBase, SpawnPos, StartingTile->GetActorRotation(), SpawnParams);
			PlayerSpawnPresentTile = GetWorld()->SpawnActor<ASTile>(MyLocalLevel->PresetStartingTile, SpawnPos, StartingTile->GetActorRotation(), SpawnParams);
			StartingTile->UpNeighbor = PlayerStartingTileBase;
			PlayerStartingTileBase->DownNeighbor = StartingTile;
			break;
		case 2:
			//LEFT
			SpawnPos = FVector(StartingTile->GetActorLocation().X + (StartingTile->TileLength), StartingTile->GetActorLocation().Y, StartingTile->GetActorLocation().Z );
			PlayerStartingTileBase = GetWorld()->SpawnActor<ASTile>(TileBase, SpawnPos, StartingTile->GetActorRotation(), SpawnParams);
			PlayerSpawnPresentTile = GetWorld()->SpawnActor<ASTile>(MyLocalLevel->PresetStartingTile, SpawnPos, StartingTile->GetActorRotation(), SpawnParams);
			StartingTile->RightNeighbor = PlayerStartingTileBase;
			PlayerStartingTileBase->LeftNeighbor = StartingTile;
			PlayerSpawnPresentTile->SetActorRotation(FRotator(PlayerSpawnPresentTile->GetActorRotation().Euler().X, 90, PlayerSpawnPresentTile->GetActorRotation().Euler().Z));
			break;
		case 3:
			//DOWN
			SpawnPos = FVector(StartingTile->GetActorLocation().X, StartingTile->GetActorLocation().Y + (StartingTile->TileLength), StartingTile->GetActorLocation().Z);
			PlayerStartingTileBase = GetWorld()->SpawnActor<ASTile>(TileBase, SpawnPos, StartingTile->GetActorRotation(), SpawnParams);
			PlayerSpawnPresentTile = GetWorld()->SpawnActor<ASTile>(MyLocalLevel->PresetStartingTile, SpawnPos, StartingTile->GetActorRotation(), SpawnParams);
			StartingTile->DownNeighbor = PlayerStartingTileBase;
			PlayerStartingTileBase->UpNeighbor = StartingTile;
			PlayerSpawnPresentTile->SetActorRotation(FRotator(PlayerSpawnPresentTile->GetActorRotation().Euler().X, 180.0f, PlayerSpawnPresentTile->GetActorRotation().Euler().Z));

			break;
	}

	PlayerStartingTileBase->SetActorLabel("StartingTile_Base");
	PlayerSpawnPresentTile->SetActorLabel("StartingTile_Populate");
#if WITH_EDITOR
	PlayerStartingTileBase->SetFolderPath(TileGenRootFolder);
	PlayerSpawnPresentTile->SetFolderPath(TileGenRootFolder);
#endif
	//Set the Preset ref to the SpawnPresetTile obj
	PlayerStartingTileBase->PresetTile = PlayerSpawnPresentTile;

	//Spawn stats from StartingTile to PlayerStartingTileBase, then we will reassign the StartingTile
	StartingTile->ShadePath();
	LevelPath.Insert(PlayerStartingTileBase, 0);
	AllActiveTiles.Insert(PlayerStartingTileBase, 0);
	PlayerStartingTileBase->ShadeStartingRoom();
	StartingTile = PlayerStartingTileBase;
}

/// <summary>
/// Dylan Log
/// 
/// - First find possible tiles to place:
///		- Compose list of all null neighbors of all active tiles (excluding start and boss rooms)
///		- outskirts of game map (any tile next to an edge)
/// - similar to start room, assign to one of these possible tiles
/// </summary>
void ASTileManager::CreateSecretRoom()
{
	TArray<ASTile*> outskirtsCheck;
	for (int tileCount = 0; tileCount < AllActiveTiles.Num(); tileCount++)
	{
		ASTile* currentTile = AllActiveTiles[tileCount];
		
		FTileInfoStruct currentInfo;
		currentInfo.tile = currentTile;

		if (currentTile->TileStatus != ETileStatus::ETile_BOSSROOM && currentTile->TileStatus != ETileStatus::ETile_STARTINGROOM)
		{
			//if up neighbor is a null ref
			if (!currentTile->UpNeighbor)
			{
				currentInfo.n.Add(1);
			}
			else if (currentTile->UpNeighbor && currentTile->UpNeighbor->TileStatus == ETileStatus::ETile_NULLROOM &&
				currentTile->UpNeighbor->TileStatus != ETileStatus::ETile_BOSSROOM && currentTile->UpNeighbor->TileStatus != ETileStatus::ETile_STARTINGROOM)
			{
				currentInfo.n.Add(1);
			}

			//check left neighbor
			if (!currentTile->LeftNeighbor)
			{
				currentInfo.n.Add(3);
			}
			else if (currentTile->LeftNeighbor && currentTile->LeftNeighbor->TileStatus == ETileStatus::ETile_NULLROOM &&
				currentTile->LeftNeighbor->TileStatus != ETileStatus::ETile_BOSSROOM && currentTile->LeftNeighbor->TileStatus != ETileStatus::ETile_STARTINGROOM)
			{
				currentInfo.n.Add(3);
			}

			if (!currentTile->RightNeighbor)
			{
				currentInfo.n.Add(4);
			}
			else if (currentTile->RightNeighbor && currentTile->RightNeighbor->TileStatus == ETileStatus::ETile_NULLROOM &&
				currentTile->RightNeighbor->TileStatus != ETileStatus::ETile_BOSSROOM && currentTile->RightNeighbor->TileStatus != ETileStatus::ETile_STARTINGROOM)
			{
				currentInfo.n.Add(4);
			}

			if (!currentTile->DownNeighbor)
			{
				currentInfo.n.Add(2);
			}
			else if (currentTile->DownNeighbor && currentTile->DownNeighbor->TileStatus == ETileStatus::ETile_NULLROOM &&
				currentTile->DownNeighbor->TileStatus != ETileStatus::ETile_BOSSROOM && currentTile->DownNeighbor->TileStatus != ETileStatus::ETile_STARTINGROOM)
			{
				currentInfo.n.Add(2);
			}

			//add our info struct to list
			if (currentInfo.n.Num() != 0 && !outskirtsCheck.Contains(currentInfo.tile)) //OutskirtTiles.Contains(currentInfo))
			{
				OutskirtTiles.Add(currentInfo);
				outskirtsCheck.Add(currentInfo.tile);
			}
		}
	}

	//now randomly pick a tile to put our secret room at (this tiles neighbor will be the secret room)
	int tileNum = GameStream.RandRange(0, OutskirtTiles.Num() - 1);

	FTileInfoStruct selected = OutskirtTiles[tileNum];
	ASTile* test = outskirtsCheck[tileNum];
	choosen = outskirtsCheck[tileNum];

	//reshuffle our n value
	selected.n = Reshuffle2(selected.n);

	int loc = GameStream.RandRange(0, selected.n.Num() - 1);

	//we now have our room we selected and the neighbor in which we are using for our secret room
	FVector SpawnPos;
	FRotator SpawnRot;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	//TO DO: weird but with center of tile being at the top, causing a 240 offset. Will need to investigate later
	switch (selected.n[loc])
	{
		case 1:
			//up
			SpawnPos = FVector(selected.tile->GetActorLocation().X, selected.tile->GetActorLocation().Y + (StartingTile->TileLength), selected.tile->GetActorLocation().Z + 240);
			SecretRoom = PlayerSpawnPresentTile = GetWorld()->SpawnActor<ASTile>(MyLocalLevel->PresetSecretRoomTile, SpawnPos, SpawnRot, SpawnParams);
			SecretRoom->DownNeighbor = selected.tile;
			selected.tile->UpNeighbor = SecretRoom;
			break;
		case 2:
			//down
			SpawnPos = FVector(selected.tile->GetActorLocation().X, selected.tile->GetActorLocation().Y - (StartingTile->TileLength), selected.tile->GetActorLocation().Z + 240);
			SpawnRot = FRotator(PlayerSpawnPresentTile->GetActorRotation().Euler().X, 180.0f, PlayerSpawnPresentTile->GetActorRotation().Euler().Z);
			SecretRoom = PlayerSpawnPresentTile = GetWorld()->SpawnActor<ASTile>(MyLocalLevel->PresetSecretRoomTile, SpawnPos, SpawnRot, SpawnParams);
			SecretRoom->UpNeighbor = selected.tile;
			selected.tile->DownNeighbor = SecretRoom;
			break;
		case 3:
			//left
			SpawnPos = FVector(selected.tile->GetActorLocation().X - (selected.tile->TileLength), selected.tile->GetActorLocation().Y, selected.tile->GetActorLocation().Z + 240);
			SpawnRot = FRotator(PlayerSpawnPresentTile->GetActorRotation().Euler().X, 90.0f, PlayerSpawnPresentTile->GetActorRotation().Euler().Z);
			SecretRoom = PlayerSpawnPresentTile = GetWorld()->SpawnActor<ASTile>(MyLocalLevel->PresetSecretRoomTile, SpawnPos, SpawnRot, SpawnParams);
			SecretRoom->RightNeighbor = selected.tile;
			selected.tile->LeftNeighbor = SecretRoom;
			break;
		case 4:
			//right
			SpawnPos = FVector(selected.tile->GetActorLocation().X + (selected.tile->TileLength), selected.tile->GetActorLocation().Y, selected.tile->GetActorLocation().Z + 240);
			SpawnRot = FRotator(PlayerSpawnPresentTile->GetActorRotation().Euler().X, -90.0f, PlayerSpawnPresentTile->GetActorRotation().Euler().Z);
			SecretRoom = PlayerSpawnPresentTile = GetWorld()->SpawnActor<ASTile>(MyLocalLevel->PresetSecretRoomTile, SpawnPos, SpawnRot, SpawnParams);
			SecretRoom->RightNeighbor = selected.tile;
			selected.tile->LeftNeighbor = SecretRoom;
			break;
	}
	// TO DO: this will need to be updated to a specific Secrete Room BP set in LocalLevel
	
	SecretRoom->SetActorLabel("SecretRoom");
#if WITH_EDITOR
	SecretRoom->SetFolderPath(TileGenRootFolder);
	SecretRoom->ShadeSecretRoom();
#endif

	//ACTIVATE WALLS
	SecretRoom->ActivateWalls();

}

/// <summary>
/// Dylan Loe
/// 
/// - Runs through branch recursively to make random branches out of main path
/// </summary>
/// <param name="TileToAdd"> Current Tile being analyzed </param>
/// <param name="CurrentPath"> Current branch path we are building </param>
/// <param name="Length"> given size of path we are aiming for </param>
void ASTileManager::CheckBranchTile(ASTile* TileToAdd, TArray<ASTile*> CurrentPath, int Length)
{
	if (Length > 0)
	{
		CurrentPath.Add(TileToAdd);
		TileToAdd->ShadeActiveRoom();
		TileToAdd->TileDescription = "";

		if ((!TileToAdd->RightNeighbor || TileToAdd->RightNeighbor->TileStatus != ETileStatus::ETile_NULLROOM) && (!TileToAdd->LeftNeighbor || TileToAdd->LeftNeighbor->TileStatus != ETileStatus::ETile_NULLROOM)
		&& (!TileToAdd->UpNeighbor || TileToAdd->UpNeighbor->TileStatus != ETileStatus::ETile_NULLROOM) && (!TileToAdd->DownNeighbor || TileToAdd->DownNeighbor->TileStatus != ETileStatus::ETile_NULLROOM))
		{
			//theres no where to go, lets just end the branch here to save time
			Length = 0;
			//exit branch
			return;
		}

		TArray <int> DirectionsToCheck = { 1, 2, 3, 4 };

		DirectionsToCheck = Reshuffle2(DirectionsToCheck);

		//pick direction and begin CheckTile
		for (int DirectionCount = 0; DirectionCount < DirectionsToCheck.Num(); DirectionCount++)
		{
			switch (DirectionsToCheck[DirectionCount])
			{
			case 1:
				//UP
				if (TileToAdd->UpNeighbor && !TileToAdd->UpNeighbor->CheckForPath && TileToAdd->UpNeighbor->TileStatus != ETileStatus::ETile_STARTINGROOM &&
					TileToAdd->UpNeighbor->TileStatus == ETileStatus::ETile_NULLROOM) {

					TileToAdd->UpNeighbor->PreviousTile = TileToAdd;
					Length--;
					CheckBranchTile(TileToAdd->UpNeighbor, CurrentPath, Length);
					return;
				}
				break;
			case 2:
				//DOWN
				if (TileToAdd->DownNeighbor && !TileToAdd->DownNeighbor->CheckForPath && TileToAdd->DownNeighbor->TileStatus != ETileStatus::ETile_STARTINGROOM &&
					TileToAdd->DownNeighbor->TileStatus == ETileStatus::ETile_NULLROOM) {

					TileToAdd->DownNeighbor->PreviousTile = TileToAdd;
					Length--;
					CheckBranchTile(TileToAdd->DownNeighbor, CurrentPath, Length);
					return;
				}
				break;
			case 3:
				//LEFT
				if (TileToAdd->LeftNeighbor && !TileToAdd->LeftNeighbor->CheckForPath && TileToAdd->LeftNeighbor->TileStatus != ETileStatus::ETile_STARTINGROOM &&
					TileToAdd->LeftNeighbor->TileStatus == ETileStatus::ETile_NULLROOM) {

					TileToAdd->LeftNeighbor->PreviousTile = TileToAdd;
					Length--;
					CheckBranchTile(TileToAdd->LeftNeighbor, CurrentPath, Length);
					return;
				}
				break;
			case 4:
				//RIGHT
				if (TileToAdd->RightNeighbor && !TileToAdd->RightNeighbor->CheckForPath && TileToAdd->RightNeighbor->TileStatus != ETileStatus::ETile_STARTINGROOM &&
					TileToAdd->RightNeighbor->TileStatus == ETileStatus::ETile_NULLROOM) {

					TileToAdd->RightNeighbor->PreviousTile = TileToAdd;
					Length--;
					CheckBranchTile(TileToAdd->RightNeighbor, CurrentPath, Length);
					return;
				}
				break;
			}
		}
	}
	return;
}

/// <summary>
/// Dylan Log
/// 
/// - Remakes our possible available tiles for path generation purposes
/// </summary>
void ASTileManager::MakeAvailableTiles()
{
	for (int TileC = 0; TileC < AllActiveTiles.Num() - 1; TileC++)
	{
		ASTile* CurrentTile = AllActiveTiles[TileC];

		//check each neighbor
		if (CurrentTile->TileStatus != ETileStatus::ETile_BOSSROOM)
		{
			if (CurrentTile->UpNeighbor && CurrentTile->UpNeighbor->TileStatus == ETileStatus::ETile_NULLROOM && !AvailableTiles.Contains(CurrentTile->UpNeighbor))
			{
				AvailableTiles.Add(CurrentTile->UpNeighbor);
			}
			if (CurrentTile->DownNeighbor && CurrentTile->DownNeighbor->TileStatus == ETileStatus::ETile_NULLROOM && !AvailableTiles.Contains(CurrentTile->DownNeighbor))
			{
				AvailableTiles.Add(CurrentTile->DownNeighbor);
			}
			if (CurrentTile->RightNeighbor && CurrentTile->RightNeighbor->TileStatus == ETileStatus::ETile_NULLROOM && !AvailableTiles.Contains(CurrentTile->RightNeighbor))
			{
				AvailableTiles.Add(CurrentTile->RightNeighbor);
			}
			if (CurrentTile->LeftNeighbor && CurrentTile->LeftNeighbor->TileStatus == ETileStatus::ETile_NULLROOM && !AvailableTiles.Contains(CurrentTile->LeftNeighbor))
			{
				AvailableTiles.Add(CurrentTile->LeftNeighbor);
			}
		}
	}
}

/// <summary>
/// Dylan Loe
/// 
/// - Activating all doors, on main path
/// </summary>
void ASTileManager::ActivateAllDoors()
{
	for(int count = 0; count < LevelPath.Num(); count++)
	{
		LevelPath[count]->ActivateDoorToPath();
	}
	DeactiveInactiveRooms();

	for(int count2 = 0; count2 < AllActiveTiles.Num(); count2++)
	{
		AllActiveTiles[count2]->SyncDoors();
	}
}

/// <summary>
/// Dylan Log
/// 
/// - Will run through once Secret room and Door configuration is implemented to remove NULL rooms from Tile Map
/// </summary>
void ASTileManager::DeactiveInactiveRooms()
{
	
}

/// <summary>
/// Knuth shuffle algorithm
/// 
/// - Reshuffling arrays
/// </summary>
/// <param name="ar"> Array input to be reshuffles</param>
/// <returns></returns>
TArray <int> ASTileManager::Reshuffle2(TArray <int> ar)
{
	// Knuth shuffle algorithm :: courtesy of Wikipedia :)
	for (int t = 0; t < ar.Num(); t++)
	{
		int r = GameStream.RandRange(t, ar.Num() - 1);
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
/// <param name="ThisTile"> Current Tile being Linked</param>
/// <param name="Col"></param>
void ASTileManager::LinkTile(ASTile* ThisTile, FMultiTileStruct Col)
{
	//for now I'm going to make right direction the positive one and left is negative
	if (ThisTile->ZIndex > 0)
	{
		ThisTile->DownNeighbor = Col.TileColumn[ThisTile->ZIndex - 1];
		ASTile* DownNeighbor = ThisTile->DownNeighbor;
		Col.TileColumn[ThisTile->ZIndex - 1]->UpNeighbor = ThisTile;

		//if doors are active, spawn a door at the tile placeholder
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		//if our lower neighbor doesn't already have a door connecting up
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

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		//if our lower neighbor doesn't already have a door connecting up
		if (!LeftNeighbor->RightDoor)
		{
			FString TileRightDoorName = "TileDoorConnecting_" + FString::FromInt(ThisTile->XIndex) + "_" + FString::FromInt(ThisTile->ZIndex) + "_to_" + FString::FromInt(LeftNeighbor->XIndex) + "_" + FString::FromInt(LeftNeighbor->ZIndex);
			
			//add LeftDoorSpawnPoint location to ThisTiles location to give world location
			FVector LeftDoorSpawnLocation = ThisTile->LeftDoorSpawnPoint.GetLocation() + ThisTile->GetActorLocation();
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

