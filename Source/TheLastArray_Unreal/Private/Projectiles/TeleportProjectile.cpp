// Fill out your copyright notice in the Description page of Project Settings.


#include "TeleportProjectile.h"
#include "SAttributeComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>

// Sets default values
ATeleportProjectile::ATeleportProjectile()
{
	//UE_LOG(LogTemp, Log, TEXT("poop!"));
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	TeleportDelay = 0.2f;
	DetonateDelay = 0.2f;

	MoveComp->InitialSpeed = 6000.f;

}

void ATeleportProjectile::BeginPlay()
{
	Super::BeginPlay();
	//start a timer for 0.2 seconds
	GetWorld()->GetTimerManager().SetTimer(TeleportTimerHandle, this, &ATeleportProjectile::Explode, DetonateDelay);
}


void ATeleportProjectile::OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//UE_LOG(LogTemp, Log, TEXT("Hit2!"));
	//if(OtherActor && OtherActor != GetInstigator())
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		//UE_LOG(LogTemp, Log, TEXT("Hit!"));
		DrawDebugSphere(GetWorld(), GetActorLocation(), 100, 12, FColor::Red, 1.0f, 0.0f);
		//run detonation sequence
		Explode_Implementation();
	}
}

void ATeleportProjectile::Explode_Implementation()
{
	GetWorldTimerManager().ClearTimer(TeleportTimerHandle);
	//UE_LOG(LogTemp, Log, TEXT("Start Detonation!"));
	//stop movement
	MoveComp->StopMovementImmediately();
	//nothing can collide while we explode
	SetActorEnableCollision(false);

	//explode
	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());
	EffectComp->DeactivateSystem();
	
	//or spawn emitter at location?
	//ExplosionComp->Activate();
	//wait 2 more seconds
	FTimerHandle TimerHandle_DelayedTeleport;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_DelayedTeleport, this, &ATeleportProjectile::TeleportSequence,TeleportDelay);
	//teleport sequence


	//skip base implementation as it will destroy actor (we need to stay alive a bit longer to finish second timer)
	//Super::Explode_Implementation();
}

//aka teleport instigator
void ATeleportProjectile::TeleportSequence()
{
	AActor* ActorToTeleport = GetInstigator();
	if (ensure(ActorToTeleport)) {
		UE_LOG(LogTemp, Log, TEXT("Teleport Player!"));
		//teleport player (just teleport the x and z NOT THE Y)
		//somehow get reference to our character
		ActorToTeleport->TeleportTo(GetActorLocation(), ActorToTeleport->GetActorRotation(), false, false);

		//destroy this obj
		//Destroy();
	}
}





