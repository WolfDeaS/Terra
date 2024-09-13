// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "TerraCharacter.h"
#include "TerraPlayerController.h"
#include "InteractableActor.h"
#include "TimeActor.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;

	NoneItem.ID = "None";

}

void UInventoryComponent::AddItemToInventory(FSItem Item, int Quantity, bool bIgnoreCargo)
{
	if (Inventory.Contains(Item))
	{
		int LocalQuantity;
		LocalQuantity = *Inventory.Find(Item);

		if (LocalQuantity + Quantity > 0)
		{
			Inventory.Add(Item, LocalQuantity + Quantity);
			if (!bIgnoreCargo)
			{
				RecalculateCargoBasedOnItem(Item.ID, LocalQuantity, LocalQuantity + Quantity);
			}
			
		}
		else
		{
			Inventory.Remove(Item);
			if (!bIgnoreCargo)
			{
				RecalculateCargoBasedOnItem(Item.ID, LocalQuantity, 0);
			}
		}
	}
	else
	{
		Inventory.Add(Item, Quantity);
		if (!bIgnoreCargo)
		{
			RecalculateCargoBasedOnItem(Item.ID, 0, Quantity);
		}
	}
}



FSItem UInventoryComponent::CreateItem(FName ID, int Quantity, bool bIgnoreCargo)
{

	FSItemCreation* ItemDataFromDT;
	ItemDataFromDT = DT_Items->FindRow<FSItemCreation>(ID, TEXT("none"), false);

	FSItem LocalItem;
	LocalItem.ID = ID;
	LocalItem.Durability = ItemDataFromDT->MaxDurability;
	LocalItem.Active = 0;
	
	if (Quantity != 0)
	{
		AddItemToInventory(LocalItem, Quantity, bIgnoreCargo);
	}

	return LocalItem;
}

void UInventoryComponent::RemoveItem(FSItem Item, int Quantity, bool bIgnoreCargo)
{

	int LocalInt;
	LocalInt = *Inventory.Find(Item);

	int LocalVafla = 1 + 2;
	if (LocalInt - Quantity <= 0)
	{
		Inventory.Remove(Item);
		if (!bIgnoreCargo)
		{
			RecalculateCargoBasedOnItem(Item.ID, LocalInt, 0);
		}
	}
	else
	{
		LocalInt -= Quantity;
		if (!bIgnoreCargo)
		{
			RecalculateCargoBasedOnItem(Item.ID, *Inventory.Find(Item), LocalInt);
		}
		Inventory.Add(Item, LocalInt);
	}
}

void UInventoryComponent::ThrowItem(FSItem Item)
{
	RemoveItem(Item, 1);

	if (ItemObjectOnThrowClass)
	{
		FSItemCreation* ItemDataFromDT;
		ItemDataFromDT = DT_Items->FindRow<FSItemCreation>(Item.ID, TEXT("none"), false);
		ItemObjectOnThrow->UpdateMesh(ItemDataFromDT->ItemMesh);
		ItemObjectOnThrow = GetWorld()->SpawnActor<AInteractableActor>(ItemObjectOnThrowClass, Character->GetTargetLocation(), Character->GetViewRotation());

		ItemObjectOnThrow->InitItem(ItemDataFromDT->Weight);
		ItemObjectOnThrow->InteractionComponent->InteractID = EInteractID::Item;
		ItemObjectOnThrow->InteractionComponent->LocalItem = Item;
		ItemObjectOnThrow->InteractionComponent->ActorREF = ItemObjectOnThrow;
		ItemObjectOnThrow->InteractionComponent->bIsDurabilityDecay = ItemDataFromDT->IsDurabilityFreshness;

		ATimeActor* LocalTimeActor;
		LocalTimeActor = Character->TimeActor;

		LocalTimeActor->ThrowedItems.Add(ItemObjectOnThrow);
	}
}

