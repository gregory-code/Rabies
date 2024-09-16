// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MainMenu.h"
#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"
#include "OnlineSessionSettings.h"

#include "Widgets/ConnectOnlineMenu.h"

#include "Framework/EOSGameInstance.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	MultiplayerButton->OnClicked.AddDynamic(this, &UMainMenu::MultiplayerClicked);
	SingeplayerButton->OnClicked.AddDynamic(this, &UMainMenu::SingleplayerClicked);

	GameInst = GetGameInstance<UEOSGameInstance>();

	OnlineMenu->SetVisibility(ESlateVisibility::Hidden);
}

void UMainMenu::SingleplayerClicked()
{

}

void UMainMenu::MultiplayerClicked()
{
	if (GameInst)
	{
		GameInst->Login();
	}
}

void UMainMenu::ChangeConnectMenuState(bool state)
{
	if (state)
	{
		OnlineMenu->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		OnlineMenu->SetVisibility(ESlateVisibility::Hidden);
	}
}