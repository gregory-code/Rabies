// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RPlayerBase.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Player/RPlayerController.h"

#include "GameplayAbilities/RAttributeSet.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAbilityGenericTags.h"


#include "AbilitySystemBlueprintLibrary.h"
#include "Animation/AnimInstance.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Net/UnrealNetwork.h"

#include "Framework/RGameMode.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SceneComponent.h"

#define ECC_RangedAttack ECC_GameTraceChannel2

ARPlayerBase::ARPlayerBase()
{
	viewPivot = CreateDefaultSubobject<USceneComponent>("Camera Pivot");
	viewCamera = CreateDefaultSubobject<UCameraComponent>("View Camera");

	viewCamera->SetupAttachment(viewPivot, USpringArmComponent::SocketName);
	viewCamera->SetRelativeLocation(DefaultCameraLocal);
	viewCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(1080.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 2.0f;

	cameraClampMax = 10;
	cameraClampMin = -60;
	bIsScoping = false;
}

void ARPlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	viewPivot->SetRelativeLocation(GetActorLocation()); // centers the pivot on the player without getting the players rotation

	if (IsFlying() && GetCharacterMovement()->IsFalling() == false && !GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetTakeOffDelayTag()))
	{
		FGameplayEventData eventData;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, URAbilityGenericTags::GetEndFlyingTag(), eventData);
	}

	FVector currentVelocity = GetVelocity();
	if (IsFlying() && bHoldingJump && currentVelocity.Z <= 0)
	{
		currentVelocity.Z = FMath::Lerp(currentVelocity.Z, -0.0005f, 8 * DeltaTime);
		GetCharacterMovement()->Velocity = currentVelocity;
	}
	
	if (bIsScoping)
	{
		RotatePlayer(DeltaTime);
	}
}

void ARPlayerBase::BeginPlay()
{
	Super::BeginPlay();

}

void ARPlayerBase::PawnClientRestart()
{
	Super::PawnClientRestart();

	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		InputSubsystem->ClearAllMappings();
		InputSubsystem->AddMappingContext(inputMapping, 0);
	}
}

void ARPlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* enhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (enhancedInputComp)
	{
		enhancedInputComp->BindAction(moveInputAction, ETriggerEvent::Triggered, this, &ARPlayerBase::Move);
		enhancedInputComp->BindAction(lookInputAction, ETriggerEvent::Triggered, this, &ARPlayerBase::Look);
		enhancedInputComp->BindAction(jumpInputAction, ETriggerEvent::Started, this, &ARPlayerBase::StartJump);
		enhancedInputComp->BindAction(jumpInputAction, ETriggerEvent::Completed, this, &ARPlayerBase::ReleaseJump);
		enhancedInputComp->BindAction(QuitOutAction, ETriggerEvent::Triggered, this, &ARPlayerBase::QuitOut);
		enhancedInputComp->BindAction(basicAttackAction, ETriggerEvent::Triggered, this, &ARPlayerBase::DoBasicAttack);
		enhancedInputComp->BindAction(basicAttackAction, ETriggerEvent::Completed, this, &ARPlayerBase::StopBasicAttack);
		enhancedInputComp->BindAction(scopeInputAction, ETriggerEvent::Started, this, &ARPlayerBase::EnableScoping);
		enhancedInputComp->BindAction(scopeInputAction, ETriggerEvent::Completed, this, &ARPlayerBase::DisableScoping);
		enhancedInputComp->BindAction(scrollInputAction, ETriggerEvent::Triggered, this, &ARPlayerBase::Scroll);
		enhancedInputComp->BindAction(specialAttackAction, ETriggerEvent::Triggered, this, &ARPlayerBase::TryActivateSpecialAttack);
		enhancedInputComp->BindAction(ultimateAttackAction, ETriggerEvent::Triggered, this, &ARPlayerBase::TryActivateUltimateAttack);
		enhancedInputComp->BindAction(AbilityConfirmAction, ETriggerEvent::Triggered, this, &ARPlayerBase::ConfirmActionTriggered);
		enhancedInputComp->BindAction(AbilityCancelAction, ETriggerEvent::Triggered, this, &ARPlayerBase::CancelActionTriggered);
		enhancedInputComp->BindAction(InteractInputAction, ETriggerEvent::Triggered, this, &ARPlayerBase::Interact);
		enhancedInputComp->BindAction(PausingInputAction, ETriggerEvent::Triggered, this, &ARPlayerBase::Pause);
	}
}

