// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "InteractionComponent.h"
#include "BTTask_GiveAllRequestItemsToChest.generated.h"


UCLASS()
class TERRA_API UBTTask_GiveAllRequestItemsToChest : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_GiveAllRequestItemsToChest();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ChestKey;
};
