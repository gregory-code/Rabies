// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyDisplay.generated.h"

/**
 * 
 */
UCLASS()
class ULobbyDisplay : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LobbyName;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY()
	class AEOSGameState* GameState;

	UFUNCTION()
	void SessionNameReplicated(const FName& newSessionName);
};