void UInventoryComponent::UseItem(FSItem Item)
{
	FSItemCreation* ItemDataFromDT;
	ItemDataFromDT = DT_Items->FindRow<FSItemCreation>(Item.ID, TEXT("none"), false);

	//TArray<FName> LocalKeys;

	TArray<FSItem> LocalItems;
	Inventory.GenerateKeyArray(LocalItems);

	TArray<int> LocalValue;
	Inventory.GenerateValueArray(LocalValue);

	uint8 LocalIndex;
	LocalIndex = LocalItems.Find(Item);

	FSItem LocalItem;
	LocalItem = Item;

	int NewItemQuantity;

	TMap<FName, float> LocalModifierBonuses = CalculateModifiersBasedOnDurability(LocalItem);
	TMap<FName, float> LocalStatusesNeed;
	float PointToConsume;

	switch (ItemDataFromDT->Type)
	{
	case EItemType::Food:

		RemoveItem(Item, 1);

		CalculateCharactersModifiers(LocalModifierBonuses);
		
		if (ItemDataFromDT->EffectDuration > 0)
		{
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [Item, this]() { RemoveItemEffect(Item); }, 5.0f, false);
		}
		

		break;

	// The Item in and of itself doesn't give you anything. It's the filling of its capacity that does.
	case EItemType::BeverageContainer:


		LocalStatusesNeed = Character->StatusComponent->GetStatusesNeed();

		PointToConsume = GetBeverageCapacityUseBasedOnStatusNeed(LocalItem.CapacityModifier, LocalStatusesNeed);

		if (!PointToConsume == 0.0f)
		{
			RemoveItem(LocalItem, 1);

			LocalItem.CapacityModifier = RemoveAndApplyBeverageModifier(LocalItem.CapacityModifier, PointToConsume, true);

			AddItemToInventory(LocalItem, 1);
		}

		break;

	case EItemType::Weapon:

		if (LocalItem.Active == 1)
		{

			CalculateCharactersModifiers(LocalModifierBonuses, 1);

			RemoveItem(LocalItem, 1);

			FSItem* EquipmentItem;
			EquipmentItem = Equipment.Find(ItemDataFromDT->EquipmentType);
			if (*EquipmentItem == LocalItem)
			{
				Equipment.Add(ItemDataFromDT->EquipmentType, NoneItem);
			}

			LocalItem.Active = 0;

			AddItemToInventory(LocalItem, 1);
		}
		else
		{

			CalculateCharactersModifiers(LocalModifierBonuses);

			if (LocalValue[LocalIndex] > 1)
			{
				NewItemQuantity = LocalValue[LocalIndex] - 1;
				Inventory.Add(LocalItems[LocalIndex], NewItemQuantity);
				LocalItem.Active = 1;
				Inventory.Add(LocalItem, 1);
			}
			else
			{
				RemoveItem(LocalItem, 1);
				LocalItem.Active = 1;
				AddItemToInventory(LocalItem, 1);
			}
			
			FSItem* EquipmentItem;
			EquipmentItem = Equipment.Find(ItemDataFromDT->EquipmentType);
			
			if (EquipmentItem->ID != "None")
			{
				UseItem(*EquipmentItem);

			}
			
			TArray<FSItem> LocalItemsForOutput;
			Inventory.GenerateKeyArray(LocalItemsForOutput);

			FSItem ItemForREF;
			ItemForREF = LocalItemsForOutput[LocalItemsForOutput.Find(LocalItem)];

			EquipmentItem = Equipment.Find(EEquipmentType::MeleeWeapon);
			Equipment.Add(ItemDataFromDT->EquipmentType, LocalItem);
		}

		break;
	case EItemType::Armour:

		if (LocalItem.Active == 1)
		{

			CalculateCharactersModifiers(LocalModifierBonuses, 1);

			RemoveItem(LocalItem, 1);

			FSItem* EquipmentItem;
			EquipmentItem = Equipment.Find(ItemDataFromDT->EquipmentType);
			if (*EquipmentItem == LocalItem)
			{
				Equipment.Add(ItemDataFromDT->EquipmentType, NoneItem);
			}

			LocalItem.Active = 0;

			AddItemToInventory(LocalItem, 1);
		}
		else
		{
			CalculateCharactersModifiers(LocalModifierBonuses);

			if (LocalValue[LocalIndex] > 1)
			{
				NewItemQuantity = LocalValue[LocalIndex] - 1;
				Inventory.Add(LocalItems[LocalIndex], NewItemQuantity);
				LocalItem.Active = 1;
				Inventory.Add(LocalItem, 1);
			}
			else
			{
				RemoveItem(LocalItem, 1);
				LocalItem.Active = 1;
				AddItemToInventory(LocalItem, 1);
			}

			FSItem* EquipmentItem;
			EquipmentItem = Equipment.Find(ItemDataFromDT->EquipmentType);

			if (EquipmentItem->ID != "None")
			{
				UseItem(*EquipmentItem);
			}

			TArray<FSItem> LocalItemsForOutput;
			Inventory.GenerateKeyArray(LocalItemsForOutput);

			FSItem ItemForREF;
			ItemForREF = LocalItemsForOutput[LocalItemsForOutput.Find(LocalItem)];

			EquipmentItem = Equipment.Find(EEquipmentType::MeleeWeapon);
			Equipment.Add(ItemDataFromDT->EquipmentType, LocalItem);
		}

		break;

	default:
		break;
	}

	if ((!Character->IsBotControlled()))
	{
		Cast<ATerraPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->CharacterMenu->UpdateStats();
	}
}

