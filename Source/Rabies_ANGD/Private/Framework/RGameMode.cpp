// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/RGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

void ARGameMode::GameOver()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;

	if (IsValid(DeathWidget))
	{
		UUserWidget* Widget = CreateWidget(GetWorld(), DeathWidget);
		if (Widget)
		{
			Widget->AddToViewport();
		}
	}
}

void ARGameMode::PausingGame()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;
	UUserWidget* Widget = CreateWidget(GetWorld(), PauseWidget);
	if (Widget)
	{
		Widget->AddToViewport();
	}
}

void ARGameMode::UnpausingGame()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = false;
	UUserWidget* Widget = CreateWidget(GetWorld(), PauseWidget);
	if (Widget)
	{
		Widget->RemoveFromViewport();
	}
}
