// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Dot/RDot_SpecialProj.h"
#include "Character/RCharacterBase.h"
#include "Enemy/REnemyBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "AbilitySystemComponent.h"

#include "Particles/ParticleSystem.h"
#include "Perception/AISense_Damage.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "Net/UnrealNetwork.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"

#include "Character/RCharacterBase.h"
#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

void ARDot_SpecialProj::HitCharacter(ARCharacterBase* usingCharacter, ARCharacterBase* hitCharacter, bool isEnemy, int hitNumber)
{
	if (isEnemy == false)
		return;

	for (TSubclassOf<UGameplayEffect>& damageEffect : EffectsToApply)
	{
		FGameplayEventData Payload = FGameplayEventData();

		FGameplayEffectContextHandle contextHandle = usingCharacter->GetAbilitySystemComponent()->MakeEffectContext();
		FGameplayEffectSpecHandle effectSpechandle = usingCharacter->GetAbilitySystemComponent()->MakeOutgoingSpec(damageEffect, 1.0f, contextHandle);

		FGameplayEffectSpec* spec = effectSpechandle.Data.Get();
		if (spec)
		{
			hitCharacter->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);

			UAISense_Damage::ReportDamageEvent(this, hitCharacter, usingCharacter, 1, hitCharacter->GetActorLocation(), hitCharacter->GetActorLocation());
		}
	
	}
}

void ARDot_SpecialProj::Init(TArray<TSubclassOf<UGameplayEffect>> effectsToApply)
{
	EffectsToApply = effectsToApply;
	OnHitCharacter.AddUObject(this, &ARDot_SpecialProj::HitCharacter);
}