void UInventoryComponent::UseHighPriorityItemByTag(FName LocalName, FName AdditionalTag)
{
	if (LocalName == "Food")
	{
		TArray<FSItem> LocalItems;
		FSItem LocalItem;

		Inventory.GenerateKeyArray(LocalItems);

		LocalItems = FindItemArrayWithModifier(LocalItems, AdditionalTag);
		if (!LocalItems.IsEmpty())
		{
			LocalItem = FindItemWithHighsetDelicious(LocalItems);
			UseItem(LocalItem);
		}
	}
}

void UInventoryComponent::RemoveItemEffect(FSItem LocalItem)
{
	TMap<FName, float> LocalModifierBonuses = CalculateModifiersBasedOnDurability(LocalItem);
	CalculateCharactersModifiers(LocalModifierBonuses, 1);
}

////////////// RECALCULATE MODIFERS //////////////

void UInventoryComponent::RecalculateInventoryModifierBonuses()
{
	if (!Inventory.IsEmpty())
	{
		TArray<FSItem> ItemsToDeleting;
		TMap<FSItem, int> ItemsToAdd;
		for (auto& LocalItem : Inventory)
		{
			ItemsToDeleting.Add(LocalItem.Key);

			FSItem NewItem;
			NewItem = LocalItem.Key;

			NewItem = CalculateModifierBonuses(NewItem);

			ItemsToAdd.Add(NewItem, LocalItem.Value);
		}


		for (auto& Item : ItemsToDeleting)
		{
			if (Item.Active == 1)
			{
				FSItemCreation* ItemDataFromDT;
				ItemDataFromDT = DT_Items->FindRow<FSItemCreation>(Item.ID, TEXT("none"), false);

				Equipment.Add(ItemDataFromDT->EquipmentType, NoneItem);
			}
			RemoveItem(Item, *Inventory.Find(Item));
		}


		for (auto& Pair : ItemsToAdd)
		{
			if (Pair.Key.Active == 1)
			{
				FSItemCreation* ItemDataFromDT;
				ItemDataFromDT = DT_Items->FindRow<FSItemCreation>(Pair.Key.ID, TEXT("none"), false);

				Equipment.Add(ItemDataFromDT->EquipmentType, Pair.Key);
			}
			AddItemToInventory(Pair.Key, Pair.Value);
		}
	}
}


