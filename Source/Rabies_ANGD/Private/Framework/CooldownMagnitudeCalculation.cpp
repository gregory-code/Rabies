// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CooldownMagnitudeCalculation.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "GameplayEffectTypes.h"
#include "Player/RPlayerBase.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"

float UCooldownMagnitudeCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    float ShootingSpeed = BaseMagnitude;

    const UAbilitySystemComponent* SourceASC = Spec.GetContext().GetOriginalInstigatorAbilitySystemComponent();

    // Check if source and target exist
    if (SourceASC)
    {
        // Get the instigator’s strength attribute
        float rangedAttackSpeed = 1.0;

        if(meleeCooldown)
            SourceASC->GetNumericAttributeBase(URAttributeSet::GetMeleeAttackCooldownReductionAttribute());

        if(rangedCooldown)
            SourceASC->GetNumericAttributeBase(URAttributeSet::GetRangedAttackCooldownReductionAttribute());

        if (specialCooldown)
            SourceASC->GetNumericAttributeBase(URAttributeSet::GetAbilityCooldownReductionAttribute());

        if (ultimateCooldown)
            SourceASC->GetNumericAttributeBase(URAttributeSet::GetUltimateCooldownReductionAttribute());

        ShootingSpeed *= rangedAttackSpeed;
        ShootingSpeed = FMath::Max(ShootingSpeed, 0.1f); // Ensure the result is not 0
    }

    return ShootingSpeed;
}
