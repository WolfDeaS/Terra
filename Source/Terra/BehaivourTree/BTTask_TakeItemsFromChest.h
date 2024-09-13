// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Terra/InteractionComponent.h"
#include "BTTask_TakeItemsFromChest.generated.h"


UCLASS()
class TERRA_API UBTTask_TakeItemsFromChest : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_TakeItemsFromChest();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector CharacterKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ChestActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector bIsEnoughItems;
	
	UPROPERTY(EditAnywhere, Category = "Parameters")
	TEnumAsByte<EItemType> ItemType;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "ItemType == EItemType::Food", EditConditionHides), Category = "Parameters")
	int HowManyPortions;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector StatusKey;
};
