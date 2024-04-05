// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "RSessionSubsystem.generated.h"

/**
 * //https://forums.unrealengine.com/t/syntax-error-declaring-dynamic-multicast-delegate/679364

 */

//DECLARE_MULTICAST_DELEGATE_OneParam(FOnCreateSessionComplete, bool Successful);

UCLASS()
class URSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	URSessionSubsystem();

	void CreateSession(int32 NumPublicConnections, bool IsLANMatch);

	//FOnCreateSessionComplete OnCreateSessionCompleteEvent;

protected:
	void OnCreateSessionCompleted(FName SessionName, bool Successful);

private:
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
};
