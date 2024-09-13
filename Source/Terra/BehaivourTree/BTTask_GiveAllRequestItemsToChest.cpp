// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_GiveAllRequestItemsToChest.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Terra/TerraCharacter.h"
#include "AITypes.h"
#include "Terra/SocialCell.h"

UBTTask_GiveAllRequestItemsToChest::UBTTask_GiveAllRequestItemsToChest()
{
	NodeName = "Give All Request Items To Chest";
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_GiveAllRequestItemsToChest, BlackboardKey), AActor::StaticClass());
	ChestKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_GiveAllRequestItemsToChest, ChestKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_GiveAllRequestItemsToChest::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ATerraCharacter* LocalCharacter = Cast<ATerraCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BlackboardKey.SelectedKeyName));
	AInteractableActor* LocalChest= Cast<AInteractableActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ChestKey.SelectedKeyName));

	TArray<FName> LocalModifiersArray;

	TMap<FName, float> LocalRequestMap = LocalCharacter->SocialCell->RequestMap;
	
	TMap<FName, float> ChangedRequestMap;
	TMap<FSItem, float> LocalItemMap;
	float LocalItemQuantity = -1.0f;
	
	if (!LocalRequestMap.IsEmpty())
	{
		for (auto& LocalItem : LocalCharacter->InventoryComponent->Inventory)
		{
			for (auto& Pair : LocalRequestMap)
			{
				if (LocalItem.Key.ModifierBonuses.Contains(Pair.Key))
				{
					if (LocalItemQuantity == LocalItem.Value)
					{
						ChangedRequestMap.Add(Pair.Key, *LocalItem.Key.ModifierBonuses.Find(Pair.Key) * LocalItemQuantity);
					}

					if (FMath::CeilToFloat(Pair.Value / *LocalItem.Key.ModifierBonuses.Find(Pair.Key)) > LocalItem.Value)
					{
						LocalItemQuantity = LocalItem.Value;

						LocalItemMap.Add(LocalItem.Key, LocalItemQuantity);
						ChangedRequestMap.Add(Pair.Key, *LocalItem.Key.ModifierBonuses.Find(Pair.Key) * LocalItemQuantity);
					}
					else
					{
						if (FMath::CeilToFloat(Pair.Value / *LocalItem.Key.ModifierBonuses.Find(Pair.Key)) > LocalItemQuantity)
						{
							LocalItemMap.Add(LocalItem.Key, LocalItemQuantity);
							LocalItemQuantity = FMath::CeilToFloat(Pair.Value / *LocalItem.Key.ModifierBonuses.Find(Pair.Key));
						}

						ChangedRequestMap.Add(Pair.Key, *LocalItem.Key.ModifierBonuses.Find(Pair.Key) * LocalItemQuantity);
					}
				}
			}

			LocalItemQuantity = -1.0f;
			for (auto& Pair : ChangedRequestMap)
			{
				if ((*LocalRequestMap.Find(Pair.Key) - Pair.Value) > 0.0f)
				{
					LocalRequestMap.Add(Pair.Key, *LocalRequestMap.Find(Pair.Key) - Pair.Value);
				}
				else
				{
					LocalRequestMap.Remove(Pair.Key);
				}
			}

			ChangedRequestMap.Empty();

			if (LocalRequestMap.IsEmpty())
			{
				break;
			}
		}

		for (auto& LocalItem : LocalItemMap)
		{
			LocalChest->InventoryComponent->AddItemToInventory(LocalItem.Key, LocalItem.Value, true);
			LocalCharacter->InventoryComponent->RemoveItem(LocalItem.Key, LocalItem.Value, false);
		}

		LocalCharacter->SocialCell->RequestMap = LocalRequestMap;

		UE_LOG(LogTemp, Warning, TEXT("Give All Request Items To Chest, Request Map Num: %d"), LocalCharacter->SocialCell->RequestMap.Num());
	}
	
	

	return EBTNodeResult::Succeeded;
}