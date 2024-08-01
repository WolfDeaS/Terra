#pragma once

#include "BT_IsHaveStatusMark.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TerraCharacter.h"
#include "AIController.h"

UBT_IsHaveStatusMark::UBT_IsHaveStatusMark()
{
	NodeName = "Is Character Has Needed Modifier";
}

bool UBT_IsHaveStatusMark::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bSuccess = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	if (!bSuccess) return false;

	ATerraCharacter* LocalCharacter = Cast<ATerraCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(CharacterKey.SelectedKeyName));

	FName Modifier = OwnerComp.GetBlackboardComponent()->GetValueAsName(ModifierKey.SelectedKeyName);

	TArray<FName> MarksArray;
	LocalCharacter->MarksActor.GenerateKeyArray(MarksArray);

	TArray<FString> ParseValue;
	TArray<FName> LocalItemArray;
	TArray<FName> LocalModifierArray;
	TArray<FName> UsedItemArray;

	for (auto& LocalMark : MarksArray)
	{
		LocalMark.ToString().ParseIntoArray(ParseValue, TEXT(";"), true);
		
		FSHerbCreation* HerbDataFromDT;
		HerbDataFromDT = LocalCharacter->DT_Herbs->FindRow<FSHerbCreation>(*ParseValue[1], TEXT("none"), false);
		
		HerbDataFromDT->ItemsGiven.GenerateKeyArray(LocalItemArray);

		for (auto& LocalItem : UsedItemArray)
		{
			LocalItemArray.Remove(LocalItem);
		}

		for (auto& LocalItem : LocalItemArray)
		{
			FSItemCreation* ItemDataFromDT;
			ItemDataFromDT = LocalCharacter->DT_Items->FindRow<FSItemCreation>(LocalItem, TEXT("none"), false);

			UsedItemArray.Add(LocalItem);


			for (auto& Pair : ItemDataFromDT->ModifierBonuses)
			{
				if (Pair.Value > 0)
				{
					if (Modifier == Pair.Key)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
} 




