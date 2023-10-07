// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAttributeComponent.h"
#include "GameFramework/Character.h"
#include <SActionComponent.h>
#include "SAICharacter.generated.h"

class UPawnSensingComponent;
class UUserWidget;
class USWorldUserWidget;
class USActionComponent;

UCLASS()
class THELASTARRAY_UNREAL_API ASAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASAICharacter();

	UPROPERTY(EditAnywhere, Category = "Loot")
		//TSubclassOf<AActor> CoinPickupClass;
		int32 CreditsOnKill;
	

protected:
	
	USWorldUserWidget* ActiveHealthBar;
	USWorldUserWidget* EnemySpottedWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> EnemySpottedWidgetClass;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TimeToHitParameterName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USActionComponent* ActionComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	float DeathReward;

	void SetTargetActor(AActor* NewTarget);

	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable, Category = "AI")
		AActor* GetTargetActor() const;

	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPawnSeen();

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);
};
