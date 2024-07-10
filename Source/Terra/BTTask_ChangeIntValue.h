// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "InteractionComponent.h"
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

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IntKey;

	UPROPERTY(EditAnywhere, Category = "Parameters")
	TEnumAsByte<EBool> bSetValue; // If "Yes", instead of adding to Value, it will be set

	UPROPERTY(EditAnywhere, Category = "Parameters")
	TEnumAsByte<EBool> bRandomValue; // If "Yes", range will be [0;Value]

	UPROPERTY(EditAnywhere, Category = "Parameters")
	int Value; 
};
