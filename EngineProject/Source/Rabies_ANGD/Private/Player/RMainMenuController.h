// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RMainMenuController.generated.h"

/**
 * 
 */
UCLASS()
class ARMainMenuController : public APlayerController
{
	GENERATED_BODY()

public:
	ARMainMenuController();

protected:
	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnRep_PlayerState"))
	void BP_OnRep_PlayerState();
	
public:
	//this function calls only on the server
	virtual void OnPossess(APawn* NewPawn) override;

	//this function calls on both the listening server, and the client.
	virtual void AcknowledgePossession(APawn* NewPawn) override;

	virtual void BeginPlay() override;

	UFUNCTION()
	void ChangeOnlineMenuState(bool state);

	UFUNCTION()
	void ChangeMainMenuState(bool state);

	UFUNCTION()
	void OpenLoadingScreen(int whichLoadingScreen);

	void CreateLoadingScreenUI();

private:
	class ACineCameraActor* CineCamera;

	//class ALevelSequenceActor* MainMenuSequence;

	void PostPossessionSetup(APawn* NewPawn);

	void JoinedSession();

	void CreateMenuUI();

	UFUNCTION()
	void OnSequenceEnd();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UMainMenu> MenuUIClass;

	UPROPERTY()
	class UMainMenu* MenuUI;

	UPROPERTY()
	class URCharacterDefination* CurrentlyHoveredCharacter;

	UPROPERTY()
	class AEOSGameState* GameState;

	int myPlayerID;

	void EnableLights(UWorld* world, FName Tag);

	void EnableBacklights();

	FTimerHandle BacklightTimerHandle;

	void EnableFrontlights();

	FTimerHandle FrontlightTimerHandle;

	void EnableComputer();

	FTimerHandle FinalTimerHandle;

	UPROPERTY(EditAnywhere, Category = "ComputerMaterial")
	UMaterialInterface* ComputerMaterial;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* AudioComp;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* LightsOnAudio;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* MonitorLightAudio;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ULoadingScreenUI> LoadingScreenUIClass;

	UPROPERTY()
	class ULoadingScreenUI* LoadingScreenUI;
};
