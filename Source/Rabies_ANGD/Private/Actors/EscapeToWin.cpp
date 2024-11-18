// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/EscapeToWin.h"
#include "Player/RPlayerBase.h"
#include "Components/SphereComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Widgets/CanEscape.h"
#include "Widgets/CannotEscape.h"
#include "Widgets/GameWinUI.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEscapeToWin::AEscapeToWin()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collider"));
	RootComponent = SphereCollider;
	SphereCollider->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AEscapeToWin::OnOverlapBegin);
	SphereCollider->OnComponentEndOverlap.AddDynamic(this, &AEscapeToWin::OnOverlapEnd);


}

// Called when the game starts or when spawned
void AEscapeToWin::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEscapeToWin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEscapeToWin::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bHasBeatenBoss)
	{
		UE_LOG(LogTemp, Warning, TEXT("You have defeated the boss! You can escape!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Door is locked, defeat the boss."));
	}
}

void AEscapeToWin::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AEscapeToWin::SetUpTrueUI()
{
	CanEscapeWidgetUI = Cast<UCanEscape>(EscapeWidgetComp->GetUserWidgetObject());
	CanEscapeWidgetUI->SetVisibility(ESlateVisibility::Hidden);
}

void AEscapeToWin::SetUpFalseUI()
{
	CannotEscapeWidgetUI = Cast<UCannotEscape>(EscapeWidgetComp->GetUserWidgetObject());
	CannotEscapeWidgetUI->SetVisibility(ESlateVisibility::Hidden);
}

void AEscapeToWin::SetUpEndUI()
{
	GameWinUI = Cast<UGameWinUI>(EscapeWidgetComp->GetUserWidgetObject());
	GameWinUI->SetVisibility(ESlateVisibility::Hidden);
}

bool AEscapeToWin::SetActivatingExit()
{
	bHasBeatenBoss = true;
	return bHasBeatenBoss;
}

void AEscapeToWin::EndGame()
{
}
