// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/RAbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"

float URAbilitySystemBlueprintLibrary::GetAbilityStaticCooldownDuration(const UGameplayAbility* AbilityCDO)
{
	float duration = 0;

	UGameplayEffect* cooldownEffect = AbilityCDO->GetCooldownGameplayEffect();
	if (cooldownEffect)
	{
		cooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(1, duration);
	}
	return duration;
}
