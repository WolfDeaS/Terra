// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_GiveSC.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Terra/TerraCharacter.h"
#include "AITypes.h"
#include "Terra/SocialCell.h"

UBTTask_GiveSC::UBTTask_GiveSC()
{
	NodeName = "Get SC";
}

EBTNodeResult::Type UBTTask_GiveSC::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ATerraCharacter* LocalCharacter = Cast<ATerraCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(CharacterKey.SelectedKeyName));
	
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(BlackboardKey.SelectedKeyName, LocalCharacter->SocialCell);

	
	return EBTNodeResult::Succeeded;
}