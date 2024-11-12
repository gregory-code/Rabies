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

	Player = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());

	UAbilityTask_WaitInputPress* waitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
	waitInputPress->OnPress.AddDynamic(this, &UGA_RangedGattlingAttack::Fire);
	waitInputPress->ReadyForActivation();

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
}

void UGA_RangedGattlingAttack::Fire(float TimeWaited)
{
	UE_LOG(LogTemp, Warning, TEXT("Firing"));
	if (CheckCooldown(cooldownHandle, actorInfo))
	{
		if (K2_HasAuthority())
		{
			if (Player)
			{
				Player->Hitscan(4000);
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

void UGA_RangedGattlingAttack::StopAttacking(FGameplayEventData Payload)
{
	K2_EndAbility();
}
