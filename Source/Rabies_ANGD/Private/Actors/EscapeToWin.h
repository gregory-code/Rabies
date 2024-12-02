// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EscapeToWin.generated.h"

UCLASS()
class AEscapeToWin : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEscapeToWin();

	bool bHasBeatenBoss = false;

protected:
	bool bStartBoss = false;
	bool bHasKeyCard = false;
	bool bHasWonGame = false;

	UPROPERTY(EditDefaultsOnly, Category = "Escape")
	class USphereComponent* SphereCollider;

	UPROPERTY(EditDefaultsOnly, Category = "Escape")
	class UStaticMeshComponent* EndGameMesh;


private:

	/*	Widget Compositions	 */
	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* GameWinWidgetComp;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* EndGameWidgetComp;

	/*	Widgets	 */
	UPROPERTY()
	class UGameWinUI* GameWinUI;

	UPROPERTY()
	class UEndGameWidget* EndGameUI;

	//Transient - Reference to the widget

	class ARPlayerBase* player;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Chest Detail")
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Chest Detail")
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/*	UI Setup  */
	UFUNCTION()
	void SetUpEndUI();

	UFUNCTION()
	void SetUpEndGame();

	UFUNCTION()
	void CheckKeyCard();

	UFUNCTION()
	void SpawnBoss();

	UFUNCTION()
	void UseKeycard();

	UFUNCTION()
	void SetActivatingExit();

	UFUNCTION()
	void EndGame();


};
