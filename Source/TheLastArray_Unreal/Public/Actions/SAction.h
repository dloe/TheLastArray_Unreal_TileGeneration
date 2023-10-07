// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "SActionComponent.h"
#include "SAction.generated.h"

class UWorld;

USTRUCT()
struct FActionRepData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	bool bIsRunning;

	//NotReplicated could use but dont
	UPROPERTY()
	AActor* Instigator;
};

UCLASS(Blueprintable)
class THELASTARRAY_UNREAL_API USAction : public UObject
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(Replicated)
	USActionComponent* ActionComp;

	UFUNCTION(BlueprintCallable, Category = "Action")
	USActionComponent* GetOwningComponent() const;

	//tags added to owing actor when activated, removed when action stops
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer GrantsTags;

	//action can only start if owningactor has none of these tags applied
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer BlockedTags;

	//check to make sure if the action is actually running (to avoid things like stopping something that never ran in the first place)
	UPROPERTY(ReplicatedUsing = "OnRep_RepData")
	FActionRepData RepData;
	//bool bIsRunning;

	UPROPERTY(Replicated)
	float TimeStarted;

	UFUNCTION()
	void OnRep_RepData();

public:

	void Initialize(USActionComponent* NewActionComponent);

	UPROPERTY(EditDefaultsOnly, Category = "Action")
	bool bAutoStart;
	
	//Action nickname to start/stop without a reference to the object
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	FName ActionName;

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool IsRunning() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	bool CanStart(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void StopAction(AActor* Instigator);
		
	UWorld* GetWorld() const override;

	//no need for it in cpp will update this in header file
	bool IsSupportedForNetworking() const override
	{
		return true;
	}
};