void ARPlayerBase::Move(const FInputActionValue& InputValue)
{
	if (GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetUnActionableTag()) || GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag())) return;

	MoveInput = InputValue.Get<FVector2D>();
	MoveInput.Normalize();

	AddMovementInput(MoveInput.Y * GetMoveFwdDir() + MoveInput.X * GetMoveRightDir());
}

void ARPlayerBase::RotatePlayer(float DeltaTime)
{
	FRotator cameraRot = viewPivot->GetComponentRotation();
	cameraRot.Roll = 0;

	FRotator currentRot = GetActorRotation();

	float yawDiff = cameraRot.Yaw - currentRot.Yaw;
	float pitchDiff = cameraRot.Pitch - currentRot.Pitch;

	if (yawDiff > 180.0f) yawDiff -= 360.0f;
	if (yawDiff < -180.0f) yawDiff += 360.0f;

	if (pitchDiff > 180.0f) pitchDiff -= 360.0f;
	if (pitchDiff < -180.0f) pitchDiff += 360.0f;

	float yawInput = FMath::Lerp(0.0f, yawDiff, 10 * DeltaTime);
	float pitchInput = FMath::Lerp(0.0f, pitchDiff, 10 * DeltaTime);

	AddControllerYawInput(yawInput);
	AddControllerPitchInput(pitchInput);
}

void ARPlayerBase::Look(const FInputActionValue& InputValue)
{
	FVector2D input = InputValue.Get<FVector2D>();

	FRotator newRot = viewPivot->GetComponentRotation();
	newRot.Pitch += input.Y;
	newRot.Yaw += input.X;

	newRot.Pitch = FMath::ClampAngle(newRot.Pitch, cameraClampMin, cameraClampMax);

	viewPivot->SetWorldRotation(newRot);
}

void ARPlayerBase::SetRabiesPlayerController(ARPlayerController* newController)
{
	playerController = newController;
}

AActor* ARPlayerBase::Hitscan(float range, float sphereRadius)
{
	int32 sizeX, sizeY;
	sizeX = -1;
	sizeY = -1;

	if (playerController == nullptr) return nullptr;

	//playerController->GetViewportSize(sizeX, sizeY);

	//FVector2D viewport = (GEngine->GameViewport->Viewport->GetSizeXY());
	sizeX /= 2.0f;
	sizeY /= 2.0f;

	//viewport.X /= 2.0f;
	//viewport.Y /= 2.0f;

	FVector WorldLocation;
	FVector WorldDirection;
	//if (playerController->DeprojectScreenPositionToWorld(viewport.X, viewport.Y, WorldLocation, WorldDirection))
	//{
		FVector startTrace = viewCamera->GetComponentLocation();
		FVector endTrace = startTrace + viewCamera->GetComponentRotation().Vector() * range;

		//FVector lineStart = GetMesh()->GetSocketLocation(RangedAttackSocketName);
		//FVector lineEnd = lineStart + GetActorForwardVector() * range;

		DrawDebugLine(GetWorld(), startTrace, endTrace, FColor::Green);

		FCollisionShape collisionShape = FCollisionShape::MakeSphere(sphereRadius);
		bool hit = GetWorld()->SweepSingleByChannel(hitResult, startTrace, endTrace, FQuat::Identity, ECC_RangedAttack, collisionShape);
		if (hit)
		{
			//FString actorName = hitResult.GetActor()->GetName();
			//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Hit!"));

			return hitResult.GetActor();
		}
	//}
	//else
	//{
		//UE_LOG(LogTemp, Warning, TEXT("I cannot interact"));
	//}

	return nullptr;
}

void ARPlayerBase::StartJump()
{
	bHoldingJump = true;
	if (bInstantJump)
	{
		Jump();
		return;
	}

	GetAbilitySystemComponent()->PressInputID((int)EAbilityInputID::Passive); // Dot's jump fly
}

void ARPlayerBase::ReleaseJump()
{
	bHoldingJump = false;
	if (bInstantJump) return;

	FGameplayEventData eventData;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, URAbilityGenericTags::GetEndTakeOffChargeTag(), eventData);

	if (!IsFlying())
	{
		Jump();
	}
}

void ARPlayerBase::QuitOut()
{
	/*APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, true);
	}*/
}

void ARPlayerBase::DoBasicAttack()
{
	GetAbilitySystemComponent()->PressInputID((int)EAbilityInputID::BasicAttack);
}

