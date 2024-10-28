// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/EOSActionGameState.h"
#include "Net/UnrealNetwork.h"
#include "Framework/EOSPlayerState.h"
#include "Player/RPlayerController.h"

void AEOSActionGameState::BeginPlay()
{
	// preferably where we spawn in the chests

	// this will be on server side
}

void AEOSActionGameState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


}