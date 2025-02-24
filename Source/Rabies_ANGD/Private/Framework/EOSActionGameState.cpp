// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/EOSActionGameState.h"
#include "Enemy/REnemyBase.h"
#include "Net/UnrealNetwork.h"
#include "Actors/ItemChest.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RPlayerBase.h"
#include "DisplayDebugHelpers.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Actors/EscapeToWin.h"
#include "Actors/ItemPickup.h"
#include "Character/RCharacterBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Animation/AnimMontage.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/PlayerState.h"
#include "Actors/ChestSpawnLocation.h"
#include "Framework/EOSPlayerState.h"
#include "Player/RPlayerController.h"
#include "Actors/EnemySpawnLocation.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

void AEOSActionGameState::BeginPlay()
{
    Super::BeginPlay();
	// this will be on server side

    if (HasAuthority() == false)
        return;

    TArray<AActor*> EscapeToWinObj;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEscapeToWin::StaticClass(), EscapeToWinObj);
    EscapeToWin = Cast<AEscapeToWin>(EscapeToWinObj[0]);
    if (EscapeToWin)
    {
        EscapeToWin->SetOwner(this);
    }

    TArray<AActor*> spawnLocations;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AChestSpawnLocation::StaticClass(), spawnLocations);

    if (spawnLocations.Num() <= AmountOfChests) return;
    MaxChests = AmountOfChests;

    for (int i = 0; i < AmountOfChests; i++)
    {
        float randomSpawn = FMath::RandRange(0, spawnLocations.Num() - 1);
        AChestSpawnLocation* thisSpawn = Cast<AChestSpawnLocation>(spawnLocations[randomSpawn]);

        if (thisSpawn != nullptr)
        {
            SpawnChest(thisSpawn->GetActorLocation(), thisSpawn->bRareChest);
            spawnLocations.RemoveAt(randomSpawn);
        }
    }


    WaveLevel = 0;
    WaveTime = enemyInitalSpawnRate;
    WaveHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &AEOSActionGameState::WaveSpawn, 0.0f));
}

void AEOSActionGameState::SpawnChest_Implementation(FVector SpawnLocation, bool bRareChest)
{
    if (HasAuthority() && ItemChestClass != nullptr) // Ensure we're on the server
    {
        FActorSpawnParameters SpawnParams;
        AItemChest* newChest = GetWorld()->SpawnActor<AItemChest>(ItemChestClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
        newChest->SetOwner(this);
        if (bRareChest)
        {
            newChest->ScrapPrice = 70.0f;
        }
        AllChests.Add(newChest); // make sure that the chest has bReplicates to true
    }
}

void AEOSActionGameState::SpawnEnemy_Implementation(int EnemyIDToSpawn, FVector SpawnLocation)
{
    if (HasAuthority()) // Ensure we're on the server
    {
        FActorSpawnParameters SpawnParams;
        AREnemyBase* newEnemy = GetWorld()->SpawnActor<AREnemyBase>(EnemyLibrary[EnemyIDToSpawn], SpawnLocation, FRotator::ZeroRotator, SpawnParams);
        if (newEnemy == nullptr || this == nullptr)
        {
            
            return;
        }
        newEnemy->SetOwner(this);             
        UAbilitySystemComponent* ASC = newEnemy->GetAbilitySystemComponent();
        ASC->SetOwnerActor(newEnemy);
        newEnemy->InitLevel(WaveLevel);
        AllEnemies.Add(newEnemy); // make sure that the enemies has bReplicates to true
    }
}

void AEOSActionGameState::SelectEnemy(AREnemyBase* selectedEnemy, bool isDeadlock, bool bIsDeadlockComponent)
{
    if(isDeadlock)
        deadlockPos = selectedEnemy->GetActorLocation();

    if (bIsDeadlockComponent)
        EscapeToWin->UpdateEscapeToWin();

    for (int i = 0; i < AllEnemies.Num(); i++)
    {
        if (selectedEnemy == AllEnemies[i])
        {
            RemoveEnemy(i);
            break;
        }
    }

    if (isDeadlock)
    {
        StartBossFight(1);
        StartBossFight(2);
    }
}


void AEOSActionGameState::SelectChest(AItemChest* openedChest, int spawnCount)
{
    for (int i = 0; i < AllChests.Num(); i++)
    {
        if (openedChest == AllChests[i])
        {
            OpenedChest(i, spawnCount);
            return;
        }
    }
}

void AEOSActionGameState::SelectItem(AItemPickup* selectedItem, ARPlayerBase* targetingPlayer)
{
    for (int i = 0; i < AllItems.Num(); i++)
    {
        if (selectedItem == AllItems[i])
        { 
            PickedUpItem(i, targetingPlayer);
            return;
        }
    }
}

void AEOSActionGameState::LeaveLevel_Implementation()
{
    LoadMapAndListen(SelectLevel);
}

void AEOSActionGameState::AwardEnemyKill_Implementation(TSubclassOf<class UGameplayEffect> rewardEffect)
{
    if (HasAuthority())
    {
        for (APlayerState* playerState : PlayerArray)
        {
            if (playerState && playerState->GetPawn())
            {
                ARPlayerBase* player = Cast<ARPlayerBase>(playerState->GetPawn());
                if (player)
                {
                    player->GetAbilitySystemComponent()->ApplyGameplayEffectToSelf(rewardEffect.GetDefaultObject(), 1.0f, player->GetAbilitySystemComponent()->MakeEffectContext());
                }
            }
        }
    }
}

void AEOSActionGameState::WaveSpawn(float timeToNextWave)
{
    if (timeToNextWave >= WaveTime)
    {
        WaveLevel += PlayerArray.Num();
        int enemiesToSpawn = WaveLevel * 3;
        enemiesToSpawn = FMath::Clamp(enemiesToSpawn, 3, 6);
        SpawnEnemyWave(enemiesToSpawn);
        WaveHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &AEOSActionGameState::WaveSpawn, 0.0f));
    }
    else
    {
        timeToNextWave += GetWorld()->GetDeltaSeconds();
        WaveHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &AEOSActionGameState::WaveSpawn, timeToNextWave));
    }
}

