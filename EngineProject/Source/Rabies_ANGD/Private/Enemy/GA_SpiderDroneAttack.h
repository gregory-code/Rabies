// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_SpiderDroneAttack.generated.h"

/**
 * 
 */
UCLASS()
class UGA_SpiderDroneAttack : public UGA_AbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_SpiderDroneAttack();

private:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void RecieveAttackHitscan(AActor* hitActor, FVector startPos, FVector endPos, bool bIsCrit);

	UFUNCTION()
	void TryCommitAttack(FGameplayEventData Payload);

	UFUNCTION()
	void HandleDamage(FGameplayEventData Payload);

	UFUNCTION()
	void StartAiming(FGameplayEventData Payload);

	UFUNCTION()
	void EndAiming(FGameplayEventData Payload);

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> AttackDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* AttackAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* ShootingParticle;

	FDelegateHandle ClientHitScanHandle;

	class AREnemyBase* Character;

	AActor* TargetActor;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetBlackboardKeyName = "Target";


	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetGunName = "gun";
};
