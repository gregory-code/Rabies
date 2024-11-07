// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "EOSActionGameState.generated.h"

class AItemChest;

/**
 * 
 */
UCLASS()
class AEOSActionGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	UFUNCTION()
	void SelectChest(AItemChest* openedChest);

private:

	UPROPERTY(EditDefaultsOnly, Category = "Chest")
	int AmountOfChests;

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SpawnChest(FVector SpawnLocation);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SpawnEnemy(int EnemyIDToSpawn, FVector SpawnLocation);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void OpenedChest(int chestID);

	UPROPERTY(EditDefaultsOnly, Category = "Chest")
	TSubclassOf<class AItemChest> ItemChestClass;

	UPROPERTY(EditDefaultsOnly, Category = "Chest")
	TSubclassOf<class AItemPickup> ItemPickupClass;

	UPROPERTY(EditDefaultsOnly, Category = "Enemies")
	TArray<TSubclassOf<class AREnemyBase>> EnemyLibrary;

	UPROPERTY(Replicated)
	TArray<AItemChest*> AllChests;

	UPROPERTY(Replicated)
	TArray<class AREnemyBase*> AllEnemies;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
