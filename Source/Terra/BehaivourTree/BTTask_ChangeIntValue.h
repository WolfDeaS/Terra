// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Terra/InteractionComponent.h"
#include "BTTask_ChangeIntValue.generated.h"

/**
 * 
 */
UCLASS()
class TERRA_API UBTTask_ChangeIntValue : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_ChangeIntValue();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Parameters")
	TEnumAsByte<EBool> SetValue; // If "Yes", instead of adding to Value, it will be set

	UPROPERTY(EditAnywhere, Category = "Parameters")
	TEnumAsByte<EBool> RandomValue; // If "Yes", range will be [0;Value]

	UPROPERTY(EditAnywhere, Category = "Parameters")
	TEnumAsByte<EBool> UseBlackboardKeys;

	UPROPERTY(EditAnywhere, Category = "Parameters", meta = (EditCondition = "UseBlackboardKeys == EBool::Yes", EditConditionHides))
	FBlackboardKeySelector ValueKey;

	UPROPERTY(EditAnywhere, Category = "Parameters", meta = (EditCondition = "UseBlackboardKeys == EBool::No", EditConditionHides))
	int Value;

};
