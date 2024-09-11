// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ConnectOnlineMenu.h"

#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"
#include "OnlineSessionSettings.h"

#include "Framework/EOSGameInstance.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Widgets/OpenLobby.h"

void UConnectOnlineMenu::NativeConstruct()
{
	Super::NativeConstruct();

	GameInst = GetGameInstance<UEOSGameInstance>();

	LoginButton->OnClicked.AddDynamic(this, &UConnectOnlineMenu::LoginButtonClicked);
	CreateSessionButton->OnClicked.AddDynamic(this, &UConnectOnlineMenu::CreateSessionButtonClicked);
	FindSessionsButton->OnClicked.AddDynamic(this, &UConnectOnlineMenu::FindSessionsButtonClicked);
	SessionNameText->OnTextChanged.AddDynamic(this, &UConnectOnlineMenu::SessionNameTextChanged);

	CreateSessionButton->SetIsEnabled(false);

	GameInst->SearchCompleted.AddUObject(this, &UConnectOnlineMenu::SessionSearchCompleted);
}

void UConnectOnlineMenu::SessionNameTextChanged(const FText& NewText)
{
	CreateSessionButton->SetIsEnabled(!NewText.IsEmpty());
}

void UConnectOnlineMenu::LoginButtonClicked()
{
	if (GameInst)
	{
		GameInst->Login();
	}
}

void UConnectOnlineMenu::CreateSessionButtonClicked()
{
	if (GameInst)
	{
		GameInst->CreateSession(FName{ SessionNameText->GetText().ToString() });
	}
}

void UConnectOnlineMenu::FindSessionsButtonClicked()
{
	if (GameInst)
	{
		GameInst->FindSession();
	}
}

void UConnectOnlineMenu::SessionSearchCompleted(const TArray<FOnlineSessionSearchResult>& searchResults)
{
	LobbyListScrollBox->ClearChildren();

	int index = 0;

	for (const FOnlineSessionSearchResult& searchResult : searchResults)
	{
		FString sessionName = GameInst->GetSessionName(searchResult);
		
		UOpenLobby* LobbyEntry = CreateWidget<UOpenLobby>(LobbyListScrollBox, OnlineLobbyClass);
		LobbyEntry->InitLobbyEntry(FName(sessionName), index);
		LobbyListScrollBox->AddChild(LobbyEntry);
		LobbyEntry->OnLobbyEntrySelected.AddDynamic(this, &UConnectOnlineMenu::LobbySelected);

		index++;
	}
}

void UConnectOnlineMenu::LobbySelected(int lobbyIndex)
{
	SelectedLobbyIndex = lobbyIndex;
}
