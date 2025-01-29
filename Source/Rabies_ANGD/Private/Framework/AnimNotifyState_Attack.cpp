// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/AnimNotifyState_Attack.h"
#include "Framework/RAttackingBoxComponent.h"


void UAnimNotifyState_Attack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	const AActor* OwnerActor = MeshComp->GetOwner();
	AttackingBoxComponent = OwnerActor->GetComponentByClass<URAttackingBoxComponent>();
	if (AttackingBoxComponent)
	{
		AttackingBoxComponent->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, AttachSocket);
		AttackingBoxComponent->StartDetection();
		AttackingBoxComponent->DoAttackCheck();
		AttackingBoxComponent->EndDetection();
	}
}
