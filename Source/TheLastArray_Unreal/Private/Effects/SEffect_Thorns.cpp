// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/SEffect_Thorns.h"
#include "SGameplayFunctionLibrary.h"
#include "Attributes/SAttributeComponent.h"
#include "Projectiles/SMagicProjectile.h"

/// <summary>
/// The Last Array - Unreal
/// Effect Thorns Script
/// Dylan Loe
/// 
/// Last Updated: 10/8/23
/// 
/// Notes:
///  - 
///     
/// </summary>

USEffect_Thorns::USEffect_Thorns()
{
	ThornReflect = 0.33f;
	Duration = 0.0f;
	Period = 0.0f;
}

void USEffect_Thorns::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	//start listening to OnHealthChanged
	USAttributeComponent* Attributes = USAttributeComponent::GetAttributes(GetOwningComponent()->GetOwner());
	if (Attributes)
	{
		Attributes->OnHealthChanged.AddDynamic(this, &USEffect_Thorns::OnHealthChanged);
	}

}

void USEffect_Thorns::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);

	//unbind
	USAttributeComponent* Attributes = USAttributeComponent::GetAttributes(GetOwningComponent()->GetOwner());
	if (Attributes)
	{
		Attributes->OnHealthChanged.RemoveDynamic(this, &USEffect_Thorns::OnHealthChanged);
	}
}

void USEffect_Thorns::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	//UE_LOG(LogTemp, Log, TEXT("Onhealthchanged called in thorns"));
	if (Delta < 0.0f && InstigatorActor != OwningComp->GetOwner()) {
		//apply percentage damage
		int32 ReflectedAmount = FMath::RoundToInt(Delta * ThornReflect); //FMath::Abs(FMath::CeilToInt(Delta * ThornReflect));

		if (ReflectedAmount == 0)
		{
			return;
		}
		ReflectedAmount = FMath::Abs(ReflectedAmount);
		USGameplayFunctionLibrary::ApplyDamage(OwningComp->GetOwner(), InstigatorActor, ReflectedAmount);
		//UE_LOG(LogTemp, Log, TEXT("Applied Thorn Damage %f"), FMath::CeilToInt( Delta * ThornReflect));
	}
}