FSItem UInventoryComponent::CalculateModifierBonuses(FSItem Item)
{
	FSItem NewItem;
	NewItem = Item;

	FSItemCreation* ItemDataFromDT;
	ItemDataFromDT = DT_Items->FindRow<FSItemCreation>(Item.ID, TEXT("none"), false);

	if (Item.Active == 1)
	{
		TArray<FName> LocalKeys;
		Item.ModifierBonuses.GenerateKeyArray(LocalKeys);
		for (auto& LocalModifier : LocalKeys)
		{
			float Value;
			Value = *Character->Modifiers.Find(LocalModifier) - *Item.ModifierBonuses.Find(LocalModifier);

			Character->Modifiers.Add(LocalModifier, Value);
		}
	}

	NewItem.ModifierBonuses = CalculateModifiersBasedOnDurability(NewItem);

	float BonusesModifier;
	BonusesModifier = 1.0f;

	if (!ItemDataFromDT->LevelRequirement.IsEmpty())
	{
		BonusesModifier *= CalculateModifiersBasedOnLevelRequirement(NewItem);

		for (auto& Pair : NewItem.ModifierBonuses)
		{
			float Value;
			Value = Pair.Value * BonusesModifier;

			NewItem.ModifierBonuses.Add(Pair.Key, Value);
		}
	}

	if (Item.Active == 1)
	{
		TArray<FName> LocalKeys;
		NewItem.ModifierBonuses.GenerateKeyArray(LocalKeys);
		for (auto& LocalModifier : LocalKeys)
		{
			float Value;
			Value = *Character->Modifiers.Find(LocalModifier) + *NewItem.ModifierBonuses.Find(LocalModifier);

			Character->Modifiers.Add(LocalModifier, Value);
		}
	}

	return NewItem;
}

float UInventoryComponent::CalculateModifiersBasedOnLevelRequirement(FSItem Item)
{
	float OutModifier;
	OutModifier = 1;

	FSItemCreation* ItemDataFromDT;
	ItemDataFromDT = DT_Items->FindRow<FSItemCreation>(Item.ID, TEXT("none"), false);

	if (!Character->ProgressionComponent->IsAttributesOnLevel(ItemDataFromDT->LevelRequirement))
	{
		OutModifier = Character->ProgressionComponent->GetAttributeOnLevelModifier(ItemDataFromDT->LevelRequirement);
	}

	return OutModifier;
}

// Return ModifierBonuses based on durability and static also
TMap<FName,float> UInventoryComponent::CalculateModifiersBasedOnDurability(FSItem Item)
{
	FSItemCreation* ItemDataFromDT;
	ItemDataFromDT = DT_Items->FindRow<FSItemCreation>(Item.ID, TEXT("none"), false);

	FSItem LocalItem;
	LocalItem = Item;

	float ItemDurability;
	ItemDurability = Item.Durability;
	float MaxDurability;
	MaxDurability = ItemDataFromDT->MaxDurability;

	float Percent;
	Percent = ItemDurability / MaxDurability;


	LocalItem.ModifierBonuses.Empty();

	
	if (!ItemDataFromDT->ModifierBonuses.IsEmpty())
	{
		if (ItemDataFromDT->ModifierBonusesOnLowDurability.IsEmpty())
		{
			for (auto& Pair : ItemDataFromDT->ModifierBonuses)
			{
				float Value;
				Value = Pair.Value * Percent;

				LocalItem.ModifierBonuses.Add(Pair.Key, Value);
			}
		}
		else
		{
			TMap<FName, float> LocalModifierBonuses;

			for (auto& Pair : ItemDataFromDT->ModifierBonuses)
			{
				if (ItemDataFromDT->ModifierBonusesOnLowDurability.Contains(Pair.Key))
				{
					LocalModifierBonuses.Add(Pair.Key, Pair.Value - *ItemDataFromDT->ModifierBonusesOnLowDurability.Find(Pair.Key));
				}
				else
				{
					LocalModifierBonuses.Add(Pair.Key, Pair.Value);
				}
			}
			for (auto& Pair : LocalModifierBonuses)
			{
				LocalItem.ModifierBonuses.Add(Pair.Key, Pair.Value * Percent + *ItemDataFromDT->ModifierBonusesOnLowDurability.Find(Pair.Key));
			}
		}
	}
	
	if (!ItemDataFromDT->StaticModifierBonuses.IsEmpty())
	{
		for (auto& Pair : ItemDataFromDT->StaticModifierBonuses)
		{
			float Value;
			if (LocalItem.ModifierBonuses.Contains(Pair.Key))
			{
				Value = *LocalItem.ModifierBonuses.Find(Pair.Key) + Pair.Value;
			}
			else
			{
				Value = Pair.Value;
			}

			LocalItem.ModifierBonuses.Add(Pair.Key, Value);
		}
	}

	return LocalItem.ModifierBonuses;
}

