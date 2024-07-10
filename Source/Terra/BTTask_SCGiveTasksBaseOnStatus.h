// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "InteractionComponent.h"
#include "BTTask_SCGiveTasksBaseOnStatus.generated.h"


UCLASS()
class TERRA_API UBTTask_SCGiveTasksBaseOnStatus : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_SCGiveTasksBaseOnStatus();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector CharacterKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector StatusKey;
};
