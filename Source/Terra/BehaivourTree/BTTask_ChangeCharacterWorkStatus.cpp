// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChangeCharacterWorkStatus.h"
#include "AIController.h"
#include "Terra/TerraCharacter.h"
#include "Terra/SocialCell.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AITypes.h"

// Test vafla
UBTTask_ChangeCharacterWorkStatus::UBTTask_ChangeCharacterWorkStatus()
{
	NodeName = "Change Character Work Status";

	AddInfoKey.AddNameFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ChangeCharacterWorkStatus, AddInfoKey));
	ActivityInfoKey.AddNameFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ChangeCharacterWorkStatus, ActivityInfoKey));
}

EBTNodeResult::Type UBTTask_ChangeCharacterWorkStatus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ATerraCharacter* LocalCharacter = Cast<ATerraCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BlackboardKey.SelectedKeyName));
	FName LocalActivityInfo;
	FName LocalAddInfo;

	if (UseBlackboardKeys == EBool::Yes)
	{
		LocalActivityInfo = OwnerComp.GetBlackboardComponent()->GetValueAsName(ActivityInfoKey.SelectedKeyName);
		LocalAddInfo = OwnerComp.GetBlackboardComponent()->GetValueAsName(AddInfoKey.SelectedKeyName);
	}
	else
	{
		LocalActivityInfo = ActivityInfo;
		LocalAddInfo = AddInfo;
	}
	

	LocalCharacter->SocialCell->ChangeCharacterWorkStatus(FunctionType, bDeleteItemFromCharactersArray, LocalCharacter, OwnerComp.GetBlackboardComponent()->GetValueAsObject(ObjectForWorking.SelectedKeyName), LocalActivityInfo, LocalAddInfo);
	
	return EBTNodeResult::Succeeded;
}