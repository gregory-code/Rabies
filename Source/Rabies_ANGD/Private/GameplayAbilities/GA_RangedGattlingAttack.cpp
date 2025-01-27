// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/GA_RangedGattlingAttack.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "Character/RCharacterBase.h"
#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_RangedGattlingAttack::UGA_RangedGattlingAttack()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("ability.attack.activate"));
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetAttackingTag());

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetBasicAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_RangedGattlingAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	cooldownHandle = Handle;
	actorInfo = ActorInfo;
	activationInfo = ActivationInfo;

	Player = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());

	UAbilityTask_WaitGameplayEvent* WaitTargetAquiredEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetGenericTargetAquiredTag());
	WaitTargetAquiredEvent->EventReceived.AddDynamic(this, &UGA_RangedGattlingAttack::Fire);
	WaitTargetAquiredEvent->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitForActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetBasicAttackActivationTag());
	WaitForActivation->EventReceived.AddDynamic(this, &UGA_RangedGattlingAttack::TryCommitAttack);
	WaitForActivation->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitStopEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetEndAttackTag());
	WaitStopEvent->EventReceived.AddDynamic(this, &UGA_RangedGattlingAttack::StopAttacking);
	WaitStopEvent->ReadyForActivation();

	if (Player)
	{
		ClientHitScanHandle = Player->ClientHitScan.AddLambda([this](AActor* hitActor, FVector startPos, FVector endPos)
			{
				RecieveAttackHitscan(hitActor, startPos, endPos);
			});
	}

	SetupWaitInputTask();
}

void UGA_RangedGattlingAttack::Fire(FGameplayEventData Payload)
{
	if (CheckCooldown(cooldownHandle, actorInfo))
	{
		if (K2_HasAuthority())
		{
			if (Player)
			{
				ApplyCooldown(cooldownHandle, actorInfo, activationInfo);
				Player->Hitscan(4000, Player->GetPlayerBaseState());
			}
		}
	}
}

void UGA_RangedGattlingAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ClientHitScanHandle.IsValid() && Player)
	{
		Player->ClientHitScan.Remove(ClientHitScanHandle);
	}
}

void UGA_RangedGattlingAttack::RecieveAttackHitscan(AActor* hitActor, FVector startPos, FVector endPos)
{

	if (K2_HasAuthority())
	{
		if (hitActor == nullptr) return;
		if (hitActor != Player)
		{
			FGameplayEventData Payload = FGameplayEventData();

			Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(hitActor);

			FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(RangedGattlingDamage, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);
			SignalDamageStimuliEvent(Payload.TargetData);
		}
	}
}

void UGA_RangedGattlingAttack::SetupWaitInputTask()
{
	UAbilityTask_WaitInputPress* WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
	WaitInputPress->OnPress.AddDynamic(this, &UGA_RangedGattlingAttack::AbilityInputPressed);
	WaitInputPress->ReadyForActivation();
}

void UGA_RangedGattlingAttack::AbilityInputPressed(float TimeWaited)
{
	SetupWaitInputTask();
	TryCommitAttack(FGameplayEventData());
}

void UGA_RangedGattlingAttack::TryCommitAttack(FGameplayEventData Payload)
{
	Fire(Payload);
}

void UGA_RangedGattlingAttack::StopAttacking(FGameplayEventData Payload)
{
	K2_EndAbility();
}
