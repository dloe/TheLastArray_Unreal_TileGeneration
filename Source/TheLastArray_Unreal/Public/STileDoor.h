// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STileDoor.generated.h"

UCLASS()
class THELASTARRAY_UNREAL_API ASTileDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASTileDoor();

	UPROPERTY(EditAnywhere, Category = "Door Info")
	bool DoorActive = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
