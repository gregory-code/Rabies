// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/RCenterChooseButton.h"

void ARCenterChooseButton::OnActorClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("CenterButton clicked!"));
	}
}

void ARCenterChooseButton::NotifyActorOnClicked(FKey ButtonPressed)
{
	OnActorClicked(this, ButtonPressed);
}
