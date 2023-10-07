// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Projectiles/SProjectileBase.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TeleportProjectile.generated.h"


UCLASS()
class THELASTARRAY_UNREAL_API ATeleportProjectile : public ASProjectileBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATeleportProjectile();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Teleport")
	float TeleportDelay;

	UPROPERTY(EditDefaultsOnly, Category = "Teleport")
	float DetonateDelay;

	UPROPERTY()
	FTimerHandle TeleportTimerHandle;

public:	
	void OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void Explode_Implementation() override;

	void TeleportSequence();

	virtual void BeginPlay() override;

};
