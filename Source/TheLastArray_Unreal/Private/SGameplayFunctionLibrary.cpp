// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameplayFunctionLibrary.h"
#include <TheLastArray_Unreal/Public/Attributes/SAttributeComponent.h>

/// <summary>
/// The Last Array - Unreal
/// Gameplay Function Library Script
/// Dylan Loe
/// 
/// Last Updated: 10/8/23
/// 
/// Notes:
///  - 
///     
/// </summary>

bool USGameplayFunctionLibrary::ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount)
{
	//UE_LOG(LogTemp, Log, TEXT("In apply damage: %f on %d"), DamageAmount, *GetNameSafe(TargetActor));
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(TargetActor);
	if (AttributeComp)
	{
		return AttributeComp->ApplyHealthChange(DamageCauser, -DamageAmount);
	}


	return false;
}

bool USGameplayFunctionLibrary::ApplyDirectionalDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount, const FHitResult& HitResult)
{
	//add an impulse if we can apply damage successfully
	if (ApplyDamage(DamageCauser, TargetActor, DamageAmount))
	{
		UPrimitiveComponent* HitComp = HitResult.GetComponent();
		if (HitComp && HitComp->IsSimulatingPhysics(HitResult.BoneName))
		{
			//direction is target - origin
			FVector Direciton = HitResult.TraceEnd - HitResult.TraceStart;
			Direciton.Normalize();

			HitComp->AddImpulseAtLocation(Direciton * 300000.0f, HitResult.ImpactPoint, HitResult.BoneName);
		}
		return true;
	}

	return false;
}
