#include "GameplayAbilities/GA_Scoping.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"

#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_Scoping::UGA_Scoping()
{
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetScopingTag());
}

void UGA_Scoping::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("Ending scope no authority"));
		K2_EndAbility();
		return;
	}

	//UAbilityTask_WaitCancel* WaitCancel = UAbilityTask_WaitCancel::WaitCancel(this);
	//WaitCancel->OnCancel.AddDynamic(this, &UGA_Scoping::StopScoping); // Use a WaitEvent instead of waitCancel, WaitCancel is a generic keyword used a lot
	//WaitCancel->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitStopEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetEndScopingTag());
	WaitStopEvent->EventReceived.AddDynamic(this, &UGA_Scoping::StopScoping);
	WaitStopEvent->ReadyForActivation();

	ARPlayerBase* player = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());
	if (player)
	{
		player->SetMovementSpeed(player->BaseScopingSpeed);
		UE_LOG(LogTemp, Error, TEXT("Scoping speed"));
	}
}

void UGA_Scoping::StopScoping(FGameplayEventData Payload)
{
	ARPlayerBase* player = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());
	if (player)
	{
		player->SetMovementSpeed(player->BaseMovementSpeed);
		UE_LOG(LogTemp, Error, TEXT("Regular walking Speed"));
	}

	K2_EndAbility();
}
