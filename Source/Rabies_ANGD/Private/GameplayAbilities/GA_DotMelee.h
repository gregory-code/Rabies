// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_DotMelee.generated.h"

/**
 * 
 */
UCLASS()
class UGA_DotMelee : public UGA_AbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_DotMelee();

private:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void HandleDamage(FGameplayEventData Payload);

	UFUNCTION()
	void DotSuperZoom();

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> AttackDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* MeleeAttackAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* FlyingAttackAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> MeleePushClass;

	FTimerHandle ZoomTimerHandle;
};
