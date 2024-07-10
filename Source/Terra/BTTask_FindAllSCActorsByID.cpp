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
}

EBTNodeResult::Type UBTTask_FindAllSCActorsByID::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ATerraCharacter* LocalCharacter = Cast<ATerraCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(CharacterKey.SelectedKeyName));
	
	FName LocalName = OwnerComp.GetBlackboardComponent()->GetValueAsName(FindActorIDKey.SelectedKeyName);
	LocalCharacter->LocalArray.Append(*LocalCharacter->SocialCell->InteractableActors.Find(LocalName));

	return EBTNodeResult::Succeeded;
}