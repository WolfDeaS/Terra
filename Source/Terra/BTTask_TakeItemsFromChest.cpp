// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TakeItemsFromChest.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TerraCharacter.h"
#include "AITypes.h"
#include "ActorsArray.h"
#include "SocialCell.h"

UBTTask_TakeItemsFromChest::UBTTask_TakeItemsFromChest()
{
	NodeName = "Take Items From Chest";
}

EBTNodeResult::Type UBTTask_TakeItemsFromChest::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ATerraCharacter* LocalCharacter = Cast<ATerraCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(CharacterKey.SelectedKeyName));
	AInteractableActor* LocalChest = Cast<AInteractableActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ChestActorKey.SelectedKeyName));

	float LocalStatusValueLeft;
	TArray<FSItem> LocalItems;
	FSItem LocalItem;
	int LocalQuantity;
	FName LocalStatus;
	FString LocalString;

	OwnerComp.GetBlackboardComponent()->SetValueAsBool(bIsEnoughItems.SelectedKeyName, true);
	

	switch (ItemType)
	{
	case EItemType::Food:

		LocalStatus = OwnerComp.GetBlackboardComponent()->GetValueAsName(StatusKey.SelectedKeyName);

		LocalChest->InventoryComponent->Inventory.GenerateKeyArray(LocalItems);
		LocalItems = LocalChest->InventoryComponent->FindItemArrayWithModifier(LocalItems, LocalStatus);

		if (LocalChest->InventoryComponent->FindItemArrayWithModifier(LocalItems, LocalStatus).IsEmpty())
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(bIsEnoughItems.SelectedKeyName, false);

			EBTNodeResult::Succeeded;
			break;
		}

		

		LocalString = OwnerComp.GetBlackboardComponent()->GetValueAsName(StatusKey.SelectedKeyName).ToString();
		LocalString.Append("DebuffsStarts");

		LocalStatusValueLeft = *LocalCharacter->Modifiers.Find(*LocalString);
		LocalStatusValueLeft *= HowManyPortions;

		while (LocalStatusValueLeft != 0.0f)
		{
			LocalItem =	LocalChest->InventoryComponent->FindItemWithHighsetDelicious(LocalItems);

			LocalQuantity = *LocalChest->InventoryComponent->Inventory.Find(LocalItem);

			if (LocalQuantity * *LocalItem.ModifierBonuses.Find(LocalStatus) > LocalStatusValueLeft)
			{
				LocalQuantity = ceil(LocalStatusValueLeft / *LocalItem.ModifierBonuses.Find(LocalStatus));
				LocalCharacter->InventoryComponent->AddItemToInventory(LocalItem, LocalQuantity);
				LocalChest->InventoryComponent->RemoveItem(LocalItem, LocalQuantity, true);

				break;
			}
			else
			{
				LocalCharacter->InventoryComponent->AddItemToInventory(LocalItem, LocalQuantity);
				LocalChest->InventoryComponent->RemoveItem(LocalItem, LocalQuantity, true);
				LocalItems.Remove(LocalItem);

				LocalStatusValueLeft -= LocalQuantity * *LocalItem.ModifierBonuses.Find(LocalStatus);
			}

			if (LocalChest->InventoryComponent->FindItemArrayWithModifier(LocalItems, LocalStatus).IsEmpty())
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsBool(bIsEnoughItems.SelectedKeyName, false);

				EBTNodeResult::Succeeded;
				break;
			}
		}

		break;

	default:
		
		break;
	}
	
	return EBTNodeResult::Succeeded;
}