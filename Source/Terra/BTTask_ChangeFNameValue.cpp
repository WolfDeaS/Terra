// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChangeFNameValue.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AITypes.h"

UBTTask_ChangeFNameValue::UBTTask_ChangeFNameValue()
{
	NodeName = "Change FName Value";
	BlackboardKey.AddNameFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ChangeFNameValue, BlackboardKey));
}

EBTNodeResult::Type UBTTask_ChangeFNameValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	OwnerComp.GetBlackboardComponent()->SetValueAsName(BlackboardKey.SelectedKeyName, FNameValue);
	
	return EBTNodeResult::Succeeded;
}