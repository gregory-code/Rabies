// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/EscapeToWin.h"
#include "Player/RPlayerBase.h"
#include "Components/SphereComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Widgets/ActivationWidget.h"
#include "Widgets/AccessDeniedWidget.h"
#include "Widgets/InitiateBossFight.h"
//#include "Widgets/CanEscape.h"
//#include "Widgets/CannotEscape.h"
//#include "Widgets/GameWinUI.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	EndGameMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("End Game Mesh"));
	EndGameMesh->SetupAttachment(GetRootComponent());
	EndGameMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	ActivateWidgetComp = CreateDefaultSubobject<UWidgetComponent>("Activation Widget Comp");
	ActivateWidgetComp->SetupAttachment(GetRootComponent());

	AcessDeniedWidgetComp = CreateDefaultSubobject<UWidgetComponent>("Access Denied Widget Comp");
	AcessDeniedWidgetComp->SetupAttachment(GetRootComponent());

	InitiateBossWidgetComp = CreateDefaultSubobject<UWidgetComponent>("Initiate Boss Widget Comp");
	InitiateBossWidgetComp->SetupAttachment(GetRootComponent());

	//CanEscapeWidgetComp = CreateDefaultSubobject<UWidgetComponent>("Can Escape Widget Comp");
	//CanEscapeWidgetComp->SetupAttachment(GetRootComponent());

	//CannotEscapeWidgetComp = CreateDefaultSubobject<UWidgetComponent>("Cannot Escape Widget Comp");
	//CannotEscapeWidgetComp->SetupAttachment(GetRootComponent());

	//GameWinWidgetComp = CreateDefaultSubobject<UWidgetComponent>("Win Game Widget Comp");
	//GameWinWidgetComp->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AEscapeToWin::BeginPlay()
{
	Super::BeginPlay();

	SetUpActivation();
	SetUpAcessDenied();
	SetUpBossUI();
	//SetUpTrueUI();
	//SetUpFalseUI();
	//SetUpEndUI();
}

// Called every frame
void AEscapeToWin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEscapeToWin::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	player = Cast<ARPlayerBase>(OtherActor);

	if (!player)
	{
		return;
	}

	if (!bStartBoss && player->IsLocallyControlled())
	{
		UE_LOG(LogTemp, Warning, TEXT("Try to Escape?"));
		ActivationUI->SetVisibility(ESlateVisibility::Visible);

		player->PlayerInteraction.AddUObject(this, &AEscapeToWin::CheckKeyCard);
		return;
	}

	//if (bHasKeyCard && player->IsLocallyControlled()) //Add a check if they have the keycard item
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("You want to escape?"));

	//	player->PlayerInteraction.AddUObject(this, &AEscapeToWin::SpawnBoss);

	//	return;
	//}

	//if (bHasBeatenBoss && player->IsLocallyControlled())
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("You have defeated the boss! You can escape!"));
	//	CanEscapeWidgetComp->SetVisibility(true);

	//	player->PlayerInteraction.AddUObject(this, &AEscapeToWin::EndGame);
	//	
	//}
	//else if(player->IsLocallyControlled())
	//{
	//	UE_LOG(LogTemp, Error, TEXT("Door is locked, defeat the boss."));
	//	CannotEscapeWidgetComp->SetVisibility(true);
	//}
}

void AEscapeToWin::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	player = Cast<ARPlayerBase>(OtherActor);

	if (!player)
	{
		return;
	}

	if (!bStartBoss && player->IsLocallyControlled())
	{
		ActivationUI->SetVisibility(ESlateVisibility::Hidden);
		AccessDeniedUI->SetVisibility(ESlateVisibility::Hidden);

		player->PlayerInteraction.Clear();

		return;
	}

	//if (bHasKeyCard && player->IsLocallyControlled())
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Find the key"));
	//	player->PlayerInteraction.Clear();
	//	return;
	//}


	//if (bHasBeatenBoss)
	//{
	//	CanEscapeWidgetComp->SetVisibility(false);
	//	player->PlayerInteraction.Clear();
	//}
	//else
	//{
	//	CannotEscapeWidgetComp->SetVisibility(false);
	//}
}

void AEscapeToWin::SetUpActivation()
{
	ActivationUI = Cast<UActivationWidget>(ActivateWidgetComp->GetUserWidgetObject());
	ActivationUI->SetVisibility(ESlateVisibility::Hidden);
}

void AEscapeToWin::SetUpAcessDenied()
{
	AccessDeniedUI = Cast<UAccessDeniedWidget>(AcessDeniedWidgetComp->GetUserWidgetObject());
	AccessDeniedUI->SetVisibility(ESlateVisibility::Hidden);
}

void AEscapeToWin::SetUpBossUI()
{
	InitiateBossUI = Cast<UInitiateBossFight>(InitiateBossWidgetComp->GetUserWidgetObject());
	InitiateBossUI->SetVisibility(ESlateVisibility::Hidden);
}

void AEscapeToWin::CheckKeyCard()
{
	//Check to see if player has keycard
	ActivationUI->SetVisibility(ESlateVisibility::Hidden);

	bHasKeyCard = true;

	if (bHasKeyCard == true)
	{
		SpawnBoss();
	}
	else
	{
		AccessDeniedUI->SetVisibility(ESlateVisibility::Visible);
	}
}

void AEscapeToWin::SpawnBoss()
{
	UE_LOG(LogTemp, Error, TEXT("Spawning Boss"));
	bStartBoss = true;

	player->PlayerInteraction.Clear();

	InitiateBossUI->SetVisibility(ESlateVisibility::Visible);
	
	//Spawn Boss into the world
}

//void AEscapeToWin::SetUpTrueUI()
//{
//	CanEscapeWidgetUI = Cast<UCanEscape>(CanEscapeWidgetComp->GetUserWidgetObject());
//	CanEscapeWidgetComp->SetVisibility(false);
//}
//
//void AEscapeToWin::SetUpFalseUI()
//{
//	CannotEscapeWidgetUI = Cast<UCannotEscape>(CannotEscapeWidgetComp->GetUserWidgetObject());
//	CannotEscapeWidgetComp->SetVisibility(false);
//}
//
//void AEscapeToWin::SetUpEndUI()
//{
//	GameWinUI = Cast<UGameWinUI>(GameWinWidgetComp->GetUserWidgetObject());
//	GameWinWidgetComp->SetVisibility(false);
//}
//
//
//
//bool AEscapeToWin::SetActivatingExit()
//{
//	bHasBeatenBoss = true;
//	return bHasBeatenBoss;
//}
//
//void AEscapeToWin::EndGame()
//{
//	UE_LOG(LogTemp, Error, TEXT("You Win!!!"));
//	bHasWonGame = true;
//
//	CanEscapeWidgetComp->SetVisibility(false);
//	player->PlayerInteraction.Clear();
//
//	GameWinWidgetComp->SetVisibility(true);
//}