// Upgrade Characters Modifiers based on items
void UInventoryComponent::CalculateCharactersModifiers(TMap<FName, float> ModifierBonuses, bool bSubtractInsteadOfAdd)
{
	for (auto& LocalModifierPair : ModifierBonuses)
	{
		
		if (Character->StatusComponent->Statuses.Contains(LocalModifierPair.Key))
		{
			float Value;
			float PREVValue = *Character->Modifiers.Find(LocalModifierPair.Key);
			Value = *Character->Modifiers.Find(LocalModifierPair.Key) + LocalModifierPair.Value;

			FString LocalString = LocalModifierPair.Key.ToString();
			LocalString.Append("Max");
			Value = FMath::Clamp(Value, 0.0f, *Character->Modifiers.Find(*LocalString));

			

			if (PREVValue != Value)
			{
				Character->StatusComponent->RemoveStatusModifiers(LocalModifierPair.Key, *Character->Modifiers.Find(LocalModifierPair.Key));
				Character->StatusComponent->AddStatusModifiers(LocalModifierPair.Key, Value);

				Character->Modifiers.Add(LocalModifierPair.Key, Value);

				if (Character->GetController()->IsPlayerController())
				{
					Cast<ATerraPlayerController>(Character->GetController())->HUD->UpdateStatusPB();
				}
			}
		}
		else if (LocalModifierPair.Key.ToString().Contains("DecayBonusValue"))
		{
			if (bSubtractInsteadOfAdd)
			{
				float Value;
				Value = *Character->Modifiers.Find(LocalModifierPair.Key) - LocalModifierPair.Value;

				Character->Modifiers.Add(LocalModifierPair.Key, Value);

				Character->StatusComponent->UpdateStatuses();
			}
			else
			{
				float Value;
				Value = *Character->Modifiers.Find(LocalModifierPair.Key) + LocalModifierPair.Value;

				Character->Modifiers.Add(LocalModifierPair.Key, Value);

				Character->StatusComponent->UpdateStatuses();
			}
		}
		else if (LocalModifierPair.Key == "Delicious")
		{
		}
		else
		{
			if (bSubtractInsteadOfAdd)
			{
				float Value;
				Value = *Character->Modifiers.Find(LocalModifierPair.Key) - LocalModifierPair.Value;

				Character->Modifiers.Add(LocalModifierPair.Key, Value);
			}
			else
			{
				float Value;
				Value = *Character->Modifiers.Find(LocalModifierPair.Key) + LocalModifierPair.Value;

				Character->Modifiers.Add(LocalModifierPair.Key, Value);
			}
		}
	}
}

// Return only TMap of ModifierBonuses, based in seconds. Same function that "CalculateModifiersBasedOnDurability", but with time
TMap<FName, float> UInventoryComponent::CalculateItemModifiersDurabilityBasedOnSeconds(FName ItemID, float Seconds)
{
	FSItem LocalItem;
	LocalItem =	CreateItem(ItemID, 0, true);

	LocalItem.Durability -= Seconds;
	if (LocalItem.Durability < 0)
	{
		LocalItem.Durability = 0;
	}

	return CalculateModifiersBasedOnDurability(LocalItem);
}

TMap<FName, float> UInventoryComponent::GetUnitedModifierBonusesByItemID(FName ItemID)
{
	FSItemCreation* ItemDataFromDT;
	ItemDataFromDT = DT_Items->FindRow<FSItemCreation>(ItemID, TEXT("none"), false);

	TMap<FName, float> Local;

	Local = ItemDataFromDT->ModifierBonuses;

	if (!ItemDataFromDT->StaticModifierBonuses.IsEmpty())
	{
		for (auto& LocalPair : ItemDataFromDT->StaticModifierBonuses)
		{
			if (Local.Contains(LocalPair.Key))
			{
				Local.Add(LocalPair.Key, LocalPair.Value + *Local.Find(LocalPair.Key));
			}
			else
			{
				Local.Add(LocalPair.Key, LocalPair.Value);
			}
		}
	}

	return Local;
}

