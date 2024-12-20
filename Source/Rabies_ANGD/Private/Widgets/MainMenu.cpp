// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MainMenu.h"
#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"
#include "OnlineSessionSettings.h"

#include "Widgets/ConnectOnlineMenu.h"
#include "Widgets/RButton.h"

#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/SoftObjectPtr.h"

#include "Framework/EOSGameInstance.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/Overlay.h"
#include "Components/Button.h"

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	MultiplayerBtn->RabiesButton->OnClicked.AddDynamic(this, &UMainMenu::MultiplayerClicked);
	SingeplayerBtn->RabiesButton->OnClicked.AddDynamic(this, &UMainMenu::SingleplayerClicked);
	SettingsBtn->RabiesButton->OnClicked.AddDynamic(this, &UMainMenu::SettingsClicked);
	CreditsBtn->RabiesButton->OnClicked.AddDynamic(this, &UMainMenu::CreditsClicked);

	ReturnCredits->RabiesButton->OnClicked.AddDynamic(this, &UMainMenu::ReturnFromCredits);
	ReturnSettings->RabiesButton->OnClicked.AddDynamic(this, &UMainMenu::ReturnFromSettings);

	GameInst = GetGameInstance<UEOSGameInstance>();

	//GameInst->SessionJoined.AddUObject(this, &UMainMenu::MoveToCharacterSelect);

	ChangeMainMenuState(true);
	ChangeConnectMenuState(false);
	ChangeSettingsState(false);
	ChangeCreditsState(false);
}

void UMainMenu::ReturnFromCredits()
{
	ChangeMainMenuState(true);
	ChangeCreditsState(false);
}

void UMainMenu::ReturnFromSettings()
{
	ChangeMainMenuState(true);
	ChangeSettingsState(false);
}

void UMainMenu::SingleplayerClicked()
{
	FName levelName = FName(*FPackageName::ObjectPathToPackageName(GameLevel.ToString()));
	UGameplayStatics::OpenLevel(GetWorld(), levelName);
}

void UMainMenu::MultiplayerClicked()
{
	if (GameInst)
	{
		GameInst->Login();
	}
}

void UMainMenu::SettingsClicked()
{
	ChangeMainMenuState(false);
	ChangeSettingsState(true);
}

void UMainMenu::CreditsClicked()
{
	ChangeMainMenuState(false);
	ChangeCreditsState(true);
}

void UMainMenu::ChangeMainMenuState(bool state)
{
	if (state)
		MainMenuOverlay->SetVisibility(ESlateVisibility::Visible);
	else
		MainMenuOverlay->SetVisibility(ESlateVisibility::Hidden);
}

void UMainMenu::ChangeConnectMenuState(bool state)
{
	if (state)
		OnlineMenuLobby->SetVisibility(ESlateVisibility::Visible);
	else
		OnlineMenuLobby->SetVisibility(ESlateVisibility::Hidden);
}

void UMainMenu::ChangeSettingsState(bool state)
{
	if (state)
		SettingsOverlay->SetVisibility(ESlateVisibility::Visible);
	else
		SettingsOverlay->SetVisibility(ESlateVisibility::Hidden);
}

void UMainMenu::ChangeCreditsState(bool state)
{
	if (state)
		CreditsOverlay->SetVisibility(ESlateVisibility::Visible);
	else
		CreditsOverlay->SetVisibility(ESlateVisibility::Hidden);
}
