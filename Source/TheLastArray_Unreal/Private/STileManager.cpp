#include "STileManager.h"
#include <Engine/World.h>

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
	UE_LOG(LogTemp, Log, TEXT("========Begin Play================"));
	Create2DTileArray();

}

void ASTileManager::Create2DTileArray()
{
	UE_LOG(LogTemp, Log, TEXT("===================Creating 2D array!=============================="));
	for (int32 Index = 0; Index < Width; Index++)
	{
		UE_LOG(LogTemp, Log, TEXT("Column Number: %f"), Index);
		//for each row, make each column
		FMultiTileStruct* Col = new FMultiTileStruct();

		//float length = TileBase->TileLength;

		const FTransform SpawnTM = FTransform(this->GetActorRotation(), this->GetActorLocation());
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		//SpawnParams.Instigator = this;

		//Populate TileColumn array with Tiles for Height size
		for (int32 Index2 = 0; Index2 < Height; Index2++)
		{
			//spawn in a Tile
			FString TileName = "Tile_Row" + FString::FromInt(Index2) + "_Col" + FString::FromInt(Index);
			ASTile* T = GetWorld()->SpawnActor<ASTile>(TileBase, this->GetActorLocation(), this->GetActorRotation(), SpawnParams);
			T->SetActorLabel(TileName);
			FVector NewLocal = FVector((this->GetActorLocation().X + (100 * Index)), (this->GetActorLocation().Y + (100 * Index2)), this->GetActorLocation().Z);
			T->SetActorLocation(NewLocal);
			//add tile to array
			Col->TileColumn.Add(T);
		}


		MyArray.Add(Col);
	}

}

// Called every frame
void ASTileManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

