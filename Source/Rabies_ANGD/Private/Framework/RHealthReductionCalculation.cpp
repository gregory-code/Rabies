// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/RHealthReductionCalculation.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "GameplayEffectTypes.h"
#include "Player/RPlayerBase.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "Character/RCharacterBase.h"

float URHealthReductionCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    float BaseMagnitude = 0.0f;

    const UAbilitySystemComponent* SourceASC = Spec.GetContext().GetOriginalInstigatorAbilitySystemComponent();
    const UAbilitySystemComponent* TargetASC = nullptr; // Cast<ARCharacterBase>(Spec.GetContext().GetEffectCauser())->GetAbilitySystemComponent(); // there is like no good way to get this sadly

    if (Spec.GetContext().GetHitResult())
    {
        // Get the actor from the hit result
        AActor* TargetActor = Spec.GetContext().GetHitResult()->GetActor();
        if (TargetActor)
        {
            TargetASC = Cast<ARCharacterBase>(TargetActor)->GetAbilitySystemComponent();
        }
    }

    // Check if source and target exist
    if (SourceASC && TargetASC)
    {
        // Get the instigator’s strength attribute
        float Strength = SourceASC->GetNumericAttributeBase(URAttributeSet::GetRangedAttackStrengthAttribute()); // THIS IS FOR THE RANGEDATTACK

        // Get the target’s defense attribute
        float Defense = TargetASC->GetNumericAttributeBase(URAttributeSet::GetDamageReductionAttribute());

        // Calculate the magnitude based on strength and defense
        BaseMagnitude = Strength - Defense;
        BaseMagnitude = FMath::Max(BaseMagnitude, 1.0f); // Ensure the result deals at least 1 damage
    }

    return BaseMagnitude;
}
