// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionMark.h"
#include "BehaviorTree/BTDecorator.h"
#include "BT_IsHaveStatusMark.generated.h"

/**
 * 
 */
UCLASS()
class TERRA_API UBT_IsHaveStatusMark : public UBTDecorator
{
	GENERATED_BODY()
	
protected:

	UBT_IsHaveStatusMark();

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector CharacterKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ModifierKey;
};