void AEOSActionGameState::SpawnEnemyWave(int amountOfEnemies)
{
    TArray<AActor*> spawnLocations;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawnLocation::StaticClass(), spawnLocations);

    for (int i = 0; i < amountOfEnemies; i++)
    {
        if (AllEnemies.Num() >= enemyMax)
            continue;

        float randomSpawn = FMath::RandRange(0, spawnLocations.Num() - 1);
        SpawnEnemy(4, spawnLocations[randomSpawn]->GetActorLocation());
        spawnLocations.RemoveAt(randomSpawn);
    }
}

void AEOSActionGameState::StartBossFight_Implementation(int enemyID)
{
    if (enemyID == 1)
        deadlockPos.X += 300;

    SpawnEnemy(enemyID, deadlockPos);
    
    for (APlayerState* playerState : PlayerArray)
    {
        if (playerState/* && playerState->GetOwningController()*/)
        {
            Cast<AEOSPlayerState>(playerState)->Server_CreateBossHealth(WaveLevel, AllEnemies.Last());
        }
    }
}

void AEOSActionGameState::RemoveEnemy_Implementation(int enemyID)
{
    if (HasAuthority())
    {
        AllEnemies[enemyID]->UpdateEnemyDeath();
        AllEnemies.RemoveAt(enemyID);
    }
}

void AEOSActionGameState::PickedUpItem_Implementation(int itemID, ARPlayerBase* targetingPlayer)
{
    if (HasAuthority()) // Ensure we're on the server
    {
        if (AllItems[itemID]->ItemAsset == KeyCard)
        {
            for (APlayerState* playerState : PlayerArray)
            {
                if (playerState && playerState->GetPawn())
                {
                    ARPlayerBase* player = Cast<ARPlayerBase>(playerState->GetPawn());
                    if (player)
                        player->AddNewItem(AllItems[itemID]->ItemAsset);
                }
            }
        }
        else
        {
            targetingPlayer->AddNewItem(AllItems[itemID]->ItemAsset);
        }
        AllItems[itemID]->UpdateItemPickedup();
        AllItems.RemoveAt(itemID);
    }
}

void AEOSActionGameState::OpenedChest_Implementation(int chestID, int spawnCount)
{
    if (HasAuthority()) // Ensure we're on the server
    {
        AllChests[chestID]->UpdateChestOpened();

        for (int i = 0; i < spawnCount; i++)
        {
            URItemDataAsset* newData = ItemLibrary[0];

            if (ItemSelection.IsEmpty() == false)
            {
                int32 randomIndex = FMath::RandRange(0, ItemSelection.Num() - 1);
                newData = ItemSelection[randomIndex];
            }
            else
            {
                int32 randomIndex = FMath::RandRange(0, ItemLibrary.Num() - 1);
                newData = ItemLibrary[randomIndex];
            }

            if (GetKeyCard())
                newData = KeyCard;

            FActorSpawnParameters SpawnParams;
            FVector spawnAdjustment = AllChests[chestID]->GetActorLocation();
            spawnAdjustment.Z += 40;
            AItemPickup* newitem = GetWorld()->SpawnActor<AItemPickup>(ItemPickupClass, spawnAdjustment, FRotator::ZeroRotator, SpawnParams);
            AllItems.Add(newitem); // make sure that the chest has bReplicates to true]
            newitem->SetOwner(this);
            float x = FMath::RandRange(-100, 100);
            float y = FMath::RandRange(-100, 100);
            FVector ForceDirection = FVector(x, y, 600.f); // Adjust the Z value to set how much it pops up
            newitem->SetOwner(this);
            newitem->SetupItem(newData, ForceDirection);
        }
        AllChests.RemoveAt(chestID);
    }
}

