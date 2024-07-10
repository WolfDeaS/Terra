// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChangeIntValue.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TerraCharacter.h"
#include "Math/UnrealMathUtility.h"
#include "AITypes.h"

UBTTask_ChangeIntValue::UBTTask_ChangeIntValue()
{
	NodeName = "Change Int Value";
}

EBTNodeResult::Type UBTTask_ChangeIntValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	int LocalInt;
	LocalInt = OwnerComp.GetBlackboardComponent()->GetValueAsInt(IntKey.SelectedKeyName);

	if (bRandomValue == EBool::Yes)
	{
		Value = FMath::RandRange(0, Value);
	}

	if (bSetValue == EBool::Yes)
	{
		LocalInt = Value;
	}
	else
	{
		LocalInt += Value;
	}


	OwnerComp.GetBlackboardComponent()->SetValueAsInt(IntKey.SelectedKeyName, LocalInt);

	return EBTNodeResult::Succeeded;
}