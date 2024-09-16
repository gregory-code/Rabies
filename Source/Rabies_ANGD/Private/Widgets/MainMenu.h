// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class UMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION()
	void ChangeConnectMenuState(bool state);
	
protected:

	virtual void NativeConstruct() override;

	class UEOSGameInstance* GameInst;

private:

	UPROPERTY(meta = (BindWidget))
	class UConnectOnlineMenu* OnlineMenu;

	UPROPERTY(meta = (BindWidget))
	class UButton* SingeplayerButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* MultiplayerButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TitleScreenText;

	UFUNCTION()
	void SingleplayerClicked();

	UFUNCTION()
	void MultiplayerClicked();
};
