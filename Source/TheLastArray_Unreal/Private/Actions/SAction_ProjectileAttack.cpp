// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SAction_ProjectileAttack.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

USAction_ProjectileAttack::USAction_ProjectileAttack()
{
	HandSocketName = "Muzzle_01";
	AttacAnimDelay = 0.2f;
}

void USAction_ProjectileAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	ACharacter* Character = Cast<ACharacter>(Instigator);
	if (Character)
	{
		Character->PlayAnimMontage(AttackAnim);

		UGameplayStatics::SpawnEmitterAttached(CastingEffects, Character->GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);

		if (Character->HasAuthority()) {
			FTimerHandle TimerHandle_AttackDelay;
			FTimerDelegate Delegate;
			Delegate.BindUFunction(this, "AttackDelay_Elasped", Character);

			GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, Delegate, AttacAnimDelay, false);
		}
	}
}

void USAction_ProjectileAttack::AttackDelay_Elasped(ACharacter* InstigatorCharacter)
{
	if (ensureAlways(ProjectileClass))
	{
		//use handsocketname instead of muzzle_01
		const FVector HandLocation = InstigatorCharacter->GetMesh()->GetSocketLocation(HandSocketName);

		//EPSCPoolMethod PoolingMethod; //defaults to none
		//attach location can be KeepWorldPosition or KeepRelativeOffset
		// scale shouldnt be needed if we use keepworldposition

		//have particle effect in players hand when shooting projectile
		//should be where our hand socket is, where the projectile spawns in
		// cast root component as a scene component?
		//UParticleSystem* EmitterTemplate, USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, FVector Scale, EAttachLocation::Type LocationType, bool bAutoDestroy, EPSCPoolMethod PoolingMethod, bool bAutoActivateSystem
		// instructor used CastSpellVFX, GetMesh,HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget
		//UGameplayStatics::SpawnEmitterAttached(CastSpellVFX, GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
		//UGameplayStatics::SpawnEmitterAttached(CastSpellVFX, GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true, EPSCPoolMethod::None, true);
		//UGameplayStatics::SpawnEmitterAttached(CastSpellVFX, GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = InstigatorCharacter;


		FCollisionShape Shape;
		Shape.SetSphere(20.0f);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(InstigatorCharacter);

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);


		//for trace
		float basicTraceRange = 5000;

		//the crosshair is at the forward vector of camera so might as well just use that
		FVector TraceStart = InstigatorCharacter->GetPawnViewLocation();//CameraComp->GetComponentLocation();

		//endpoint far into the look at distance (not to far, still somewhat towards crosshair on a miss)
		FVector TraceEnd = TraceStart + (InstigatorCharacter->GetControlRotation().Vector() * basicTraceRange);

		FHitResult hitCam;
		//if we dont hit anything just use the end of the trace line
		FVector projectileEndLocale = TraceEnd;
		//if(GetWorld()->LineTraceSingleByObjectType(hitCam, TraceStart, TraceEnd, ObjectQueryParams))
		if (GetWorld()->SweepSingleByObjectType(hitCam, TraceStart, TraceEnd, FQuat::Identity, ObjectQueryParams, Shape, Params))
		{
			//if we got a hit, then we now have a start (handlocation), and end point (hitCam.ImpactPoint)
			projectileEndLocale = hitCam.ImpactPoint;
		}

		//OLD - rotation is looking at that point we now have
		//FRotator ProjectileSpawnRotation = UKismetMathLibrary::FindLookAtRotation(HandLocation, projectileEndLocale);

		//more accurate version
		FRotator ProjRotation = FRotationMatrix::MakeFromX(projectileEndLocale - HandLocation).Rotator();


		//replaced GetControlRotation with our new target rotation
		const FTransform SpawnTM = FTransform(ProjRotation, HandLocation);
		AActor* T = GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
	}

	StopAction(InstigatorCharacter);
}


