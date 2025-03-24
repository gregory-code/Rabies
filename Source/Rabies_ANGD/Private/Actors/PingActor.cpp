// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PingActor.h"
#include "Widgets/PingUI.h"
#include "Actors/ItemChest.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Animation/AnimInstance.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Components/SphereComponent.h"

#include "Net/UnrealNetwork.h"

#include "Actors/ItemPickup.h"
#include "Components/WidgetComponent.h"

#include "Framework/RGameMode.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SceneComponent.h"

// Sets default values
APingActor::APingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;


	PingUIWidgetComp = CreateDefaultSubobject<UWidgetComponent>("Ping UI");
	RootComponent = PingUIWidgetComp;
	//PingUIWidgetComp->SetupAttachment(GetRootComponent());
}

void APingActor::SetChestCostText(int costAmount)
{
	if (PingUI)
	{	
		GetWorld()->GetTimerManager().ClearTimer(DestroyTimer);
		GetWorldTimerManager().SetTimer(DestroyTimer, this, &APingActor::TimedDestroy, 8.0f, true);
		PingUI->SetChestCostText(costAmount);
	}
}

void APingActor::SetItemIcon(URItemDataAsset* data)
{
	if (PingUI)
	{
		GetWorld()->GetTimerManager().ClearTimer(DestroyTimer);
		GetWorldTimerManager().SetTimer(DestroyTimer, this, &APingActor::TimedDestroy, 8.0f, true);
		PingUI->SetItemIcon(data);
	}
}

// Called when the game starts or when spawned
void APingActor::BeginPlay()
{
	Super::BeginPlay();
	
	PingUIWidgetComp->SetWidgetClass(PingUIClass);
	PingUI = CreateWidget<UPingUI>(GetWorld(), PingUIWidgetComp->GetWidgetClass());
	if (PingUI)
	{
		PingUIWidgetComp->SetWidget(PingUI);
	}

	//PingUIWidgetComp->SetWidgetClass(PingUIClass);
	//PingUI = CreateWidget<UPingUI>(GetWorld(), PingUIWidgetComp->GetWidgetClass());

	GetWorldTimerManager().SetTimer(DestroyTimer, this, &APingActor::TimedDestroy, 1.5f, true);

}

void APingActor::TimedDestroy()
{
	if (IsValid(this)) // Ensure the actor is valid before destroying it
	{
		Destroy();
	}
}

// Called every frame
void APingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

