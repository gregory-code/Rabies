// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PingActor.generated.h"

class URItemDataAsset;
UCLASS()
class APingActor : public AActor
{
	GENERATED_BODY()

private:

	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* PingUIWidgetComp;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UPingUI> PingUIClass;

	UPROPERTY()
	class UPingUI* PingUI;

	FTimerHandle DestroyTimer;

	UFUNCTION()
	void TimedDestroy();
	
public:	
	// Sets default values for this actor's properties
	APingActor();

public:
	void SetChestCostText(int costAmount);

	void SetItemIcon(URItemDataAsset* data);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
