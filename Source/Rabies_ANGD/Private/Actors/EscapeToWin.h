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

protected:
	bool bHasBeatenBoss = false;

	UPROPERTY(EditDefaultsOnly, Category = "Escape")
	class USphereComponent* SphereCollider;


private:
	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* EscapeWidgetComp;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UCannotEscape* CannotEscapeWidgetUI;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UCanEscape* CanEscapeWidgetUI;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UGameWinUI* GameWinUI;

	//UI for the false bHasBeatenBoss

	//UI for the true bHasBeatenBoss

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

	UFUNCTION()
	void SetUpTrueUI();

	UFUNCTION()
	void SetUpFalseUI();

	UFUNCTION()
	void SetUpEndUI();

	UFUNCTION()
	bool SetActivatingExit();

	UFUNCTION()
	void EndGame();


};
