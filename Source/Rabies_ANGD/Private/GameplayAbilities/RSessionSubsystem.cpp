// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/RSessionSubsystem.h"
#include "OnlineSubsystemUtils.h"

URSessionSubsystem::URSessionSubsystem() : 
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionCompleted)) { }

void URSessionSubsystem::CreateSession(int32 NumPublicConnections, bool IsLANMatch)
{

}

void URSessionSubsystem::OnCreateSessionCompleted(FName SessionName, bool Successful)
{

}