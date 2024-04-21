#include "GameplayAbilities/GA_Scoping.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

#include "GameplayAbilities/RAbilityGenericTags.h"

UGA_Scoping::UGA_Scoping()
{
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetScopingTag());
}

void UGA_Scoping::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Error, TEXT("Starting Scope maybe?"));

	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("Ending scope no authority"));
		K2_EndAbility();
		return;
	}
}

void UGA_Scoping::TargetCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	UE_LOG(LogTemp, Error, TEXT("Target scoping cancelled"));
	K2_EndAbility();
}
