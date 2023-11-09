// Fill out your copyright notice in the Description page of Project Settings.


#include "Attributes/SAttributeComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Net/UnrealNetwork.h"
#include <TheLastArray_Unreal/Public/SGameModeBase.h>
#include "Actions/SActionEffect.h"
#include <Actions/SActionComponent.h>

/// <summary>
/// Attribute Component Script
/// Dylan Loe
/// 
/// Last Updated: 10/8/23
/// 
/// Notes:
///  - contains all our attributes
///		- IE max health, health, 
///  - idea is we can attach this attribute component to our player, ai characters, etc and will have a  manageable component for keeping track
/// 
/// - there is tests for server replication so this attribute component should have proper networking
///     
/// </summary>

//cheat var for upping our damage for test purposes
static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamageMultiplier"), 1.0f, TEXT("Global Damage Multiplier for Attribute Component."), ECVF_Cheat);

/// <summary>
/// Sets default values for this component's properties
/// </summary>
USAttributeComponent::USAttributeComponent()
{
	// set to max health on start
	Health = HealthMax;
	Rage = 0;
	//UE_LOG(LogTemp, Log, TEXT("Starting Health: %f"), Health);
	//UE_LOG(LogTemp, Log, TEXT("Max Health: %f"), HealthMax);
	OnHealthChanged.Broadcast(NULL, this, Health, 0);
	OnRageChanged.Broadcast(this, Rage, 0);

	SetIsReplicatedByDefault(true);
}

/// <summary>
/// 
/// </summary>
/// <param name="InstigatorActor"></param>
/// <returns></returns>
bool USAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -GetHealthMax());
}

/// <summary>
/// 
/// </summary>
/// <returns></returns>
bool USAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

/// <summary>
/// 
/// </summary>
/// <param name="Instigator"></param>
/// <param name="Delta"></param>
/// <returns></returns>
bool USAttributeComponent::ApplyHealthChange(AActor* Instigator, float Delta)
{
	//godmod
	if (!GetOwner()->CanBeDamaged() && Delta < 0.0f)
		return false;

	//replication bug
	//if (!GetOwner()->HasAuthority())
	//{
		//return false;
	//}



	//this does it to EVERYTHING, including player, enemies, explosives, etc
	//potentially could have mutliple versions to tweak more granularly
	if (Delta < 0.0f)
	{
		float DamageMultiplier = CVarDamageMultiplier.GetValueOnGameThread();

		Delta *= DamageMultiplier;
	}

	float OldHealth = Health;
	float NewHealth = FMath::Clamp(Health + Delta, 0.0f, HealthMax);
	//UE_LOG(LogTemp, Log, TEXT("before Health: %f"), Health);

	//UE_LOG(LogTemp, Log, TEXT("after Health: %f"), Health);
	float HealthDelta = NewHealth - OldHealth;
	//OnHealthChanged.Broadcast(Instigator, this, Health, HealthDelta);

	if (GetOwner()->HasAuthority())
	{
		Health = NewHealth;

		if (HealthDelta != 0.0f)
		{
			MulticastHealthChanged(Instigator, Health, HealthDelta);
		}

		//UE_LOG(LogTemp, Log, TEXT("True Delta: %d"), FMath::Abs(Delta));
//could add a rage multiplier
		ApplyRageChange(Instigator, FMath::Abs(Delta));

		//died
		if (HealthDelta < 0.0f && Health == 0.0f)
		{
			ASGameModeBase* GM = GetWorld()->GetAuthGameMode<ASGameModeBase>();
			if (GM)
			{
				GM->OnActorKilled(GetOwner(), Instigator);
			}
		}
	}
	//UE_LOG(LogTemp, Log, TEXT("New Health: %f"), Health);
	//OnHealthChanged.Broadcast(nullptr, this, Health, Delta);
	//for now it will return true, we can add to this later

	//make sure we know if this even applied a health change
	return HealthDelta != 0;
}

/// <summary>
/// 
/// </summary>
/// <param name="Instigator"></param>
/// <param name="Delta"></param>
void USAttributeComponent::ApplyRageChange(AActor* Instigator, float Delta)
{
	//UE_LOG(LogTemp, Log, TEXT("BEFORE Range Delta: %d"), Rage);
	//add rage when we take damage
	Rage = FMath::Clamp(Rage + Delta, 0.0f, RageMax);
	//UE_LOG(LogTemp, Log, TEXT("AFTER Range Delta: %d"), Rage);
	//Update Rage UI 
	MulticastRageChanged(Instigator, Rage, Delta);
	//OnRageChanged.Broadcast(this, Rage, Delta);

}

/// <summary>
/// 
/// </summary>
/// <param name="Instigator"></param>
/// <param name="CostDelta"></param>
/// <returns></returns>
bool USAttributeComponent::SpendRage(AActor* Instigator, float CostDelta)
{
	if (Rage > CostDelta)
	{
		ApplyRageChange(Instigator, -CostDelta);
		return true;
	} 
	return false;
}

/// <summary>
/// 
/// </summary>
/// <returns></returns>
bool USAttributeComponent::IsUnderMaxHealth() const
{
	//UE_LOG(LogTemp, Log, TEXT("Health: %f"), Health);
	return Health < HealthMax;
}

/// <summary>
/// 
/// </summary>
/// <returns></returns>
float USAttributeComponent::GetHealthMax() const
{
	return HealthMax;
}

/// <summary>
/// Static function
/// </summary>
/// <param name="FromActor"></param>
/// <returns></returns>
USAttributeComponent* USAttributeComponent::GetAttributes(AActor* FromActor)
{
	if (FromActor)
	{
		return Cast<USAttributeComponent>(FromActor->GetComponentByClass(USAttributeComponent::StaticClass()));
	}


	return nullptr;
}

/// <summary>
/// 
/// </summary>
/// <param name="Actor"></param>
/// <returns></returns>
bool USAttributeComponent::IsActorAlive(AActor* Actor)
{
	USAttributeComponent* AttributeComp = GetAttributes(Actor);
	if (AttributeComp)
	{
		return AttributeComp->IsAlive();
	}
	//if we dont have an attribute assume we are dead
	return false;
}

/// <summary>
/// 
/// </summary>
/// <param name="OutLifetimeProps"></param>
void USAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAttributeComponent, Health);
	DOREPLIFETIME(USAttributeComponent, HealthMax);
	DOREPLIFETIME(USAttributeComponent, RageMax);
	DOREPLIFETIME(USAttributeComponent, Rage);
	//DOREPLIFETIME_CONDITION(USAttributeComponent, HealthMax, COND_InitialOnly);
}

/// <summary>
/// 
/// </summary>
/// <param name="InstigatorActor"></param>
/// <param name="NewHealth"></param>
/// <param name="Delta"></param>
void USAttributeComponent::MulticastHealthChanged_Implementation(AActor* InstigatorActor, float NewHealth, float Delta)
{
	OnHealthChanged.Broadcast(InstigatorActor, this, NewHealth, Delta);
}

/// <summary>
/// 
/// </summary>
/// <param name="InstigatorActor"></param>
/// <param name="NewRage"></param>
/// <param name="Delta"></param>
void USAttributeComponent::MulticastRageChanged_Implementation(AActor* InstigatorActor, float NewRage, float Delta)
{
	OnRageChanged.Broadcast(this, Rage, Delta);
}