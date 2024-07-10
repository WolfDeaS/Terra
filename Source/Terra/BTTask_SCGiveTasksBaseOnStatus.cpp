// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SCGiveTasksBaseOnStatus.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TerraCharacter.h"
#include "AITypes.h"
#include "ActorsArray.h"
#include "SocialCell.h"

UBTTask_SCGiveTasksBaseOnStatus::UBTTask_SCGiveTasksBaseOnStatus()
{
	NodeName = "SC Give Tasks Base On Status";
}

EBTNodeResult::Type UBTTask_SCGiveTasksBaseOnStatus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ATerraCharacter* LocalCharacter = Cast<ATerraCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(CharacterKey.SelectedKeyName));
	
	LocalCharacter->SocialCell->CalculateAndAddToRequestStatus(OwnerComp.GetBlackboardComponent()->GetValueAsName(StatusKey.SelectedKeyName));
	LocalCharacter->SocialCell->GiveTasks();

	
	return EBTNodeResult::Succeeded;
}