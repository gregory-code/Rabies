// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "EOSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class UEOSGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void Login();

	UFUNCTION(BlueprintCallable)
	void CreateSession(const FName& SessionName);

	UFUNCTION(BlueprintCallable)
	void FindSession();

	const FName& GetSessionNameKey() const { return SessionNameKey; }

protected:
	virtual void Init() override;

private:
	class IOnlineSubsystem* onlineSubsystem;
	IOnlineIdentityPtr identityPtr;
	IOnlineSessionPtr sessionPtr;
	TSharedPtr<class FOnlineSessionSearch> sessionSearch;

	void LoginCompleted(int numOfPlayers, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
	void CreateSessionCompleted(FName SessionName, bool bWasSuccessful);
	void FindSessionsCompleted(bool bWasSuccessful);

	FName SessionNameKey{ "SessionName" };
};
