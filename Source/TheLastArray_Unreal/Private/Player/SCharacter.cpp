// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interactables/SInteractionActorComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include <TheLastArray_Unreal/Public/Attributes/SAttributeComponent.h>
#include "Actions/SActionComponent.h"
#include "Player/SPlayerState.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	EffectSpellCastComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectSpellCastComp->SetupAttachment(RootComponent);

	InteractiveComp = CreateDefaultSubobject<USInteractionActorComponent>("InteractionComp");

	AttributeComponent = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");

	ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");

	//rotate to whatever we are moving towards
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	
	BlackHoleRageCost = 25;
	bUseControllerRotationYaw = false;

	TimeToHitParameterName = "TimeToHit";
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//Add dynamic is known for not being found through unreal magic (AKA INTELLISENSE), its ok that it doesnt see it here
	AttributeComponent->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);

	
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//APlayerController* PC = Cast<APlayerController>(GetController());
	//PState = Cast<ASPlayerState>(PC->PlayerState);
	//debug print out our credit amount
	//int Credits = PState->GetCredits();
	//UE_LOG(LogTemp, Log, TEXT("Player Credits on Start: %d"), PState->GetCredits());

	//test thorns 
	//if (ensure(ActionComp))
	//{
		//ActionComp->AddAction(this, ThornActionClass);
		//set damage here?
		//ThornActionClass->
		//UE_LOG(LogTemp, Log, TEXT("check"));
	//}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	const FName forward = "MoveForward";
	PlayerInputComponent->BindAxis(forward, this, &ASCharacter::MoveForward);
	const FName right = "MoveRight";
	PlayerInputComponent->BindAxis(right, this, &ASCharacter::MoveRight);

	const FName turn = "Turn";
	PlayerInputComponent->BindAxis(turn, this, &APawn::AddControllerYawInput);
	const FName lookup = "Lookup";
	PlayerInputComponent->BindAxis(lookup, this, &APawn::AddControllerPitchInput);

	const FName jump = "Jump";
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);

	const FName blackhole = "Blackhole";
	PlayerInputComponent->BindAction("Blackhole",IE_Pressed, this, &ASCharacter::BlackholeAttack);

	const FName teleportAbility = "TeleportAbility";
	PlayerInputComponent->BindAction("TeleportAbility", IE_Pressed, this, &ASCharacter::TeleportAbility);

	//spawn projectile
	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);

	//interactjion
	const FName interact = "PrimaryInteract";
	PlayerInputComponent->BindAction(interact, IE_Pressed, this, &ASCharacter::PrimaryInteract);

	//actions
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::SprintStop);
}

//movement
void ASCharacter::MoveForward(float value)
{
	FRotator ControlBot = GetControlRotation();
	ControlBot.Pitch = 0.0f;
	ControlBot.Roll = 0.0f;
	AddMovementInput(ControlBot.Vector(), value);


}
void ASCharacter::MoveRight(float value)
{
	FRotator ControlBot = GetControlRotation();
	ControlBot.Pitch = 0.0f;
	ControlBot.Roll = 0.0f;

	// X forward (red), Y is right (Green), z is up (blue) (unlike unity)

	const FVector RightVector = FRotationMatrix(ControlBot).GetScaledAxis(EAxis::Y);

	AddMovementInput(RightVector, value);
}


void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (Delta < 0.0f)                                                //Player took damange
	{
		//UE_LOG(LogTemp, Log, TEXT("On Health changed!"));
		//change material
		
			USkeletalMeshComponent* SkeletalMesh = GetMesh();
			SkeletalMesh->SetScalarParameterValueOnMaterials(TimeToHitParameterName, GetWorld()->TimeSeconds);
			//SkeletalMesh->SetParameterValueOnMaterials("HitFlashColor", HitFlashColor);
			FVector4 ColortoVector = HitFlashColor;
			SkeletalMesh->SetVectorParameterValueOnMaterials("HitFlashColor", ColortoVector);
			
			SkeletalMesh->SetScalarParameterValueOnMaterials("HitFlashSpeed", HitFlashSpeed);


			//Thorns buff here
			//thorns run here - will need to move to StartAction
			//if (ensure(ActionComp))
			//{
				
				//set damage here?
				//ThornActionClass->
				//UE_LOG(LogTemp, Log, TEXT("check"));
			//}
	}

	if (NewHealth <= 0.0f && Delta < 0.0f)
	{
		UE_LOG(LogTemp, Log, TEXT("Player Death!"));
		APlayerController* PC = Cast<APlayerController>(GetController());
		DisableInput(PC);

		SetLifeSpan(8.0f);

	}
}

void ASCharacter::PrimaryAttack()
{
	ActionComp->StartActionByName(this, "PrimaryAttack");

}

void ASCharacter::PrimaryInteract()
{
	if(InteractiveComp)
	{
		InteractiveComp->PrimaryInteract();
	}
	
}

//blackhole assignment 2
void ASCharacter::BlackholeAttack()
{
	//check if we have rage to trigger
	//COULD ALSO DO THIS IN BP for Action_Blackhole
	if (AttributeComponent->SpendRage(this, BlackHoleRageCost)) {
		ActionComp->StartActionByName(this, "Blackhole");
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Not enough Rage!"));
	}

}

void ASCharacter::TeleportAbility()
{
//	UE_LOG(LogTemp, Log, TEXT("Teleport Ability!"));
//	//UE_LOG(LogTemp, Warning, TEXT("BH ATtACK!"));
//	//copy down primary attack once we see the print
//
//	PlayAnimMontage(AttackAnim);
//
//
//	GetWorldTimerManager().SetTimer(TimerHandle_TeleportAttack, this, &ASCharacter::TeleportAbility_TimeElasped, 0.2f);
	//COULD ALSO DO THIS IN BP
	if (ActionComp->CheckActionName("Dash"))
	{
		ActionComp->StartActionByName(this, "Dash");
	}
}

//cheat
void ASCharacter::HealSelf(float Amount /*= 100  */)
{
	AttributeComponent->ApplyHealthChange(this, Amount);

}

//override from 
FVector ASCharacter::GetPawnViewLocation() const
{
	//Super::GetPawnViewLocation();

	return CameraComp->GetComponentLocation(); //FVector();
}

//actions
void ASCharacter::SprintStart()
{
	ActionComp->StartActionByName(this, "Sprint");
}

void ASCharacter::SprintStop()
{
	ActionComp->StopActionByName(this, "Sprint");
}