// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChangeCharacterWorkStatus.h"
#include "AIController.h"
#include "TerraCharacter.h"
#include "SocialCell.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AITypes.h"

// Test vafla
UBTTask_ChangeCharacterWorkStatus::UBTTask_ChangeCharacterWorkStatus()
{
	NodeName = "Change Character Work Status";
}

EBTNodeResult::Type UBTTask_ChangeCharacterWorkStatus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ATerraCharacter* LocalCharacter = Cast<ATerraCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BlackboardKey.SelectedKeyName));

	LocalCharacter->SocialCell->ChangeCharacterWorkStatus(FunctionType, bDeleteItemFromCharactersArray, LocalCharacter, OwnerComp.GetBlackboardComponent()->GetValueAsObject(ObjectForWorking.SelectedKeyName), ActivityInfo, AddInfo);
	
	return EBTNodeResult::Succeeded;
}