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
	void ChangeMainMenuState(bool state);

	UFUNCTION()
	void ChangeConnectMenuState(bool state);

	UFUNCTION()
	void ChangeSettingsState(bool state);

	UFUNCTION()
	void ChangeCreditsState(bool state);
	
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
	class UButton* SettingsButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* CreditsButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TitleScreenText;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* MainMenuOverlay;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* SettingsOverlay;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* CreditsOverlay;

	UFUNCTION()
	void SingleplayerClicked();

	UFUNCTION()
	void MultiplayerClicked();

	UFUNCTION()
	void SettingsClicked();

	UFUNCTION()
	void CreditsClicked();
};
