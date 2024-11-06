// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ItemChest.h"
#include "Player/RPlayerBase.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AItemChest::AItemChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	ChestBottomMesh = CreateDefaultSubobject<UStaticMeshComponent>("Chest Bottom Mesh");
	ChestBottomMesh->SetupAttachment(GetRootComponent());
	ChestBottomMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	RootComponent = ChestBottomMesh;
	
	ChestTopMesh = CreateDefaultSubobject<UStaticMeshComponent>("Chest Top Mesh");
	ChestTopMesh->SetupAttachment(GetRootComponent());
	ChestTopMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	// sphere radius
	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collider"));
	SphereCollider->SetupAttachment(GetRootComponent());
	SphereCollider->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AItemChest::OnOverlapBegin);
	SphereCollider->OnComponentEndOverlap.AddDynamic(this, &AItemChest::OnOverlapEnd);
}

// Called when the game starts or when spawned
void AItemChest::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AItemChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemChest::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARPlayerBase* player = Cast<ARPlayerBase>(OtherActor); // save player
	if (!player)
	{
		return;
	}
	//player->Attribute
	player->PlayerInteraction.AddUObject(this, &AItemChest::Interact);


	if (InteractionWidget)
	{
		SetUpUI(true);
	}

}

void AItemChest::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ARPlayerBase* player = Cast<ARPlayerBase>(OtherActor);
	if (!player)
	{
		return;
	}

	if (InteractionWidget)
	{
		SetUpUI(false);
	}

	//player->PlayerInteraction.Remove(FOnPlayerInteraction); // figure out how to remove UObject so that it doesn't interact when not in range.

}

void AItemChest::SetUpUI(bool SetInteraction)
{
	if (!WidgetInstance && InteractionWidget)
	{
		WidgetInstance = CreateWidget<UUserWidget>(GetWorld(), InteractionWidget);
	}

	if (WidgetInstance && SetInteraction)
	{
		WidgetInstance->AddToViewport();
	}
	else if (WidgetInstance && !SetInteraction)
	{
		WidgetInstance->RemoveFromParent();
	}
}

void AItemChest::Interact()
{
	Destroy();
	// this is where you'd check how much money they have and then if they have enough send the RPC to the server to get item
}
