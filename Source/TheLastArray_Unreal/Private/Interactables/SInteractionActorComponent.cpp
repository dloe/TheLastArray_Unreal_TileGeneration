// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/SInteractionActorComponent.h"
#include "SGameplayInterface.h"
#include "VT/VirtualTextureBuildSettings.h"
#include "Widgets/SWorldUserWidget.h"
#include <TheLastArray_Unreal/TheLastArray_Unreal.h>

static TAutoConsoleVariable<bool> CVarDebugDrawInteraction(TEXT("su.InteractionDebugDraw"), false, TEXT("Enable Debug lines for Interact Command."), ECVF_Cheat);

// Sets default values for this component's properties
USInteractionActorComponent::USInteractionActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	TraceDistance = 500.0f;
	TraceRadius = 30.0f;
	CollisionChannel = ECC_WorldDynamic;
}



// Called when the game starts
void USInteractionActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void USInteractionActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//figure out if we are the owning client, only run this on the controller we are currently controllering 
	APawn* MyPawn = Cast<APawn>(GetOwner());
	if (MyPawn->IsLocallyControlled())
	{

		//we might not need it in tick since it doesnt need to run every tick (maybe like on a timer?)
		FindBestInteractable();
	}
	
}



void USInteractionActorComponent::FindBestInteractable()
{

	bool bDebugDraw = CVarDebugDrawInteraction.GetValueOnGameThread();

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);

	AActor* MyOwner = GetOwner();

	FVector Start;
	FVector End;

	FVector EyeLocation;
	FRotator EyeRotation;
	//this can cause a bug with interacting based on our char eye dir and not our crosshair
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	End = EyeLocation + (EyeRotation.Vector() * TraceDistance); //CameraComp->GetComponentLocation() + (GetControlRotation().Vector() * 1000);//EyeLocation + (EyeRotation.Vector() * 1000);

	//v1
	//FHitResult hit;
	//start and end (using eyelocal for start)
	//bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(hit, EyeLocation, End, ObjectQueryParams);


	//ver 2
	
	TArray<FHitResult> Hits;
	FCollisionShape shape;
	shape.SetSphere(TraceRadius);
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, shape);

	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;

	//clear ref
	FocusedActor = nullptr;

	for (FHitResult hit : Hits)
	{
		AActor* HitActor = hit.GetActor();
		if (HitActor)
		{
			if (bDebugDraw)
				DrawDebugSphere(GetWorld(), hit.ImpactPoint, TraceRadius, 32, LineColor, false, 0.2f);
			if (HitActor->Implements<USGameplayInterface>())
			{
				//UE_LOG(LogTemp, Log, TEXT("Interacting with HitActor"));
				FocusedActor = HitActor;

				//break out of for loop
				break;
			}
		}
	}

	if (FocusedActor)
	{
		if (DefaultWidgetInstance == nullptr && ensure(DefaultWidgetClass)) 
		{
			DefaultWidgetInstance = CreateWidget<USWorldUserWidget>(GetWorld(), DefaultWidgetClass);


		}

		if (DefaultWidgetInstance)
		{
			DefaultWidgetInstance->AttachedActor = FocusedActor;

			if (!DefaultWidgetInstance->IsInViewport())
			{
				DefaultWidgetInstance->AddToViewport();
			}
		}
	}
	else
	{
		//remove the widget if we dont have focused actor
		if (DefaultWidgetInstance)
		{
			DefaultWidgetInstance->RemoveFromParent();

		}
	}



	//FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;

	if (bDebugDraw) {
		DrawDebugLine(GetWorld(), EyeLocation, End, LineColor, false, 2.0f, 0, 2.0f);
	}
}

void USInteractionActorComponent::PrimaryInteract()
{
	ServerInteract(FocusedActor);
	
}


void USInteractionActorComponent::ServerInteract_Implementation(AActor* InFocus)
{
	if (InFocus == nullptr) {
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "No Focus Actor to interact.");
		return;
	}
	APawn* MyPawn = Cast<APawn>(GetOwner());
	//is ok if null ptr 

	LogOnScreen(this, FString::Printf(TEXT("serv Interact")), FColor::Green);
	ISGameplayInterface::Execute_Interact(InFocus, MyPawn);

}