// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ConnectOnlineMenu.h"
#include "Framework/EOSGameInstance.h"
#include "Components/Button.h"

void UConnectOnlineMenu::NativeConstruct()
{
	Super::NativeConstruct();

	GameInst = GetGameInstance<UEOSGameInstance>();

	LoginButton->OnClicked.AddDynamic(this, &UConnectOnlineMenu::LoginButtonClicked);
}

void UConnectOnlineMenu::LoginButtonClicked()
{
	if (GameInst)
	{
		GameInst->Login();
	}
}
