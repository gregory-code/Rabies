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
