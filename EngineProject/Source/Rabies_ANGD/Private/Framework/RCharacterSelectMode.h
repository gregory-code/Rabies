// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/RabiesGameModeBase.h"
#include "RCharacterSelectMode.generated.h"


/**
 * 
 */
UCLASS()
class ARCharacterSelectMode : public ARabiesGameModeBase
{
	GENERATED_BODY()
	
protected:
	virtual void InitGameState() override;


private:

	UPROPERTY(EditDefaultsOnly, Category = "CagedCharacter")
	FVector ShownCage;

	UPROPERTY(EditDefaultsOnly, Category = "CagedCharacter")
	FVector OffScreen;

	UPROPERTY(EditDefaultsOnly, Category = "CagedCharacter")
	FVector Sideline;
};
