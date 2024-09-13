// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindNearestStatusRestore.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Terra/TerraCharacter.h"
#include "AITypes.h"

UBTTask_FindNearestStatusRestore::UBTTask_FindNearestStatusRestore()
{
	NodeName = "Find Nearest Status Restore";
}

EBTNodeResult::Type UBTTask_FindNearestStatusRestore::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	ATerraCharacter* LocalCharacter = Cast<ATerraCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(CharacterKey.SelectedKeyName));

	FName LocalStatus = OwnerComp.GetBlackboardComponent()->GetValueAsName(StatusKey.SelectedKeyName);

	FString LocalString = OwnerComp.GetBlackboardComponent()->GetValueAsName(StatusKey.SelectedKeyName).ToString();
	LocalString.Append("DebuffsStarts");

	// Eat food from inv
	while (*LocalCharacter->Modifiers.Find(LocalStatus) < *LocalCharacter->Modifiers.Find(*LocalString))
	{
		LocalCharacter->InventoryComponent->UseHighPriorityItemByTag("Food", LocalStatus);

		TArray<FSItem> LocalItems;
		LocalCharacter->InventoryComponent->Inventory.GenerateKeyArray(LocalItems);
		if (LocalCharacter->InventoryComponent->FindItemArrayWithModifier(LocalItems, LocalStatus).IsEmpty())
		{
			break;
		}
	}

	float LocalFloat;
	LocalFloat = 0.0f;

	TArray<FSItem> LocalItems;

	// Find nearest chest if not enough food in inv
	LocalCharacter->InventoryComponent->Inventory.GenerateKeyArray(LocalItems);
	LocalItems = LocalCharacter->InventoryComponent->FindItemArrayWithModifier(LocalItems, LocalStatus);
	if (!LocalItems.IsEmpty())
	{
		
		for (auto& LocalItem : LocalItems)
		{
			LocalFloat += *LocalItem.ModifierBonuses.Find(LocalStatus) * *LocalCharacter->InventoryComponent->Inventory.Find(LocalItem);


			if (LocalFloat >= *LocalCharacter->Modifiers.Find(*LocalString))
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsBool(IsEnoughFoodKey.SelectedKeyName, true);
				break;
			}
		}
		if (LocalFloat < *LocalCharacter->Modifiers.Find(*LocalString))
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(IsEnoughFoodKey.SelectedKeyName, false);
			OwnerComp.GetBlackboardComponent()->SetValueAsName(FindActorIDKey.SelectedKeyName, "Chest");
		}
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(IsEnoughFoodKey.SelectedKeyName, false);
		OwnerComp.GetBlackboardComponent()->SetValueAsName(FindActorIDKey.SelectedKeyName, "Chest");
	}

	return EBTNodeResult::Succeeded;
}