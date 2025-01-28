// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/GA_DotMelee.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"

#include "Player/RPlayerBase.h"

#include "Framework/EOSActionGameState.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_DotMelee::UGA_DotMelee()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("ability.attack.activate"));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("ability.attack.activate"));
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetAttackingTag());

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetBasicAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_DotMelee::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("Ending Attack no commitment"));
		K2_EndAbility();
		//EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (!CheckCooldown(Handle, ActorInfo))
	{
		K2_EndAbility();
		return;
	}

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	ARPlayerBase* character = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());

	UAbilityTask_PlayMontageAndWait* playTargettingMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, character->IsFlying() ? FlyingAttackAnim : FlyingAttackAnim);
	playTargettingMontageTask->OnBlendOut.AddDynamic(this, &UGA_DotMelee::K2_EndAbility);
	playTargettingMontageTask->OnInterrupted.AddDynamic(this, &UGA_DotMelee::K2_EndAbility);
	playTargettingMontageTask->OnCompleted.AddDynamic(this, &UGA_DotMelee::K2_EndAbility);
	playTargettingMontageTask->OnCancelled.AddDynamic(this, &UGA_DotMelee::K2_EndAbility);
	playTargettingMontageTask->ReadyForActivation();

	FGameplayEffectSpecHandle pushSpec = MakeOutgoingGameplayEffectSpec(MeleePushClass, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
	pushSpec.Data.Get()->SetSetByCallerMagnitude(URAbilityGenericTags::GetGenericTargetAquiredTag(), 0.0f);
	ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, pushSpec);

	UAbilityTask_WaitGameplayEvent* WaitForDamage = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetGenericTargetAquiredTag());
	WaitForDamage->EventReceived.AddDynamic(this, &UGA_DotMelee::HandleDamage);
	WaitForDamage->ReadyForActivation();

	GetWorld()->GetTimerManager().SetTimer(ZoomTimerHandle, this, &UGA_DotMelee::DotSuperZoom, 0.5f, false);
}

void UGA_DotMelee::HandleDamage(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(AttackDamage, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);
		SignalDamageStimuliEvent(Payload.TargetData);

	}
}

void UGA_DotMelee::DotSuperZoom()
{
	FGameplayEffectSpecHandle pushSpec = MakeOutgoingGameplayEffectSpec(MeleePushClass, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
	pushSpec.Data.Get()->SetSetByCallerMagnitude(URAbilityGenericTags::GetGenericTargetAquiredTag(), 1000.0f);
	ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, pushSpec);
}
