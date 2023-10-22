// Fill out your copyright notice in the Description page of Project Settings.


#include "STile.h"

// Sets default values
ASTile::ASTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DoorsRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TilesRoot"));
	RootComponent = DoorsRoot;
	//DoorsRoot->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASTile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASTile::SetUpDoorTransforms_Implementation()
{

}



#pragma region Door Setup
void ASTile::ActivateDoorToPath()
{

}

void ASTile::ActivateDoorsBranch()
{

}

void ASTile::ActivateDoorsRandom()
{
	
}
#pragma endregion

#pragma region Labeling Rooms

void ASTile::ShadeNull()
{
	DrawDebugSphere(GetWorld(), this->GetActorLocation(), 200.0f, 20, FColor::Red, false, 100);
	TileStatus = ETileStatus::ETile_NULLROOM;
}

void ASTile::ShadePath()
{
	DrawDebugSphere(GetWorld(), this->GetActorLocation(), 200.0f, 20, FColor::Blue, false, 100);
	
	TileStatus = ETileStatus::ETile_PATH;
}

void ASTile::ShadeActiveRoom()
{
	DrawDebugSphere(GetWorld(), this->GetActorLocation(), 200.0f, 20, FColor::Silver, false, 100);
	TileStatus = ETileStatus::ETile_ROOM;
}

void ASTile::ShadeBossRoom()
{
	DrawDebugSphere(GetWorld(), this->GetActorLocation(), 200.0f, 20, FColor::Emerald, false, 100);
	TileStatus = ETileStatus::ETile_BOSSROOM;
}

void ASTile::ShadeStartingRoom()
{
	DrawDebugSphere(GetWorld(), this->GetActorLocation(), 200.0f, 20, FColor::Green, false, 100);
	TileStatus = ETileStatus::ETile_STARTINGROOM;
}

void ASTile::ShadeSecretRoom()
{
	DrawDebugSphere(GetWorld(), this->GetActorLocation(), 200.0f, 20, FColor::Purple, false, 100);
	TileStatus = ETileStatus::ETile_SECRETROOM;
}

void ASTile::ShadeTestRoom()
{
	//DrawDebugSphere(GetWorld(), this->GetActorLocation(), 200.0f, 20, FColor::Magenta, false, 100);
	//TileStatus = ETileStatus::ETile_ROOM;
}

void ASTile::ShadeEndRoom()
{
	DrawDebugSphere(GetWorld(), this->GetActorLocation(), 200.0f, 20, FColor::Black, false, 100);
	TileStatus = ETileStatus::ETile_BOSSROOM;
}

#pragma endregion

