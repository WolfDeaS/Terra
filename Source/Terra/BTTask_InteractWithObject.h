// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "InteractionComponent.h"
#include "BTTask_InteractWithObject.generated.h"


UCLASS()
class TERRA_API UBTTask_InteractWithObject : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_InteractWithObject();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ObjectKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ActivityStatus;

	ATerraCharacter* LocalCharacter;
};
