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

	UFUNCTION(NetMulticast, Reliable)
	void SetParameters(float radius, float height, FRotator rotation, FVector location);

private:

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	UStaticMeshComponent* CylinderMesh;

	float CylinderRadius;
	float CylinderHeight;
	FRotator CylinderRotation;
	FVector CylinderLocation;
};
