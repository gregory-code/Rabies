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

void ARGameMode::PausingGame(bool SetPause)
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = SetPause;

	if (!PauseUI && PauseWidget)
	{
		PauseUI = CreateWidget(GetWorld(), PauseWidget);
	}

	if (PauseUI && SetPause)
	{
		UE_LOG(LogTemp, Warning, TEXT("I am Pausing"));
		PauseUI->AddToViewport();
	}
	else if (PauseUI && !SetPause)
	{
		UE_LOG(LogTemp, Warning, TEXT("I am Unpausing"));
		PauseUI->RemoveFromParent();
		PauseUI = nullptr;
	}
}

//void ARGameMode::UnpausingGame()
//{
//	UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = false;
//	/*UUserWidget* Widget = 
//	if (Widget)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("I am Unpausing"));
//		Widget->RemoveFromParent();
//	}*/
//}
