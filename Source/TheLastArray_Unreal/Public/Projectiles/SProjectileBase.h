// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include <Runtime/Engine/Classes/PhysicsEngine/RadialForceComponent.h>
#include "SProjectileBase.generated.h"


class USphereComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;

//marks this class as incomplete so as to not include it on certain drop downs
UCLASS(ABSTRACT)
class THELASTARRAY_UNREAL_API ASProjectileBase : public AActor
{
	GENERATED_BODY()
	
protected:

	//sound cue
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundCue* ImpactSound;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* ImpactVFX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* MoveComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UParticleSystemComponent* EffectComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAudioComponent* AudioCompFlight;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	float ImpactShakeInnerRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	float ImpactShakeOuterRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	float Falloff;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		URadialForceComponent* myRadialForce;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
		TSubclassOf<UCameraShakeBase> MyShake;

	UFUNCTION()
	virtual void OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	//BlueprintNativeEvents = C++ base implementation, can be expanded in blueprints
	//blueprintcallable to allow child classes to trigger explosions
	//Not required for assignment but useful for expanding in blueprint later on
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Explode();

	virtual void PostInitializeComponents() override;

public:	
	// Sets default values for this actor's properties
	ASProjectileBase();
};
