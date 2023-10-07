// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreditsChanged, ASPlayerState*, PlayerState, int32, NewCredits, int32, Delta);

UCLASS()
class THELASTARRAY_UNREAL_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = "OnRep_Credits", Category = "Events")
		int32 Credits;

	UFUNCTION()
		void OnRep_Credits(int32 OldCredits);
public:
	ASPlayerState();

	//might put get credits function

	//might add a spend/add credits function
	//returns total credits from playerstate
	UFUNCTION(BlueprintCallable, Category = "Events")
	int32 GetCredits();

	//removes value and returns true if succeeds
	UFUNCTION(BlueprintCallable, Category = "Events")
	bool RemoveCredits(int32 delta);

	UFUNCTION(BlueprintCallable, Category = "Events")
	//add credit value to total credits
	int32 AddCredits(int32 delta);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCreditsChanged OnCreditsChanged;

	UFUNCTION(BlueprintNativeEvent)
	void SavePlayerState(USSaveGame* SaveObject);

	UFUNCTION(BlueprintNativeEvent)
	void LoadPlayerState(USSaveGame* SaveObject);
};