// Replace Item
FSItem UInventoryComponent::AddBeverage(FSItem Item, FName BeverageID, float Value)
{
	FSItem LocalItem;
	LocalItem = Item;

	RemoveItem(Item, 1);

	UE_LOG(LogTemp, Error, TEXT("AddBeverage: Start"));


	
	if (LocalItem.CapacityModifier.Contains(BeverageID))
	{
		LocalItem.CapacityModifier.Add(BeverageID, Value + *LocalItem.CapacityModifier.Find(BeverageID));

		UE_LOG(LogTemp, Error, TEXT("AddBeverage: Contains"));
	}
	else
	{
		LocalItem.CapacityModifier.Add(BeverageID, Value);

		UE_LOG(LogTemp, Error, TEXT("AddBeverage: Dont Contains"));
	}

	UE_LOG(LogTemp, Warning, TEXT("AddBeverage: CapacityModifier Num %d"), LocalItem.CapacityModifier.Num());

	AddItemToInventory(LocalItem, 1);

	return LocalItem;
}

// Replace Item
FSItem UInventoryComponent::AddSeveralBeverage(FSItem Item, TMap<FName, float> Beverage, bool bIgnoreCargo)
{
	FSItem LocalItem;
	LocalItem = Item;

	RemoveItem(Item, 1, bIgnoreCargo);

	for (auto& Pair : Beverage)
	{
		if (LocalItem.CapacityModifier.Contains(Pair.Key))
		{
			LocalItem.CapacityModifier.Add(Pair.Key, Pair.Value + *LocalItem.CapacityModifier.Find(Pair.Key));

			UE_LOG(LogTemp, Error, TEXT("AddSeveralBeverage: Contains"));
		}
		else
		{
			LocalItem.CapacityModifier.Add(Pair.Key, Pair.Value);

			UE_LOG(LogTemp, Error, TEXT("AddSeveralBeverage: Dont Contains"));
		}

		UE_LOG(LogTemp, Warning, TEXT("AddSeveralBeverage: CapacityModifier Num %d"), LocalItem.CapacityModifier.Num());
	}
	UE_LOG(LogTemp, Warning, TEXT("AddSeveralBeverage: CapacityModifier Num %d"), LocalItem.CapacityModifier.Num());

	AddItemToInventory(LocalItem, 1, bIgnoreCargo);

	return LocalItem;
}


TMap<FName, float> UInventoryComponent::RemoveAndApplyBeverageModifier(TMap<FName,float> CapacityModifier, float BeverageValue, bool bApplyModifiers)
{
	float OverallCapacity = 0.0f;
	TMap<FName, float> CapacityPercentage;
	TMap<FName, float> LocalCapacity;
	TMap<FName, float> OutputCapacity;
	TMap<FName, float> LocalModifierBonuses;

	for (auto& Pair : CapacityModifier)
	{
		OverallCapacity += Pair.Value;
	}

	

	for (auto& Pair : CapacityModifier)
	{
		CapacityPercentage.Add(Pair.Key, Pair.Value / OverallCapacity);
	}

	for (auto& Pair : CapacityPercentage)
	{
		LocalCapacity.Add(Pair.Key, Pair.Value * BeverageValue);
	}

	for (auto& Pair : LocalCapacity)
	{
		OutputCapacity.Add(Pair.Key, *CapacityModifier.Find(Pair.Key) - Pair.Value);

		FSBeverageCreation* BeverageDataFromDT;
		BeverageDataFromDT = DT_Beverage->FindRow<FSBeverageCreation>(Pair.Key, TEXT("none"), false);

		if (bApplyModifiers)
		{
			for (auto& LocalPair : BeverageDataFromDT->ModifierBonuses)
			{
				LocalModifierBonuses.Add(LocalPair.Key, LocalPair.Value * Pair.Value);
			}
		}
	}

	if (bApplyModifiers)
	{
		CalculateCharactersModifiers(LocalModifierBonuses);
	}

	return OutputCapacity;
}

///////////////////////////////////////////////////


////////////////////// FIND ///////////////////////

