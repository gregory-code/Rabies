// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/EOSGameInstance.h"
#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"
#include "OnlineSessionSettings.h"

void UEOSGameInstance::Login()
{
	if (identityPtr)
	{
		FOnlineAccountCredentials onlineAccountCredentials;
		onlineAccountCredentials.Type = "accountportal";
		onlineAccountCredentials.Id = "";
		onlineAccountCredentials.Token = "";
		identityPtr->Login(0, onlineAccountCredentials);
	}
}

void UEOSGameInstance::CreateSession(const FName& SessionName)
{
	UE_LOG(LogTemp, Warning, TEXT("Trying to create"));
	if (sessionPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Creating Session"));
		FOnlineSessionSettings SessionSettings;
		SessionSettings.bIsDedicated = false;
		SessionSettings.bIsLANMatch = false;
		SessionSettings.bAllowInvites = true;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bAllowJoinInProgress = true;
		SessionSettings.bAllowJoinViaPresence = true;
		SessionSettings.bUseLobbiesIfAvailable = true;
		SessionSettings.NumPublicConnections = 10;
		SessionSettings.Set(GetSessionNameKey(), SessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		sessionPtr->CreateSession(0, SessionName, SessionSettings);
	}
}

void UEOSGameInstance::FindSession()
{
	if (sessionPtr)
	{
		sessionSearch = MakeShareable(new FOnlineSessionSearch);

		sessionSearch->bIsLanQuery = false;
		sessionSearch->MaxSearchResults = 100;
		sessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

		sessionPtr->FindSessions(0, sessionSearch.ToSharedRef());
	}
}

void UEOSGameInstance::Init()
{
	Super::Init();

	onlineSubsystem =  IOnlineSubsystem::Get();
	identityPtr = onlineSubsystem->GetIdentityInterface();
	identityPtr->OnLoginCompleteDelegates->AddUObject(this, &UEOSGameInstance::LoginCompleted);

	sessionPtr = onlineSubsystem->GetSessionInterface();
	sessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::CreateSessionCompleted);

	sessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOSGameInstance::FindSessionsCompleted);
}

void UEOSGameInstance::LoginCompleted(int numOfPlayers, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Logged in"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to Login"));
	}


}

void UEOSGameInstance::CreateSessionCompleted(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Logged into session"));
	}
}

void UEOSGameInstance::FindSessionsCompleted(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		for (const FOnlineSessionSearchResult& lobbyFound : sessionSearch->SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found sess: %s"), *lobbyFound.GetSessionIdStr());
			lobbyFound.GetSessionIdStr();
		}
	}
}

