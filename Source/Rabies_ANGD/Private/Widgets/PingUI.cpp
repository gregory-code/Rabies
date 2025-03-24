// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PingUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Framework/RItemDataAsset.h"

void UPingUI::SetChestCostText(int costAmount)
{
	if (CostText != nullptr)
	{
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Setting Cost Text")));
		FText Text = FText::Format(FText::FromString("{0}"), FText::AsNumber(costAmount));
		CostText->SetText(Text);

	}
}

void UPingUI::SetItemIcon(URItemDataAsset* data)
{
	if (ItemIcon == nullptr)
		return;

	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Setting Item Icon")));
	FSlateBrush brush;
	brush.SetResourceObject(data->ItemIcon);
	ItemIcon->SetBrush(brush);
}
