// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Dot/RTargetActor_DotUltimate.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Player/RPlayerController.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/RPlayerBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "Engine/World.h"
#include "Components/DecalComponent.h"
#include "Enemy/REnemyBase.h"
#include "Net/UnrealNetwork.h"

ARTargetActor_DotUltimate::ARTargetActor_DotUltimate()
{
	PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;
    ShouldProduceTargetDataOnServer = true;

    // Create Cylinder Mesh
    CylinderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CylinderMesh"));
    RootComponent = CylinderMesh;

    // Assign the default cylinder mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderAsset(TEXT("/Engine/BasicShapes/Cylinder"));
    if (CylinderAsset.Succeeded())
    {
        CylinderMesh->SetStaticMesh(CylinderAsset.Object);
    }

	SetRootComponent(CylinderMesh);

    CylinderMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CylinderMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    CylinderMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ARTargetActor_DotUltimate::SetParameters(float radius, float height, FRotator rotation, FVector location)
{
    CylinderRadius = radius;
    CylinderHeight = height;
    CylinderRotation = rotation;
    CylinderLocation = location;
}

void ARTargetActor_DotUltimate::SetBetweenTwoPoints_Implementation(const FVector& start, const FVector& end, bool bigLaser)
{
    FVector adjustedEnd = end + ((end - start) * 2.0f);

    FVector Direction = (adjustedEnd - start);
    FVector Midpoint = (start + (adjustedEnd)) * 0.5f;

    float Length = Direction.Size();

    SetActorLocation(Midpoint);
    SetActorRotation(CylinderRotation);

    CylinderHeight = Length;
    CylinderRadius = (bigLaser) ? 150.0f : 50.0f;

    // Scale the cylinder to match length (assuming default cylinder height = 200 units)
    float xyScale = (bigLaser) ? 3.0f : 1.0f ;
    FVector Scale = FVector(xyScale, xyScale, Length / 100.0f);
    CylinderMesh->SetWorldScale3D(Scale);
}
