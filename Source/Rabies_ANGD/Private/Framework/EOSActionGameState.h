// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "EOSActionGameState.generated.h"

class AREnemyBase;
class AItemChest;
class AItemPickup;
class URItemDataAsset;

/**
 * 
 */
UCLASS()
class AEOSActionGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	UFUNCTION()
	void SelectEnemy(AREnemyBase* selectedEnemy, bool isDeadlock, bool bIsDeadlockComponent);

	UFUNCTION()
	void SelectChest(AItemChest* openedChest, int spawnCount);

	UFUNCTION()
	void SelectItem(AItemPickup* selectedItem, class ARPlayerBase* targetingPlayer);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void AwardEnemyKill(TSubclassOf<class UGameplayEffect> rewardEffect);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void StartBossFight(int enemyID);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void LeaveLevel();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestSpawnDotLaserMarks(UNiagaraSystem* SystemToSpawn, FVector SpawnLocation, FVector Direction, float ScorchSize);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestSpawnVFXOnCharacter(UNiagaraSystem* SystemToSpawn, class ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float otherValue);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestSpawnVFX(UNiagaraSystem* SystemToSpawn, FVector SpawnLocation, FVector Direction, float otherValue);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RequestSpawnVFX(UNiagaraSystem* SystemToSpawn, FVector SpawnLocation, FVector Direction, float otherValue);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RequestSpawnVFXOnCharacter(UNiagaraSystem* SystemToSpawn, class ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float otherValue);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AdjustFireOnCharacter(UNiagaraSystem* SystemToSpawn, class ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float otherValue);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AdjustIceOnCharacter(UNiagaraSystem* SystemToSpawn, class ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float otherValue);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AdjustIceOnCharacter(UNiagaraSystem* SystemToSpawn, class ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float otherValue);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShootTexUltimate(UNiagaraSystem* SystemToSpawn, class ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, FVector endPos);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RequestPlayAudio(USoundBase* Sound, FVector Location, FRotator Rotation, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, class USoundAttenuation* AttenuationSettings = nullptr);
private:

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnDotLaserMarks(UNiagaraSystem* SystemToSpawn, FVector SpawnLocation, FVector Direction, float ScorchSize);

	UPROPERTY(EditDefaultsOnly, Category = "Deadlock")
	FVector deadlockPos;

	FTimerHandle WaveHandle;

	void LoadMapAndListen(TSoftObjectPtr<UWorld> levelToLoad);

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> GameLevel;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> SelectLevel;


	UPROPERTY()
	class AEscapeToWin* EscapeToWin;

	UPROPERTY()
	float WaveTime;

	UPROPERTY()
	int WaveLevel;

	UFUNCTION()
	void WaveSpawn(float timeToNextWave);

	UFUNCTION()
	void SpawnEnemyWave(int amountOfEnemies);

	UPROPERTY(EditDefaultsOnly, Category = "Chest")
	int AmountOfChests;

	UPROPERTY(EditDefaultsOnly, Category = "Enemies")
	int enemyInitalSpawnRate;

	UPROPERTY(EditDefaultsOnly, Category = "Enemies")
	int enemyMax;

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SpawnChest(FVector SpawnLocation, bool bRareChest);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SpawnEnemy(int EnemyIDToSpawn, FVector SpawnLocation);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void OpenedChest(int chestID, int spawnCount);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void RemoveEnemy(int enemyID);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void PickedUpItem(int itemID, class ARPlayerBase* targetingPlayer);

	UPROPERTY(EditDefaultsOnly, Category = "Chest")
	TSubclassOf<AItemChest> ItemChestClass;

	UPROPERTY(EditDefaultsOnly, Category = "Chest")
	TSubclassOf<AItemPickup> ItemPickupClass;

	UPROPERTY(EditDefaultsOnly, Category = "Enemies")
	TArray<TSubclassOf<class AREnemyBase>> EnemyLibrary;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TArray<URItemDataAsset*> ItemLibrary;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TArray<URItemDataAsset*> ItemSelection;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	URItemDataAsset* KeyCard;


	UPROPERTY(Replicated)
	TArray<AItemChest*> AllChests;

	UPROPERTY()
	int MaxChests;

	UPROPERTY()
	bool bGottenKeyCard = false;

	UFUNCTION()
	bool GetKeyCard();

	UPROPERTY(Replicated)
	TArray<AItemPickup*> AllItems;

	UPROPERTY(Replicated)
	TArray<class AREnemyBase*> AllEnemies;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
