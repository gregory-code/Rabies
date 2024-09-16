// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RButton.generated.h"

/**
 * 
 */
UCLASS()
class URButton : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnHoverButton();

	UFUNCTION()
	void OnUnhoverButton();

	class UCanvasPanelSlot* RabiesCanvasSlot;

public:

	UPROPERTY(EditDefaultsOnly, Category = "Rabies_Style")
	FVector2D DefaultScale;

	UPROPERTY(EditDefaultsOnly, Category = "Rabies_Style")
	float DefaultRotation;

	UPROPERTY(EditDefaultsOnly, Category = "Rabies_Style")
	float HoverScale;

	UPROPERTY(EditDefaultsOnly, Category = "Rabies_Style")
	float HoverRotation;

	UPROPERTY(EditDefaultsOnly, Category = "Rabies_Style")
	FText ButtonsText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RabiesText;

	UPROPERTY(meta = (BindWidget))
	class UButton* RabiesButton;

	UPROPERTY(EditDefaultsOnly, Category = "Rabies_ColorStyle")
	FSlateColor NormalColor;

	UPROPERTY(EditDefaultsOnly, Category = "Rabies_ColorStyle")
	FSlateColor HoveredColor;

	UPROPERTY(EditDefaultsOnly, Category = "Rabies_ColorStyle")
	FSlateColor PressedColor;

	UPROPERTY(EditDefaultsOnly, Category = "Rabies_ColorStyle")
	FSlateColor BorderColor;
};