bool AEOSActionGameState::GetKeyCard()
{
    if (MaxChests <= 0.0f)
        return false;

    if (bGottenKeyCard)
        return false;

    float percentage = ((float)AllChests.Num() / (float)MaxChests) * 90.0f;

    if (FMath::RandRange(0, 100) >= percentage)
    {
        bGottenKeyCard = true;
        return true;
    }

    return false;
}


void AEOSActionGameState::Multicast_ShootTexUltimate_Implementation(UNiagaraSystem* SystemToSpawn, ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, FVector endPos)
{
    if (!SystemToSpawn) return;
    if (!characterAttached) return;

    UNiagaraComponent* SpawnSystemAttached = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SystemToSpawn, SpawnLocation, FRotator::ZeroRotator, FVector(1.0f), true, true);

    //UNiagaraComponent* SpawnSystemAttached = UNiagaraFunctionLibrary::SpawnSystemAttached(SystemToSpawn, characterAttached->GetMesh(), NAME_None, SpawnLocation, FRotator::ZeroRotator, EAttachLocation::KeepWorldPosition, true, true, ENCPoolMethod::None, true);

    if (SpawnSystemAttached)
    {
        SpawnSystemAttached->SetVectorParameter(FName("BeamEnd"), endPos);
    }
}

void AEOSActionGameState::LoadMapAndListen(TSoftObjectPtr<UWorld> levelToLoad)
{
    if (!levelToLoad.IsValid())
    {
        levelToLoad.LoadSynchronous();
    }

    if (levelToLoad.IsValid())
    {
        const FName levelName = FName(*FPackageName::ObjectPathToPackageName(levelToLoad.ToString()));
        GetWorld()->ServerTravel(levelName.ToString() + "?listen");
    }
}

void AEOSActionGameState::Server_RequestSpawnVFXOnCharacter_Implementation(UNiagaraSystem* SystemToSpawn, ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float otherValue)
{
    Multicast_RequestSpawnVFXOnCharacter(SystemToSpawn, characterAttached, SpawnLocation, Direction, otherValue);
}

bool AEOSActionGameState::Server_RequestSpawnVFXOnCharacter_Validate(UNiagaraSystem* SystemToSpawn, ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float otherValue)
{
    return true;
}

void AEOSActionGameState::Server_RequestSpawnVFX_Implementation(UNiagaraSystem* SystemToSpawn, FVector SpawnLocation, FVector Direction, float otherValue)
{

}

bool AEOSActionGameState::Server_RequestSpawnVFX_Validate(UNiagaraSystem* SystemToSpawn, FVector SpawnLocation, FVector Direction, float otherValue)
{
    return true;
}


void AEOSActionGameState::Multicast_AdjustIceOnCharacter_Implementation(UNiagaraSystem* SystemToSpawn, ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float otherValue)
{
    //if (!SystemToSpawn) return;
    if (!characterAttached) return;

    if (otherValue <= 0)
    {
        if (characterAttached->DynamicMaterialInstance)
        {
            characterAttached->DynamicMaterialInstance->SetVectorParameterValue(FName("Color"), FVector(0.0f, 0.0f, 0.0f));
        }
        return;
    }

    if (characterAttached->DynamicMaterialInstance)
    {
        characterAttached->DynamicMaterialInstance->SetVectorParameterValue(FName("Color"), FVector(0.0f, 0.868313f, 1.0f));
        characterAttached->DynamicMaterialInstance->SetScalarParameterValue(FName("Intensity"), 0.2f);
    }
}

void AEOSActionGameState::Server_AdjustIceOnCharacter_Implementation(UNiagaraSystem* SystemToSpawn, ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float otherValue)
{
    Multicast_AdjustIceOnCharacter(SystemToSpawn, characterAttached, SpawnLocation, Direction, otherValue);
}

bool AEOSActionGameState::Server_AdjustIceOnCharacter_Validate(UNiagaraSystem* SystemToSpawn, ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float otherValue)
{
    return true;
}


