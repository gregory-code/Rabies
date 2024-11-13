// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GA_SpiderDroneAttack.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"

#include "AI/REnemyAIController.h"

#include "Framework/EOSActionGameState.h"

#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "Enemy/REnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"


UGA_SpiderDroneAttack::UGA_SpiderDroneAttack()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("ability.attack.activate"));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("ability.attack.activate"));
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetAttackingTag());

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetBasicAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_SpiderDroneAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Error, TEXT("Set up attack"));
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("Ending Attack no commitment"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}


	UAbilityTask_WaitGameplayEvent* WaitForActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetBasicAttackActivationTag());
	WaitForActivation->EventReceived.AddDynamic(this, &UGA_SpiderDroneAttack::TryCommitAttack);
	WaitForActivation->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitForDamage = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetGenericTargetAquiredTag());
	WaitForDamage->EventReceived.AddDynamic(this, &UGA_SpiderDroneAttack::HandleDamage);
	WaitForDamage->ReadyForActivation();
}

void UGA_SpiderDroneAttack::TryCommitAttack(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		Character = Cast<ARCharacterBase>(GetOwningActorFromActorInfo());
		AREnemyAIController* aiController = Cast<AREnemyAIController>(Character->GetInstigatorController());
		if (aiController)
		{
			UBlackboardComponent* blackboardComp = aiController->GetBlackboardComponent();
			if (blackboardComp)
			{
				UObject* targetObj = blackboardComp->GetValueAsObject(TargetBlackboardKeyName);
				TargetActor = Cast<AActor>(targetObj);
				if (TargetActor)
				{
					AimHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_SpiderDroneAttack::AimAtTarget, 2.0f));
				}
			}
		}

		if (Character)
		{
			//character->ServerPlayAnimMontage(AttackAnim);
		}
	}
}

void UGA_SpiderDroneAttack::HandleDamage(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(AttackDamage, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);
		SignalDamageStimuliEvent(Payload.TargetData);

	}
}

void UGA_SpiderDroneAttack::AimAtTarget(float aimTime)
{
	aimTime -= GetWorld()->GetDeltaSeconds();
	if (aimTime > 0)
	{
		USkeletalMeshComponent* skeletalMesh = Character->GetMesh();
		if (!skeletalMesh)
			return;
		
		FTransform gunTransform = skeletalMesh->GetBoneTransform(TargetGunName);
		FVector TargetLocation = TargetActor->GetActorLocation();
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(gunTransform.GetLocation(), TargetLocation);

		gunTransform.SetRotation(FQuat(LookAtRotation));
		//skeletalMesh->SetBoneTran(TargetGunName, gunTransform, EBoneSpaces::ComponentSpace);

		// Refresh bone transforms to apply changes
		skeletalMesh->RefreshBoneTransforms();

		UE_LOG(LogTemp, Error, TEXT("Aiming"));

		AimHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_SpiderDroneAttack::AimAtTarget, aimTime));
	}
}
