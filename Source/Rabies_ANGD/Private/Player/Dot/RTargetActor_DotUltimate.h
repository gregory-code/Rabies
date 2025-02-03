// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "RTargetActor_DotUltimate.generated.h"

/**
 * 
 */
UCLASS()
class ARTargetActor_DotUltimate : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
public:
	ARTargetActor_DotUltimate();

	UFUNCTION(NetMulticast, Reliable)
	void SetBetweenTwoPoints(const FVector& start, const FVector& end, bool bigLaser);

	void SetOwningPlayerControler(class ARPlayerBase* myPlayer);

	void SetDamageEffects(TSubclassOf<class UGameplayEffect> attackDamage);

private:

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> AttackDamage;

	UFUNCTION()
	void CheckDamage();

	FTimerHandle DamageTimer;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	UStaticMeshComponent* CylinderMesh;

	class ARPlayerBase* MyPlayer;

	float CylinderRadius;
	float CylinderHeight;
};
