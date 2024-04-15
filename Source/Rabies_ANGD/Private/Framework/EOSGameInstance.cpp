// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/EOSGameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"

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

void UEOSGameInstance::Init()
{
	Super::Init();

	onlineSubsystem =  IOnlineSubsystem::Get();
	identityPtr = onlineSubsystem->GetIdentityInterface();
	identityPtr->OnLoginCompleteDelegates->AddUObject(this, &UEOSGameInstance::LoginCompleted);
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