void ARPlayerBase::StopBasicAttack()
{
	FGameplayEventData eventData;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, URAbilityGenericTags::GetEndAttackTag(), eventData);
}

void ARPlayerBase::EnableScoping()
{
	GetAbilitySystemComponent()->PressInputID((int)EAbilityInputID::Scoping);
}

void ARPlayerBase::DisableScoping()
{
	FGameplayEventData eventData;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, URAbilityGenericTags::GetEndScopingTag(), eventData);
}

void ARPlayerBase::Scroll(const FInputActionValue& InputActionVal)
{

	float scrollAmount = InputActionVal.Get<float>();

	scrollAmount *= -1;

	DefaultCameraLocal.X += (scrollAmount * 100);

	DefaultCameraLocal.X = FMath::Clamp(DefaultCameraLocal.X, -800, -200);

	if (!bIsScoping)
	{
		GetWorldTimerManager().ClearTimer(CameraLerpHandle);
		viewCamera->SetRelativeLocation(DefaultCameraLocal);
	}
}

void ARPlayerBase::TryActivateSpecialAttack()
{

}

void ARPlayerBase::TryActivateUltimateAttack()
{

}

void ARPlayerBase::ConfirmActionTriggered()
{
	//UE_LOG(LogTemp, Warning, TEXT("Confirmed"));
	//GetAbilitySystemComponent()->InputConfirm();
}

void ARPlayerBase::CancelActionTriggered()
{
	//UE_LOG(LogTemp, Warning, TEXT("Cancelled"));
	//GetAbilitySystemComponent()->InputCancel();
}

void ARPlayerBase::Interact()
{
	if (!canInteract)
	{
		UE_LOG(LogTemp, Warning, TEXT("I cannot interact"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("I am interacting"));
}

void ARPlayerBase::Pause()
{
	/*ARGameMode* GameMode = GetWorld()->GetAuthGameMode<ARGameMode>();
	isPaused = !isPaused;

	if (isPaused)
	{
		GameMode->PausingGame(isPaused);
		//Remove all controls to the characters when they are paused.
	}
	else
	{
		GameMode->PausingGame(isPaused);
		//Return all controls to the characters when they are unpaused.
	}*/
}

FVector ARPlayerBase::GetMoveFwdDir() const
{
	FVector CamerFwd = viewCamera->GetForwardVector();
	CamerFwd.Z = 0;
	return CamerFwd.GetSafeNormal();
}

FVector ARPlayerBase::GetMoveRightDir() const
{
	return viewCamera->GetRightVector();
}

void ARPlayerBase::ScopingTagChanged(bool bNewIsAiming)
{
	bUseControllerRotationYaw = bNewIsAiming;
	bIsScoping = bNewIsAiming;
	GetCharacterMovement()->bOrientRotationToMovement = !bNewIsAiming;


	if (IsValid(playerController))
		playerController->ChangeCrosshairState(bNewIsAiming);

	if (bNewIsAiming)
	{
		cameraClampMax = 50;
		cameraClampMin = -50;
		LerpCameraToLocalOffset(AimCameraLocalOffset);
	}
	else
	{
		DisableScoping();
		cameraClampMax = 10;
		cameraClampMin = -60;
		LerpCameraToLocalOffset(DefaultCameraLocal);
	}
}

void ARPlayerBase::LerpCameraToLocalOffset(const FVector& LocalOffset)
{
	GetWorldTimerManager().ClearTimer(CameraLerpHandle);
	CameraLerpHandle = GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ARPlayerBase::TickCameraLocalOffset, LocalOffset));
}

void ARPlayerBase::TickCameraLocalOffset(FVector Goal)
{
	FVector CurrentLocalOffset = viewCamera->GetRelativeLocation();
	if (FVector::Dist(CurrentLocalOffset, Goal) < 1)
	{
		viewCamera->SetRelativeLocation(Goal);
		return;
	}

	FVector NewLocalOffset = FMath::Lerp(CurrentLocalOffset, Goal, GetWorld()->GetDeltaSeconds() * AimCameraLerpingSpeed);
	viewCamera->SetRelativeLocation(NewLocalOffset);
	CameraLerpHandle = GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ARPlayerBase::TickCameraLocalOffset, Goal));
}

void ARPlayerBase::SetInteraction(bool setInteract)
{
	canInteract = setInteract;
}

void ARPlayerBase::SetPausetoFalse()
{
	isPaused = false;
}
