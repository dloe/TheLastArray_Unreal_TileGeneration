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

/// <summary>
/// Player Controller Script
/// Dylan Loe
/// 
/// Last Updated: 10/8/23
/// 
/// Notes:
///  - Main controller for player movement (in 3d environment)
///	 - incorporates attribute components to handle
///  - incorporates action component to handle
///  
/// - this should have proper networking implementation in the event I end up adding some co-op option for this prototype.
///     
/// </summary>

// Sets default values
ASCharacter::ASCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;

	//
	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	//
	EffectSpellCastComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectSpellCastComp->SetupAttachment(RootComponent);

	InteractiveComp = CreateDefaultSubobject<USInteractionActorComponent>("InteractionComp");
	AttributeComponent = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");
	ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");

	//rotate to whatever we are moving towards
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	// for now blackhole special abilities cost 25 rage, this is hardcoded for test purposes
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
	
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/// <summary>
/// Called to bind functionality to input
/// </summary>
/// <param name="PlayerInputComponent"></param>
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

	//interaction
	const FName interact = "PrimaryInteract";
	PlayerInputComponent->BindAction(interact, IE_Pressed, this, &ASCharacter::PrimaryInteract);

	//actions
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::SprintStop);
}

//movement for forward moving and for moving right
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


/// <summary>
/// 
/// - Runs when our player has a health change
/// 
/// </summary>
/// <param name="InstigatorActor"> Instigator</param>
/// <param name="OwningComp"> Owning Component</param>
/// <param name="NewHealth"> New health value</param>
/// <param name="Delta"> Health Delta Change</param>
void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	//Player took damage
	if (Delta < 0.0f) 
	{
		//UE_LOG(LogTemp, Log, TEXT("On Health changed!"));
		//change material
		
			USkeletalMeshComponent* SkeletalMesh = GetMesh();
			SkeletalMesh->SetScalarParameterValueOnMaterials(TimeToHitParameterName, GetWorld()->TimeSeconds);
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

/// <summary>
/// Action Component: Start projectile attack for shooting
/// </summary>
void ASCharacter::PrimaryAttack()
{
	ActionComp->StartActionByName(this, "PrimaryAttack");

}

/// <summary>
/// Interactive Component: run primary interact for item pickups, door interactions and others
/// </summary>
void ASCharacter::PrimaryInteract()
{
	if(InteractiveComp)
	{
		InteractiveComp->PrimaryInteract();
	}
	
}

/// <summary>
/// Attribute Component: It costs rage to use this special attack, rage is stored in the attribute component
/// Action Component: Start Blackhole action for blackhole attack
/// </summary>
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

/// <summary>
/// 
/// - Originally this was all called inside our player but have moved the teleport ability to its own ActionComponent that we instead call here
/// 
/// </summary>
void ASCharacter::TeleportAbility()
{

	//	GetWorldTimerManager().SetTimer(TimerHandle_TeleportAttack, this, &ASCharacter::TeleportAbility_TimeElasped, 0.2f);
	//Note to self: This could also be done in bp
	if (ActionComp->CheckActionName("Dash"))
	{
		ActionComp->StartActionByName(this, "Dash");
	}
}

/// <summary>
/// For debug/cheat purposes in testing
/// 
/// </summary>
/// <param name="Amount"></param>
void ASCharacter::HealSelf(float Amount /*= 100  */)
{
	AttributeComponent->ApplyHealthChange(this, Amount);

}

/// <summary>
/// Overriding pawn view location for our own player usage
/// </summary>
/// <returns></returns>
FVector ASCharacter::GetPawnViewLocation() const
{
	// Note to self: dont call super here since we return the incorrect value, lets use cameracomponent instead
	//Super::GetPawnViewLocation();

	return CameraComp->GetComponentLocation(); //FVector();
}


/// <summary>
///  Action Component: start sprint action
/// </summary>
void ASCharacter::SprintStart()
{
	ActionComp->StartActionByName(this, "Sprint");
}
/// <summary>
/// Action Component: start sprint action
/// </summary>
void ASCharacter::SprintStop()
{
	ActionComp->StopActionByName(this, "Sprint");
}