// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ConnectOnlineMenu.generated.h"

/**
 * 
 */
UCLASS()
class UConnectOnlineMenu : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	class UEOSGameInstance* GameInst;

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* LoginButton;

	UFUNCTION()
	void LoginButtonClicked();
};