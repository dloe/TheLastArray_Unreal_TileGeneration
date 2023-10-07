// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Actions/SActionEffect.h"
#include "SAttributeComponent.generated.h"

class USAttributeComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewHealth, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRageChanged, USAttributeComponent*, OwningComp, float, NewRage, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnCreditChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewCredits, float, Delta);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THELASTARRAY_UNREAL_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USAttributeComponent();

	//static functions

	//assess our attributecomp much easier
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	static USAttributeComponent* GetAttributes(AActor* FromActor);

	UFUNCTION(BlueprintCallable, Category = "Attributes", meta = (DisplayName = "IsAlive"))
	static bool IsActorAlive(AActor* Actor);



protected:

	

	//health attribute
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
	float Health;

	//other examples
	//HealthMax, stamina, strength
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = "Attributes")
	float HealthMax = 100;

	//UFUNCTION(ReplicatedUsing="")
	//bool bIsAlive;

	UFUNCTION(NetMulticast, Reliable) //@FIXME: mark as unreliable once we move the 'state' out of sCharacter
	void MulticastHealthChanged(AActor* InstigatorActor, float NewHealth, float Delta);

	UFUNCTION(NetMulticast, Unreliable) //@FIXME: mark as unreliable once we move the 'state' out of sCharacter
	void MulticastRageChanged(AActor* InstigatorActor, float NewRage, float Delta);

	//Rage attribute
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Attributes")
	float Rage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = "Attributes")
	float RageMax = 100;

public:


	
	UFUNCTION(BlueprintCallable)
	bool Kill(AActor* InstigatorActor);

	UFUNCTION(BlueprintCallable)
	bool IsAlive() const;

	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnCreditChanged OnCreditChanged;

	UPROPERTY(BlueprintAssignable)
	FOnRageChanged OnRageChanged;

	//return if the change succeeded
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyHealthChange(AActor* Instigator, float Delta);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void ApplyRageChange(AActor* Instigator, float Delta);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool SpendRage(AActor* Instigator, float CostDelta);

	UFUNCTION(BlueprintCallable)
	bool IsUnderMaxHealth() const;
		
	float GetHealthMax() const;
};