FSItem UInventoryComponent::FindItemWithHighsetDelicious(TArray<FSItem> ItemArray)
{
	float LocalValue = -1.0f;
	FSItem OutputItem;

	for (auto& LocalItem : ItemArray)
	{
		if (LocalValue < *LocalItem.ModifierBonuses.Find("Delicious"))
		{
			LocalValue = *LocalItem.ModifierBonuses.Find("Delicious");
			OutputItem = LocalItem;
		}
	}

	return OutputItem;
}

FSItem UInventoryComponent::FindItemByID(FName LocalID, TArray<FSItem> ItemArray)
{
	FSItem OutputItem;

	for (auto& LocalItem : ItemArray)
	{
		if (LocalItem.ID == LocalID)
		{
			return LocalItem;
		}
	}

	return NoneItem;
}

TArray<FSItem> UInventoryComponent::FindItemArrayWithModifier(TArray<FSItem> ItemArray, FName Modifier)
{
	TArray<FSItem> LocalItemArray;

	for (auto& LocalItem : ItemArray)
	{
		if (LocalItem.ModifierBonuses.Contains(Modifier))
		{
			LocalItemArray.Add(LocalItem);
		}
	}

	return LocalItemArray;
}

float UInventoryComponent::GetBeverageCapacityUseBasedOnStatusNeed(TMap<FName,float> Capacity, TMap<FName, float> StatusNeed)
{
	TArray<FName> LocalStatusesNeed;
	UE_LOG(LogTemp, Warning, TEXT("Get Beverage Capacity Use: StatusNeed Num = %d"), StatusNeed.Num());
	TMap<FName, float> ModifierBonusesForOnePoint;
	TMap<FName, float> OverallModifierBonuses;
	TMap<FName, float> LocalStatusNeedValues;

	float OverallCapacty = 0.0f;

	for (auto& Pair : Capacity)
	{
		FSBeverageCreation* BeverageDataFromDT;
		BeverageDataFromDT = DT_Beverage->FindRow<FSBeverageCreation>(Pair.Key, TEXT("none"), false);

		OverallCapacty += Pair.Value;
		
		for (auto& LocalPair : BeverageDataFromDT->ModifierBonuses)
		{
			if (StatusNeed.Contains(LocalPair.Key))
			{
				LocalStatusesNeed.Add(LocalPair.Key);
			}

			OverallModifierBonuses.Add(LocalPair.Key, LocalPair.Value * Pair.Value);
			UE_LOG(LogTemp, Warning, TEXT("Get Beverage Capacity Use: OverallModifierBonuses Water 1 = %f"), LocalPair.Value * Pair.Value);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Get Beverage Capacity Use: OverallCapacty = %f"), OverallCapacty);

	if (OverallCapacty == 0.0f)
	{
		return 0.0f;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Get Beverage Capacity Use: OverallModifierBonuses Num = %d"), OverallModifierBonuses.Num());

		UE_LOG(LogTemp, Warning, TEXT("Get Beverage Capacity Use: OverallModifierBonuses Water 2 = %f"), OverallModifierBonuses.Find("Thirst"));

		for (auto& Pair : OverallModifierBonuses)
		{
			ModifierBonusesForOnePoint.Add(Pair.Key, Pair.Value / OverallCapacty);
		}

		UE_LOG(LogTemp, Warning, TEXT("Get Beverage Capacity Use: ModifierBonusesForOnePoint Num = %d"), ModifierBonusesForOnePoint.Num());

		for (auto& Status : LocalStatusesNeed)
		{
			LocalStatusNeedValues.Add(Status, *StatusNeed.Find(Status));

			UE_LOG(LogTemp, Warning, TEXT("Get Beverage Capacity Use: LocalStatusesNeed Num = %d"), LocalStatusesNeed.Num());
		}

		float PointOutput = -1.0f;

		UE_LOG(LogTemp, Warning, TEXT("Get Beverage Capacity Use: LocalStatusNeedValues Num = %d"), LocalStatusNeedValues.Num());

		for (auto& Pair : LocalStatusNeedValues)
		{
			UE_LOG(LogTemp, Warning, TEXT("Get Beverage Capacity Use: Last Check = %f"), Pair.Value / *ModifierBonusesForOnePoint.Find(Pair.Key));

			if (Pair.Value / *ModifierBonusesForOnePoint.Find(Pair.Key) < PointOutput || PointOutput == -1.0f)
			{
				PointOutput = Pair.Value / *ModifierBonusesForOnePoint.Find(Pair.Key);
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("Get Beverage Capacity Use: PointOutput Max = %f"), FMath::Max(PointOutput, 0.0f));

		return FMath::Max(PointOutput, 0.0f);
	}
}

float UInventoryComponent::GetBeverageCapacityFree(FSItem LocalItem)
{
	if (LocalItem.ModifierBonuses.Contains("Capacity"))
	{
		float LocalCapacity;
		LocalCapacity = *LocalItem.ModifierBonuses.Find("Capacity");

		float OverallValue = 0.0f;

		for (auto& LocalPair : LocalItem.CapacityModifier)
		{
			OverallValue += LocalPair.Value;
		}

		return LocalCapacity - OverallValue;
	}
	else
	{
		return -1.0f;
	}
}

float UInventoryComponent::GetBeverageCapacityFill(FSItem LocalItem)
{
	if (LocalItem.ModifierBonuses.Contains("Capacity"))
	{
		float OverallValue = 0.0f;

		for (auto& LocalPair : LocalItem.CapacityModifier)
		{
			OverallValue += LocalPair.Value;
		}

		return OverallValue;
	}
	else
	{
		return -1.0f;
	}
}


///////////////////////////////////////////////////

void UInventoryComponent::RecalculateCargoBasedOnItem(FName ID, int PrevQuantity, int NewQuantity)
{
	FSItemCreation* ItemDataFromDT;
	ItemDataFromDT = DT_Items->FindRow<FSItemCreation>(ID, TEXT("none"), false);

	float LocalWeight;
	LocalWeight = *Character->Modifiers.Find("Weight");
	LocalWeight -= ItemDataFromDT->Weight * PrevQuantity; 
	LocalWeight += ItemDataFromDT->Weight * NewQuantity;

	Character->Modifiers.Add("Weight", LocalWeight);

	float NewModifier;
	NewModifier = *Character->Modifiers.Find("Cargo") / *Character->Modifiers.Find("Weight");
	Character->Modifiers.Add("MovementSpeedCargoModifier", NewModifier);
	Character->UpdateCharacterMovement();
}

void UInventoryComponent::Init()
{
	TArray<TEnumAsByte<EEquipmentType>> LocalEnums;
	LocalEnums.Add(EEquipmentType::Helmet);
	LocalEnums.Add(EEquipmentType::MeleeWeapon);

	for (auto& LocalEnum : LocalEnums)
	{
		Equipment.Add(LocalEnum, NoneItem);
	}

	TArray<FName> Rows;
	Rows = DT_Items->GetRowNames();
	for (auto& Row : Rows)
	{
		FSItemCreation* ItemDataFromDT;
		ItemDataFromDT = DT_Items->FindRow<FSItemCreation>(Row, TEXT("none"), false);

		if (ItemDataFromDT->IsDurabilityFreshness)
		{
			IDThatRot.Add(Row);
		}
	}
}

void UInventoryComponent::OnSecondCalculations()
{
	
	TArray<FSItem> ItemToDelete;
	TMap<FSItem, int> ItemToAdd;

	for (auto& ItemMap : Inventory)
	{
		if (IDThatRot.Contains(ItemMap.Key.ID) && ItemMap.Key.Durability > 0)
		{
			ItemToDelete.Add(ItemMap.Key);
			
			FSItem NewItem;
			NewItem = ItemMap.Key;
			NewItem.Durability--;

			UE_LOG(LogTemp, Error, TEXT("OnSecondCalculations: Rot"));
			ItemToAdd.Add(NewItem, ItemMap.Value);
		}
	}

	if (!ItemToDelete.IsEmpty())
	{
		for (auto& Item : ItemToDelete)
		{
			RemoveItem(Item, *Inventory.Find(Item), 1);
		}
	}
	
	if (!ItemToAdd.IsEmpty())
	{
		for (auto& Pair : ItemToAdd)
		{
			AddItemToInventory(Pair.Key, Pair.Value, 1);
		}
	}
}