void AEOSActionGameState::Multicast_AdjustFireOnCharacter_Implementation(UNiagaraSystem* SystemToSpawn, ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float otherValue)
{
    if (!SystemToSpawn) return;
    if (!characterAttached) return;

    float max = (otherValue * 1.5f) * characterAttached->WeakpointSize;
    float min = (otherValue) * characterAttached->WeakpointSize;

    max = FMath::Clamp(max, 0, 170);
    min = FMath::Clamp(min, 0, 170);

    if (characterAttached->CurrentFire == nullptr)
    {
        characterAttached->CurrentFire = UNiagaraFunctionLibrary::SpawnSystemAttached(SystemToSpawn, characterAttached->GetMesh(), NAME_None, SpawnLocation, FRotator::ZeroRotator, EAttachLocation::KeepWorldPosition, true, true, ENCPoolMethod::None, true);
        if (characterAttached->CurrentFire)
        {
            characterAttached->CurrentFire->SetFloatParameter(FName("BaseSizeMax"), max);
            characterAttached->CurrentFire->SetFloatParameter(FName("BaseSizeMin"), min);
        }
        return;
    }


    if (otherValue <= 0 && characterAttached->CurrentFire)
    {
        characterAttached->CurrentFire->Deactivate();
        characterAttached->CurrentFire->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
        characterAttached->CurrentFire->DestroyComponent();
        characterAttached->CurrentFire = nullptr;
        if (characterAttached->DynamicMaterialInstance)
        {
            characterAttached->DynamicMaterialInstance->SetVectorParameterValue(FName("Color"), FVector(0.0f, 0.0f, 0.0f));
        }
        return;
    }

    if (characterAttached->DynamicMaterialInstance)
    {
        float normalized = min / 170.0f;
        float fireIntensity = FMath::Lerp(2.0f, 0.1f, normalized);

        characterAttached->DynamicMaterialInstance->SetVectorParameterValue(FName("Color"), FVector(1.0f, 0.165817f, 0.0f));
        characterAttached->DynamicMaterialInstance->SetScalarParameterValue(FName("Intensity"), fireIntensity);
    }

    characterAttached->CurrentFire->SetFloatParameter(FName("BaseSizeMax"), max);
    characterAttached->CurrentFire->SetFloatParameter(FName("BaseSizeMin"), min);
}

void AEOSActionGameState::Multicast_RequestSpawnVFXOnCharacter_Implementation(UNiagaraSystem* SystemToSpawn, ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float otherValue)
{
    if (!SystemToSpawn) return;
    if (!characterAttached) return;
    
    UNiagaraComponent* SpawnSystemAttached = UNiagaraFunctionLibrary::SpawnSystemAttached(SystemToSpawn, characterAttached->GetMesh(), NAME_None, SpawnLocation, FRotator::ZeroRotator, EAttachLocation::KeepWorldPosition, true, true, ENCPoolMethod::None, true);

    if (SpawnSystemAttached)
    {
        //NiagaraComp->SetVectorParameter(FName("Alignment"), Direction);
    }
}

void AEOSActionGameState::Multicast_RequestSpawnVFX_Implementation(UNiagaraSystem* SystemToSpawn, FVector SpawnLocation, FVector Direction, float otherValue)
{
    if (!SystemToSpawn) return;

    UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SystemToSpawn, SpawnLocation, FRotator::ZeroRotator, FVector(1.0f), true, true);

}


void AEOSActionGameState::Server_RequestSpawnDotLaserMarks_Implementation(UNiagaraSystem* SystemToSpawn, FVector SpawnLocation, FVector Direction, float ScorchSize)
{
    Multicast_SpawnDotLaserMarks(SystemToSpawn, SpawnLocation, Direction, ScorchSize);
}

bool AEOSActionGameState::Server_RequestSpawnDotLaserMarks_Validate(UNiagaraSystem* SystemToSpawn, FVector SpawnLocation, FVector Direction, float ScorchSize)
{
    return true;
}

void AEOSActionGameState::Multicast_SpawnDotLaserMarks_Implementation(UNiagaraSystem* SystemToSpawn, FVector SpawnLocation, FVector Direction, float ScorchSize)
{
    if (!SystemToSpawn) return;

    UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SystemToSpawn, SpawnLocation, FRotator::ZeroRotator, FVector(1.0f), true, true);

    if (NiagaraComp)
    {
        NiagaraComp->SetVectorParameter(FName("Alignment"), Direction);
        NiagaraComp->SetFloatParameter(FName("ScorchSize"), ScorchSize);
    }
}




void AEOSActionGameState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(AEOSActionGameState, AllChests, COND_None);
    DOREPLIFETIME_CONDITION(AEOSActionGameState, AllEnemies, COND_None);
    DOREPLIFETIME_CONDITION(AEOSActionGameState, AllItems, COND_None);
}