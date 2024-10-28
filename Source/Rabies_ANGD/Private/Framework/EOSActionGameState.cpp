// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/EOSActionGameState.h"
#include "Net/UnrealNetwork.h"
#include "Actors/ItemChest.h"
#include "Framework/EOSPlayerState.h"
#include "Player/RPlayerController.h"

void AEOSActionGameState::BeginPlay()
{
	// preferably where we spawn in the chests

	// this will be on server side

    SpawnChest(FVector(0, 0, 0));
}

void AEOSActionGameState::SpawnChest_Implementation(FVector SpawnLocation)
{
    if (HasAuthority() && ItemChestClass != nullptr) // Ensure we're on the server
    {
        FActorSpawnParameters SpawnParams;
        AItemChest* newChest = GetWorld()->SpawnActor<AItemChest>(ItemChestClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
        AllChests.Add(newChest); // make sure that the chest has bReplicates to true
        // You can do additional setup for NewActor if necessary
    }
}

void AEOSActionGameState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(AEOSActionGameState, AllChests, COND_None);
}