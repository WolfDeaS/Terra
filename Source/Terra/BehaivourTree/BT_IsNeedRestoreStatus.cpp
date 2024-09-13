#pragma once

#include "BT_IsNeedRestoreStatus.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Terra/TerraCharacter.h"
#include "AIController.h"

bool UBT_IsNeedRestoreStatus::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bSuccess = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	if (!bSuccess) return false;

	ATerraCharacter* LocalCharacter = Cast<ATerraCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(CharacterKey.SelectedKeyName));

	if (!LocalCharacter->StatusComponent->GetDebuffStatuses().IsEmpty())
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsName(StatusKey.SelectedKeyName, LocalCharacter->StatusComponent->GetDebuffStatuses()[0]);
		return true;
	}

	return false;
}




