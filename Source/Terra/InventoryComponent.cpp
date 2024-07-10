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
	LocalItem.ModifierBonuses = ItemDataFromDT->ModifierBonuses;
	LocalItem.Durability = ItemDataFromDT->MaxDurability;
	LocalItem.Active = 0;
	
	AddItemToInventory(LocalItem, Quantity, bIgnoreCargo);

	return LocalItem;
}

void UInventoryComponent::RemoveItem(FSItem Item, int Quantity, bool bIgnoreCargo)
{

	FSItem LocalItem;
	LocalItem = Item;

	int LocalInt;
	LocalInt = *Inventory.Find(LocalItem);

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
		LocalInt = *Inventory.Find(Item) - Quantity;
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

	TArray<FName> LocalKeys;

	TArray<FSItem> LocalItems;
	Inventory.GenerateKeyArray(LocalItems);

	TArray<int> LocalValue;
	Inventory.GenerateValueArray(LocalValue);

	uint8 LocalIndex;
	LocalIndex = LocalItems.Find(Item);

	FSItem LocalItem;
	LocalItem = Item;

	int NewItemQuantity;

	switch (ItemDataFromDT->Type)
	{
	case EItemType::Food:

		RemoveItem(Item, 1);
		Item.ModifierBonuses.GenerateKeyArray(LocalKeys);

		CalculateModifiers(Item.ModifierBonuses);
		
		if (ItemDataFromDT->EffectDuration > 0)
		{
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [Item, this]() { RemoveItemEffect(Item); }, 5.0f, false);
		}
		

		break;

	case EItemType::Weapon:

		if (LocalItem.Active == 1)
		{

			CalculateModifiers(Item.ModifierBonuses, 1);

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

			LocalItem.Active = 1;

			CalculateModifiers(Item.ModifierBonuses);

			if (LocalValue[LocalIndex] > 1)
			{
				NewItemQuantity = LocalValue[LocalIndex] - 1;
				Inventory.Add(LocalItems[LocalIndex], NewItemQuantity);
				Inventory.Add(LocalItem, 1);
			}
			else
			{
				RemoveItem(LocalItem, 1);
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

			CalculateModifiers(Item.ModifierBonuses, 1);

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

			LocalItem.Active = 1;

			CalculateModifiers(Item.ModifierBonuses);

			if (LocalValue[LocalIndex] > 1)
			{
				NewItemQuantity = LocalValue[LocalIndex] - 1;
				Inventory.Add(LocalItems[LocalIndex], NewItemQuantity);
				Inventory.Add(LocalItem, 1);
			}
			else
			{
				RemoveItem(LocalItem, 1);
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
	CalculateModifiers(LocalItem.ModifierBonuses, 1);
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

	NewItem = CalculateModifiersBasedOnDurability(NewItem);

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

FSItem UInventoryComponent::CalculateModifiersBasedOnDurability(FSItem Item)
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
	if (ItemDataFromDT->bStaticBonuses)
	{
		LocalItem.ModifierBonuses = ItemDataFromDT->ModifierBonuses;
	}
	else if	(ItemDataFromDT->ModifierBonusesOnLowDurability.IsEmpty())
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
		if (Percent > 0.5)
		{
			for (auto& Pair : ItemDataFromDT->ModifierBonuses)
			{
				float Value;
				Value = Pair.Value * ((Percent - 0.5) / 0.5);

				LocalItem.ModifierBonuses.Add(Pair.Key, Value);
			}
		}
		else
		{
			for (auto& Pair : ItemDataFromDT->ModifierBonusesOnLowDurability)
			{
				float Value;
				Value = Pair.Value * ((0.5 - Percent) / 0.5);

				LocalItem.ModifierBonuses.Add(Pair.Key, Value);
			}
		}
	}

	return LocalItem;
}

void UInventoryComponent::CalculateModifiers(TMap<FName, float> ModifierBonuses, bool bSubtractInsteadOfAdd)
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
			NewItem = CalculateModifierBonuses(NewItem);

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
