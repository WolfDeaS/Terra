// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_InteractWithObject.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TerraCharacter.h"
#include "AITypes.h"
#include "ActorsArray.h"
#include "SocialCell.h"

UBTTask_InteractWithObject::UBTTask_InteractWithObject()
{
	NodeName = "Interact With Object";

	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_InteractWithObject, BlackboardKey), AActor::StaticClass());
	ObjectKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_InteractWithObject, ObjectKey), AActor::StaticClass());

	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_InteractWithObject::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	LocalCharacter = Cast<ATerraCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BlackboardKey.SelectedKeyName));

	UE_LOG(LogTemp, Warning, TEXT("Interact With Object: Begin"));

	// It checks is object InteractableActor.
	if (OwnerComp.GetBlackboardComponent()->GetValueAsObject(ObjectKey.SelectedKeyName)->IsA<AInteractableActor>() && !LocalCharacter->bIsInteracted)
	{
		UE_LOG(LogTemp, Warning, TEXT("Interact With Object: Begin Interaction & Check Succeeded"));

		AInteractableActor* LocalIA = Cast<AInteractableActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ObjectKey.SelectedKeyName));
		LocalIA->InteractionComponent->OnInteractionBegan(LocalCharacter);

		return EBTNodeResult::InProgress;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("LocalObject isnt Interactable Actor"));
		return EBTNodeResult::Failed;
	}

	
}

void UBTTask_InteractWithObject::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	UE_LOG(LogTemp, Warning, TEXT("Interact With Object: In Progress"));

	// Character again must not have any of interaction. It must not be a first check
	if (!LocalCharacter->bIsInteracted)
	{
		UE_LOG(LogTemp, Warning, TEXT("Interact With Object: Output"));

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
