// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChangeIntValue.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Terra/TerraCharacter.h"
#include "Math/UnrealMathUtility.h"
#include "AITypes.h"

UBTTask_ChangeIntValue::UBTTask_ChangeIntValue()
{
	NodeName = "Change Int Value";

	BlackboardKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ChangeIntValue, BlackboardKey));
	ValueKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ChangeIntValue, ValueKey));
}

EBTNodeResult::Type UBTTask_ChangeIntValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	int LocalInt;
	LocalInt = OwnerComp.GetBlackboardComponent()->GetValueAsInt(BlackboardKey.SelectedKeyName);

	int LocalValue;

	if (UseBlackboardKeys == EBool::Yes)
	{
		UE_LOG(LogTemp, Warning, TEXT("Change Int Value: EBool:Yes"));
		LocalValue = OwnerComp.GetBlackboardComponent()->GetValueAsInt(ValueKey.SelectedKeyName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Change Int Value: EBool:No"));
		LocalValue = Value;
	}

	UE_LOG(LogTemp, Warning, TEXT("Change Int Value: LocalValue - %d"), LocalValue);

	if (RandomValue == EBool::Yes)
	{
		LocalValue = FMath::RandRange(0, LocalValue);
		UE_LOG(LogTemp, Warning, TEXT("Change Int Value: LocalValue Random - %d"), LocalValue);
	}

	if (SetValue == EBool::Yes)
	{
		LocalInt = LocalValue;
	}
	else
	{
		LocalInt += LocalValue;
	}

	UE_LOG(LogTemp, Warning, TEXT("Change Int Value: LocalInt - %d"), LocalInt);

	OwnerComp.GetBlackboardComponent()->SetValueAsInt(BlackboardKey.SelectedKeyName, LocalInt);

	return EBTNodeResult::Succeeded;
}