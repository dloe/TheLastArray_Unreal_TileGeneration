// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAction.h"
#include "SAction_ProjectileAttack.generated.h"

class UAnimMontage;
class UParticleSystem;

/**
 * 
 */
UCLASS()
class THELASTARRAY_UNREAL_API USAction_ProjectileAttack : public USAction
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Attack")
		TSubclassOf<AActor> ProjectileClass;
	UPROPERTY(VisibleAnywhere, Category = "Effects")
		FName HandSocketName;
	UPROPERTY(VisibleAnywhere, Category = "Attack")
		float AttacAnimDelay;
	UPROPERTY(EditAnywhere, Category = "Attack")
		UAnimMontage* AttackAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
		UParticleSystem* CastingEffects;

	UFUNCTION()
	void AttackDelay_Elasped(ACharacter* InstigatorCharacter);

public:
	//UFUNCTION(BlueprintNativeEvent, Category = "Action")
	virtual void StartAction_Implementation(AActor* Instigator) override;

	USAction_ProjectileAttack();


};
