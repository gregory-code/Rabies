// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/GA_DotFlying.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Framework/EOSPlayerState.h"
#include "Player/RPlayerController.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Engine/World.h"

#include "Net/UnrealNetwork.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"

#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_DotFlying::UGA_DotFlying()
{
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetFlyingTag());
}

void UGA_DotFlying::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("Ending scope no authority"));
		K2_EndAbility();
		return;
	}

	Player = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());
	if (Player == nullptr)
		return;

	bFlying = false;

	UAbilityTask_WaitGameplayEvent* EndTakeOffEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetEndTakeOffChargeTag());
	EndTakeOffEvent->EventReceived.AddDynamic(this, &UGA_DotFlying::StopTakeOff);
	EndTakeOffEvent->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* GravityJumpEffectAdd = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetApplyGravityJump());
	GravityJumpEffectAdd->EventReceived.AddDynamic(this, &UGA_DotFlying::ApplyGravityJump);
	GravityJumpEffectAdd->ReadyForActivation();

	GetWorld()->GetTimerManager().ClearTimer(SlowFallHandle);
	SlowFallHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_DotFlying::ProcessFlying));
	
	GetWorld()->GetTimerManager().ClearTimer(TakeOffHandle);
	CurrentHoldDuration = 0;

	if (!Player->GetCharacterMovement() || Player->GetCharacterMovement()->IsFalling()) return;

	Player->GetAbilitySystemComponent()->AddLooseGameplayTag(URAbilityGenericTags::GetTakeOffDelayTag());
	Player->playerController->ChangeTakeOffState(true, 0);

	TakeOffHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_DotFlying::Hold, CurrentHoldDuration));
}

void UGA_DotFlying::StopFlying()
{
	K2_EndAbility();
}

void UGA_DotFlying::ProcessFlying()
{
	if (bFlying)
	{

		if(Player->GetCharacterMovement()->IsFalling() == false && !Player->IsTakeOffDelay()) StopFlying();

		if (Player->IsHoldingJump() && CurrentGravityDuration >= 1)
		{
			FVector currentVelocity = Player->GetVelocity();
			if (currentVelocity.Z <= 0)
			{
				if (CurrentHoldDuration > 0 && inFall == false)
				{
					Player->GetPlayerBaseState()->Server_ClampVelocity(10.0f);
				}
				else
				{
					Player->GetPlayerBaseState()->Server_ClampVelocity(-10.0f);
					inFall = true;
				}

				FVector airVelocity = Player->GetVelocity();
				airVelocity.Z = 0;

				float multiplier = (airVelocity.Length() >= 300) ? 0.25f : 0.01f;
				CurrentHoldDuration -= GetWorld()->GetDeltaSeconds() * multiplier;
				Player->playerController->ChangeTakeOffState(true, CurrentHoldDuration);
			}
		}
	}

	SlowFallHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_DotFlying::ProcessFlying));
}

void UGA_DotFlying::StopTakeOff(FGameplayEventData Payload)
{
	if (bFlying)
	{
		return;
	}

	K2_EndAbility();
}

void UGA_DotFlying::Hold(float timeRemaining)
{
	if (CurrentHoldDuration <= 1)
	{
		CurrentHoldDuration += GetWorld()->GetDeltaSeconds();
		bool visible = (CurrentHoldDuration >= 1 / 8.0f) ? true : false;
		Player->playerController->ChangeTakeOffState(visible, CurrentHoldDuration);
		TakeOffHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_DotFlying::Hold, CurrentHoldDuration));
	}
	else
	{
		bFlying = true;

		FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(FlyingSpeedClass, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		FlyingSpeedEffectHandle = ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec);

		FGameplayEffectSpecHandle EffectSpec2 = MakeOutgoingGameplayEffectSpec(GravityJumpClass, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		GravityJumpEffectHandle = ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec2);

		Player->GetAbilitySystemComponent()->AddLooseGameplayTag(URAbilityGenericTags::GetUnActionableTag());
		Player->ServerPlayAnimMontage(TakeOffMontage);

		GetWorld()->GetTimerManager().ClearTimer(TakeOffHandle);
		CurrentGravityDuration = 0;
		GravityHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_DotFlying::GravityJump, CurrentGravityDuration));
	}
}

void UGA_DotFlying::GravityJump(float timeRemaining)
{
	if (CurrentGravityDuration <= 1.0f)
	{
		CurrentGravityDuration += GetWorld()->GetDeltaSeconds();
		GravityHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_DotFlying::GravityJump, CurrentGravityDuration));
	}
	else
	{
		RemoveGravityJump();
	}
}

void UGA_DotFlying::ApplyGravityJump(FGameplayEventData Payload)
{
	Player->Jump();
}

void UGA_DotFlying::RemoveGravityJump()
{
	Player->GetAbilitySystemComponent()->AddLooseGameplayTag(URAbilityGenericTags::GetFlyingTag());
	Player->GetAbilitySystemComponent()->RemoveLooseGameplayTag(URAbilityGenericTags::GetTakeOffDelayTag());
	Player->GetAbilitySystemComponent()->RemoveLooseGameplayTag(URAbilityGenericTags::GetUnActionableTag());
}

void UGA_DotFlying::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	Player->playerController->ChangeTakeOffState(false, 0);
	GetWorld()->GetTimerManager().ClearTimer(TakeOffHandle);

	Player->GetAbilitySystemComponent()->RemoveLooseGameplayTag(URAbilityGenericTags::GetTakeOffDelayTag());
	Player->GetAbilitySystemComponent()->RemoveLooseGameplayTag(URAbilityGenericTags::GetFlyingTag());
	Player->GetAbilitySystemComponent()->RemoveLooseGameplayTag(URAbilityGenericTags::GetUnActionableTag());

	Player->ServerPlayAnimMontage(HardLandingMontage);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->RemoveActiveGameplayEffect(FlyingSpeedEffectHandle);
		ASC->RemoveActiveGameplayEffect(GravityJumpEffectHandle);
		ASC->RemoveActiveGameplayEffect(GravitySlowFallEffectHandle);
		ASC->RemoveActiveGameplayEffect(GravityFallEffectHandle);
	}
}
