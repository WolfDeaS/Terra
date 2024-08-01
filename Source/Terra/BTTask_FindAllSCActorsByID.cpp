// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindAllSCActorsByID.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TerraCharacter.h"
#include "AITypes.h"
#include "ActorsArray.h"
#include "SocialCell.h"

UBTTask_FindAllSCActorsByID::UBTTask_FindAllSCActorsByID()
{
	NodeName = "Find All Social Cell Interactable Actors By ID";

	CharacterKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindAllSCActorsByID, CharacterKey), AActor::StaticClass());
	BlackboardKey.AddNameFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindAllSCActorsByID, BlackboardKey));
}

EBTNodeResult::Type UBTTask_FindAllSCActorsByID::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ATerraCharacter* LocalCharacter = Cast<ATerraCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(CharacterKey.SelectedKeyName));
	
	LocalCharacter->LocalArray.Empty();
	FName LocalName = OwnerComp.GetBlackboardComponent()->GetValueAsName(BlackboardKey.SelectedKeyName);
	LocalCharacter->LocalArray.Append(*LocalCharacter->SocialCell->InteractableActors.Find(LocalName));

	return EBTNodeResult::Succeeded;
}