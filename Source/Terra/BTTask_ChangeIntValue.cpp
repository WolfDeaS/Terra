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

	BlackboardKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ChangeIntValue, BlackboardKey));
}

EBTNodeResult::Type UBTTask_ChangeIntValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	int LocalInt;
	LocalInt = OwnerComp.GetBlackboardComponent()->GetValueAsInt(BlackboardKey.SelectedKeyName);

	if (RandomValue == EBool::Yes)
	{
		Value = FMath::RandRange(0, Value);
	}

	if (SetValue == EBool::Yes)
	{
		LocalInt = Value;
	}
	else
	{
		LocalInt += Value;
	}


	OwnerComp.GetBlackboardComponent()->SetValueAsInt(BlackboardKey.SelectedKeyName, LocalInt);

	return EBTNodeResult::Succeeded;
}