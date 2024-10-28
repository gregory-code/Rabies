// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ItemChest.h"
#include "Player/RPlayerBase.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AItemChest::AItemChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	ChestMesh = CreateDefaultSubobject<UStaticMeshComponent>("Chest Mesh");
	ChestMesh->SetupAttachment(GetRootComponent());
	ChestMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RootComponent = ChestMesh;
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

void AItemChest::OnOverlapBegin(AActor* overlappedActor, AActor* otherActor)
{
	ARPlayerBase* player = Cast<ARPlayerBase>(otherActor);
	if (!player)
	{
		return;
	}


}