// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/LobbyDisplay.h"
#include "Components/TextBlock.h"
#include "Framework/EOSGameState.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void ULobbyDisplay::NativeConstruct()
{
	Super::NativeConstruct();

	GameState = Cast<AEOSGameState>(UGameplayStatics::GetGameState(this));


	if (GameState)
	{
		FName lobbyName = GameState->GetSessionName();
		LobbyName->SetText(FText::FromName(lobbyName));
		GameState->OnSessionNameReplicated.AddDynamic(this, &ULobbyDisplay::SessionNameReplicated);
	}
}

void ULobbyDisplay::SessionNameReplicated(const FName& newSessionName)
{
	LobbyName->SetText(FText::FromName(newSessionName));
}
