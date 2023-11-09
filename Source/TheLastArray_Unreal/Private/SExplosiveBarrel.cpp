// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"

/// <summary>
/// The Last Array - Unreal
/// Explosive Barrel Script
/// Dylan Loe
/// 
/// Last Updated: 10/8/23
/// 
/// Notes:
///  - 
///     
/// </summary>

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Base = CreateDefaultSubobject<UStaticMeshComponent>("Base");
	//what it can collide with (what profile we using)
	Base->SetCollisionProfileName("PhysicsActor");
	//make sure it can interact
	Base->SetSimulatePhysics(true);
	//setup for rigid body collision
	Base->SetNotifyRigidBodyCollision(true);
	
	RootComponent = Base;

	myRadialForce = CreateDefaultSubobject<URadialForceComponent>("myRadialForce");
	myRadialForce->Radius = 700;
	myRadialForce->ImpulseStrength = 2000;
	//optional, ignore 'Mass' of other objects (if false, the impulse strength will be much higher to push most objects depending on mass)
	myRadialForce->bImpulseVelChange = true;
	myRadialForce->SetupAttachment(RootComponent);
	//myRadialForce->AddObjectTypeToAffect(ECC_Pawn); //ECC_PhysicsBody, ECC_Vehicle, ECC_Destructable

}

void ASExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//set up on component hit function for static mesh (also called it oncomphit
	Base->OnComponentHit.AddDynamic(this, &ASExplosiveBarrel::OnCompHit);
}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();

	//add radial impulse
	//Base->AddRadialImpulse();
	
}

//on component hit
void ASExplosiveBarrel::OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//if the colliding component isnt null, not us and their component isnt null
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		//fire impulse
		myRadialForce->FireImpulse();
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Barrel Hit: %s"), *OtherActor->GetName()));

		UE_LOG(LogTemp, Log, TEXT("OnActorHit in explosive barrel"));

		UE_LOG(LogTemp, Warning, TEXT("OtherActor: %s, at game time: %f"), *GetNameSafe(OtherActor), GetWorld()->TimeSeconds);

		FString CombinedString = FString::Printf(TEXT("Hit at location: %s, at game time %f"), *Hit.ImpactPoint.ToString());
		DrawDebugString(GetWorld(), Hit.ImpactPoint, CombinedString, nullptr, FColor::Green, 2.0f, true);
	}

}

// Called every frame
void ASExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



