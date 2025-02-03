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

void ARTargetActor_DotUltimate::SetOwningPlayerControler(class ARPlayerBase* myPlayer)
{
    MyPlayer = myPlayer;
    GetWorldTimerManager().SetTimer(DamageTimer, this, &ARTargetActor_DotUltimate::CheckDamage, 0.2f, true);
}

void ARTargetActor_DotUltimate::SetDamageEffects(TSubclassOf<class UGameplayEffect> attackDamage)
{
    AttackDamage = attackDamage;
}

void ARTargetActor_DotUltimate::CheckDamage()
{
    if (MyPlayer == nullptr || AttackDamage == nullptr)
        return;

    // Get Start and End positions of the cylinder
    //FVector startLocation = GetActorLocation() - (GetActorUpVector() * (CylinderHeight / 2));
    //FVector endLocation = GetActorLocation() + (GetActorUpVector() * (CylinderHeight / 2));

    float CapsuleRadius = CylinderRadius;
    float CapsuleHalfHeight = CylinderHeight / 2;
    FQuat CapsuleRotation = GetActorQuat();

    TArray<FOverlapResult> OverlapResults;

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    QueryParams.AddIgnoredActor(MyPlayer);

    bool bHit = GetWorld()->OverlapMultiByChannel(OverlapResults, GetActorLocation(), CapsuleRotation, ECC_PhysicsBody, FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight), QueryParams);

    DrawDebugCapsule(GetWorld(), GetActorLocation(), CapsuleHalfHeight, CapsuleRadius, CapsuleRotation, FColor::Blue, false, 0.3f);
    for (const FOverlapResult& Result : OverlapResults)
    {
        if (Result.GetActor())
        {
            AREnemyBase* potentialEnemy = Cast<AREnemyBase>(Result.GetActor());
            if (potentialEnemy == nullptr)
                return;

            FGameplayEffectSpecHandle specHandle = MyPlayer->GetAbilitySystemComponent()->MakeOutgoingSpec(AttackDamage, 1.0f, MyPlayer->GetAbilitySystemComponent()->MakeEffectContext());

            FGameplayEffectSpec* spec = specHandle.Data.Get();
            if (spec)
            {
                potentialEnemy->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);
            }
        }
    }

}

void ARTargetActor_DotUltimate::SetBetweenTwoPoints_Implementation(const FVector& start, const FVector& end, bool bigLaser)
{
    FVector Midpoint = (start + (end)) * 0.5f;

    FVector Direction = (end - start);
    float Length = Direction.Size();
    FRotator Rotation = UKismetMathLibrary::MakeRotFromZ(Direction);

    SetActorLocation(Midpoint);
    SetActorRotation(Rotation);

    CylinderHeight = Length;
    CylinderRadius = (bigLaser) ? 200.0f : 75.0f;

    // Scale the cylinder to match length (assuming default cylinder height = 200 units)
    float xyScale = (bigLaser) ? 4.0f : 1.5f ;
    FVector Scale = FVector(xyScale, xyScale, Length / 100.0f);
    CylinderMesh->SetWorldScale3D(Scale);
}
