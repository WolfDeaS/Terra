// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Terra/InteractionComponent.h"
#include "BTTask_ChangeCharacterWorkStatus.generated.h"


UCLASS()
class TERRA_API UBTTask_ChangeCharacterWorkStatus : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_ChangeCharacterWorkStatus();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	//bFunctionType to make Character: 0 - Working, 1 - ReturnToSC 2 - Rest.
	UPROPERTY(EditAnywhere, Category = "Parameters")
	uint8 FunctionType;

	// bDeleteItemFromCharactersArray - Means delete character from CharactersOutOfActivities if bFunctionType 1. Or delete character from CharactersActivities if bFunctionType 0
	UPROPERTY(EditAnywhere, Category = "Parameters")
	bool bDeleteItemFromCharactersArray;

	UPROPERTY(EditAnywhere, Category = "Parameters")
	FBlackboardKeySelector ObjectForWorking;

	UPROPERTY(EditAnywhere, Category = "Parameters")
	TEnumAsByte<EBool> UseBlackboardKeys;

	UPROPERTY(EditAnywhere, Category = "Parameters", meta = (EditCondition = "UseBlackboardKeys == EBool::Yes", EditConditionHides))
	FBlackboardKeySelector ActivityInfoKey;

	UPROPERTY(EditAnywhere, Category = "Parameters", meta = (EditCondition = "UseBlackboardKeys == EBool::Yes", EditConditionHides))
	FBlackboardKeySelector AddInfoKey;

	UPROPERTY(EditAnywhere, Category = "Parameters", meta = (EditCondition = "UseBlackboardKeys == EBool::No", EditConditionHides))
	FName ActivityInfo;

	UPROPERTY(EditAnywhere, Category = "Parameters", meta = (EditCondition = "UseBlackboardKeys == EBool::No", EditConditionHides))
	FName AddInfo;
};
