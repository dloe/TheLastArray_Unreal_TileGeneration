// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/SMagicProjectile.h"
#include "Attributes/SAttributeComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include <SGameplayFunctionLibrary.h>
#include "Actions/SActionEffect.h"
#include <Actions/SActionComponent.h>

/// <summary>
/// The Last Array - Unreal
/// Magic Projectile Script
/// Dylan Loe
/// 
/// Last Updated: 10/8/23
/// 
/// Notes:
///  - 
///     
/// </summary>

// Sets default values
ASMagicProjectile::ASMagicProjectile()
{

	//UE_LOG(LogTemp, Log, TEXT("check"));
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap);

	myRadialForce = CreateDefaultSubobject<URadialForceComponent>("myRadialForce");
	myRadialForce->Radius = 700;
	myRadialForce->ImpulseStrength = 1000;
	//optional, ignore 'Mass' of other objects (if false, the impulse strength will be much higher to push most objects depending on mass)
	myRadialForce->bImpulseVelChange = false;
	myRadialForce->SetupAttachment(RootComponent);

	InitialLifeSpan = 15.0f;

	AudioCompFlight->Play(0.0f);
	DamageAmount = 20.0f;
	
}
 
void ASMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor && OtherActor != GetInstigator())
	{
		//static FGameplayTag Tag = FGameplayTag::RequestGameplayTag("Status.Parrying");
		//UE_LOG(LogTemp, Log, TEXT("OnActorOverlap in projectile"));

		USActionComponent* ActionComp = Cast<USActionComponent>(OtherActor->GetComponentByClass(USActionComponent::StaticClass()));
		//check for is parrying tag
		if (ActionComp && ActionComp->ActiveGameplayTags.HasTag(ParryTag))
		{
			//get movement comp and invert veleocity, this will cause the projectile to be pointed back
			MoveComp->Velocity = -MoveComp->Velocity;

			//take over ownership so we can fix the get instigator issue while parrying (to avoid skipping the conditional)
			//we will become the dasmage dealer
			SetInstigator(Cast<APawn>(OtherActor));
			return;
		}

		if (USGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, DamageAmount, SweepResult))
		{
			Explode();

			//apply burning
			if (ActionComp && HasAuthority())
			{
				ActionComp->AddAction(GetInstigator(), BurningActionClass);
			}
		}
	}
}

//void ASMagicProjectile::OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
//{
//	Super::OnActorHit(HitComp, OtherActor, OtherComp,NormalImpulse, Hit);
//	//double check this with answer
//	//FString temp = OtherActor->GetFName().ToString();
//	//UE_LOG(LogTemp, Log, TEXT("Other actor: s%"), temp);
//	//if (OtherActor && OtherActor != GetInstigator())
//	//{
//		//UE_LOG(LogTemp, Log, TEXT("OnCompHit in projectile"));
//		//if we hit anything else (logic ported from blueprint)
//
//		DrawDebugSphere(GetWorld(), GetActorLocation(), 100, 12, FColor::White, 1.0f, 0.0f);
//
//		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());
//
//		
//
//		//camera shake on projectile impact
//		//add camera shake at a certain location in the world then look for players within their affected radius and apply 
//		//the scale based on how close they are
//		//const UObject* WorldContextObject, TSubclassOf<class UCameraShakeBase> Shake, FVector Epicenter, float InnerRadius, float OuterRadius, float Falloff, bool bOrientShakeTowardsEpicenter
//		
//		//establish radius for myshake
//		//make outer and inner radius float vars editable via inspector
//		//UGameplayStatics::PlayWorldCameraShake(this, MyShake, Hit.ImpactPoint, ImpactShakeInnerRadius, ImpactShakeOuterRadius, Falloff);
//
//		Destroy();
//	//}
//}

