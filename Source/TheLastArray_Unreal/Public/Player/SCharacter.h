#pragma once

#include "CoreMinimal.h"
#include "Attributes/SAttributeComponent.h"
#include "Interactables/SInteractionActorComponent.h"
#include "GameFramework/Character.h"
#include <Actions/SActionComponent.h>
#include "Particles/ParticleSystemComponent.h"
#include "SPlayerState.h"
#include "Actions/SActionEffect.h"
#include "SCharacter.generated.h"


class UCameraComponent;
class USpringArmComponent;
class USInteractionComponent;
class UAnimMontage;
class USActionComponent;

UCLASS()
class THELASTARRAY_UNREAL_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:

#pragma region Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UParticleSystemComponent* EffectSpellCastComp;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	//interaction
	UPROPERTY(VisibleAnywhere)
	USInteractionActorComponent* InteractiveComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "Components")
	USActionComponent* ActionComp;

#pragma endregion

	//TO DO
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "Player")
	ASPlayerState* PState;

	UPROPERTY(EditDefaultsOnly)
	int HitFlashSpeed;

	UPROPERTY(EditDefaultsOnly)
	int BlackHoleRageCost;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TimeToHitParameterName;

	UPROPERTY(EditDefaultsOnly)
	FColor HitFlashColor;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<USActionEffect> ThornActionClass;

	//actions
	void SprintStart();
	void SprintStop();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float value);
	void MoveRight(float value);

	void BlackholeAttack();

	void TeleportAbility();

	void PrimaryAttack();

	void PrimaryInteract();

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

	//slightly better way to bind your events in timeline
	virtual void PostInitializeComponents() override;

	virtual FVector GetPawnViewLocation() const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(Exec)
	void HealSelf(float Amount = 100);

};
