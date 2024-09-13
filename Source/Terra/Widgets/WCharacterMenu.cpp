// Fill out your copyright notice in the Description page of Project Settings.


#include "WCharacterMenu.h"
#include "Components/WrapBox.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"

void UWCharacterMenu::NativeConstruct()
{
	Super::NativeConstruct();
}

void UWCharacterMenu::ChangeUIStatus()
{
	if (StatusUI)
	{
		this->SetVisibility(ESlateVisibility::Visible);
		Cast<APlayerController>(Character->GetController())->SetPause(1);
		StatusUI = 0;
		UpdateUI();
	}
	else
	{
		this->SetVisibility(ESlateVisibility::Collapsed);
		Cast<APlayerController>(Character->GetController())->SetPause(0);
		UnSelectedItem();
		LocalMenuInventory.Empty();
		StatusUI = 1;
	}
}

void UWCharacterMenu::UpdateUI()
{
	BUT_Chest->SetVisibility(ESlateVisibility::Hidden);
	BUT_Skills->SetVisibility(ESlateVisibility::Visible);
	bIsChestOpen = false;
	CreateAttributeWB();
	CreateInventoryWB(Character->InventoryComponent->Inventory);
	ChangeUIVisibleParts(0);

	UpdateStats();
}

void UWCharacterMenu::UpdateStats()
{
	FString LocalString;
	LocalString.Append(FString::SanitizeFloat(*Character->Modifiers.Find("Weight")));
	LocalString.Append("/");
	LocalString.Append(FString::SanitizeFloat(*Character->Modifiers.Find("Cargo")));

	Cargo->SetText(FText::FromString(LocalString));
}

void UWCharacterMenu::CreateAttributeWB()
{
	if (AttributeSizeBoxClass)
	{
		WB_Attributes->ClearChildren();
		for (auto& LocalAttribute : Character->ProgressionComponent->Attributes)
		{
			AttributeSizeBox = CreateWidget<UWAttributeSizeBox>(this, AttributeSizeBoxClass);
			AttributeSizeBox->Init(&LocalAttribute, Character->DT_Attributes, Character->DT_Skills, Character->ProgressionComponent->NewLevelXPRequired);
			AttributeSizeBox->Parent = this;
			WB_Attributes->AddChildToWrapBox(AttributeSizeBox);
		}	
	}
}

void UWCharacterMenu::CreateInventoryWB(TMap<FSItem, int> LocalInventory)
{
	if (InventorySizeBoxClass)
	{
		WB_Inventory->ClearChildren();
		UE_LOG(LogTemp, Warning, TEXT("Yeah1"));

		if (!LocalInventory.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("Yeah2"));
			TArray<FSItem> LocalItems;
			LocalInventory.GenerateKeyArray(LocalItems);


			for (auto& LocalItem : LocalItems)
			{
				UE_LOG(LogTemp, Warning, TEXT("Yeah3"));
				UE_LOG(LogTemp, Warning, TEXT("InventoryWB: Modifier Bonuses Num %d"), LocalItem.ModifierBonuses.Num());
				InventorySizeBox = CreateWidget<UWInventorySizeBox>(this, InventorySizeBoxClass);
				InventorySizeBox->Init(&LocalItem, *LocalInventory.Find(LocalItem), Character->InventoryComponent->DT_Items);
				InventorySizeBox->Parent = this;
				WB_Inventory->AddChildToWrapBox(InventorySizeBox);
			}
		}
	}
}

void UWCharacterMenu::CreateObjectInventoryWB()
{
	CreateInventoryWB(InteractableActor->InventoryComponent->Inventory);
}

void UWCharacterMenu::UpdateAttributeWB()
{
	for (auto& LocalChild : WB_Attributes->GetAllChildren())
	{
		Cast<UWAttributeSizeBox>(LocalChild)->UpdateSkillsWB();
	}
}

void UWCharacterMenu::ChangeUIVisibleParts(uint8 Status)
{
	// 0 - Inventory
	// 1 - Skills
	// 2 - Chest
	// 3 - Another Shit, for example = fill capacity, pour out of the capacity etc.

	BUT_Left->SetVisibility(ESlateVisibility::Hidden);
	BUT_Right->SetVisibility(ESlateVisibility::Hidden);
	FString LocalString;

	switch (Status)
	{
	case 0: 
		InventoryMenu->SetVisibility(ESlateVisibility::Visible);
		SkillMenu->SetVisibility(ESlateVisibility::Hidden);
		CreateInventoryWB(Character->InventoryComponent->Inventory);
		UnSelectedItem();
		bIsChest = false;
		MenuStatus = 0;

		break;
	case 1:
		SkillMenu->SetVisibility(ESlateVisibility::Visible);
		InventoryMenu->SetVisibility(ESlateVisibility::Hidden);
		UnSelectedItem();
		MenuStatus = 1;

		break;
	case 2:
		SkillMenu->SetVisibility(ESlateVisibility::Hidden);
		BUT_Skills->SetVisibility(ESlateVisibility::Hidden);
		BUT_Chest->SetVisibility(ESlateVisibility::Visible);
		CreateObjectInventoryWB();
		UnSelectedItem();
		bIsChest = true;
		bIsChestOpen = true;
		MenuStatus = 2;

		break;

	case 3:
		InventoryMenu->SetVisibility(ESlateVisibility::Visible);
		SkillMenu->SetVisibility(ESlateVisibility::Hidden);
		BUT_Skills->SetVisibility(ESlateVisibility::Hidden);
		BUT_Inventory->SetVisibility(ESlateVisibility::Hidden);
		CreateInventoryWB(LocalMenuInventory);
		UnSelectedItem();
		bIsChest = false;
		MenuStatus = 3;

		if (WindowID == "BeverageCapacityIA")
		{
			LocalString = "Pour in IN";
			BUT_Left->SetVisibility(ESlateVisibility::Visible);
			TXT_Left->SetText(FText::FromString(LocalString));
			BUT_Right->SetVisibility(ESlateVisibility::Visible);

			LocalString = "Pour from IA";
			TXT_Right->SetText(FText::FromString(LocalString));
		}

		break;
	default:
		break;
	}

	SetDesc();
}

void UWCharacterMenu::ChangeSelectedItem(UWInventorySizeBox* NewSelectedItemSB)
{
	if (SelectedInventorySizeBox)
	{
		SelectedInventorySizeBox->ChangeButtonStatus(0);
	}
	SelectedInventorySizeBox = NewSelectedItemSB;

	if (MenuStatus == 3)
	{
		InteractableActor->InteractionComponent->LocalItem = SelectedItemStruct;
		InteractableActor->InteractionComponent->AfterInteractionEffect(Character);
		ChangeUIStatus();
	}
	else
	{
		SelectedInventorySizeBox->ChangeButtonStatus(1);
	}
	
}

void UWCharacterMenu::UnSelectedItem()
{
	if (SelectedInventorySizeBox)
	{
		SelectedInventorySizeBox->ChangeButtonStatus(0);
		SelectedInventorySizeBox = NULL;
		FSItem LocalStruct;
		LocalStruct.Durability = 0;
		SelectedItemStruct = LocalStruct;
	}
}

void UWCharacterMenu::SetDesc()
{
	if (WindowID == "BeverageCapacityIA")
	{
		TArray<FSItem> LocalItemArray;
		InteractableActor->InventoryComponent->Inventory.GenerateKeyArray(LocalItemArray);

		FString LocalString;
		
		for (auto& Pair : LocalItemArray[0].CapacityModifier)
		{
			LocalString.Append(Pair.Key.ToString());
			LocalString.Append(" - ");
			LocalString.Append(FString::SanitizeFloat(Pair.Value));
			LocalString.Append(" (");
			float LocalValue = Pair.Value / *LocalItemArray[0].ModifierBonuses.Find("capacity") * 100.0f;
			LocalString.Append(FString::SanitizeFloat(LocalValue));
			LocalString.Append("%)\n");
		}
		TXT_Desc->SetText(FText::FromString(LocalString));
		UE_LOG(LogTemp, Warning, TEXT("SetDesc: BeverageCapacityIA - %d"), LocalItemArray[0].CapacityModifier.Num());
	}
}

void UWCharacterMenu::LeftRightButtonEffect(uint8 ButtonID)
{
	switch (ButtonID)
	{
	default:
		float LocalValue;

		break;

	case 0:

		InventoryMenu->SetVisibility(ESlateVisibility::Visible);
		SkillMenu->SetVisibility(ESlateVisibility::Hidden);
		BUT_Skills->SetVisibility(ESlateVisibility::Hidden);
		BUT_Inventory->SetVisibility(ESlateVisibility::Hidden);
		BUT_Left->SetVisibility(ESlateVisibility::Visible);
		BUT_Right->SetVisibility(ESlateVisibility::Visible);

		UnSelectedItem();
		bIsChest = false;

		if (WindowID == "BeverageCapacityIA")
		{
			LocalMenuInventory.Empty();
			UE_LOG(LogTemp, Warning, TEXT("Inv Num %d"), Character->InventoryComponent->Inventory.Num());
			for (auto& LocalForItem : Character->InventoryComponent->Inventory)
			{
				LocalValue = Character->InventoryComponent->GetBeverageCapacityFree(LocalForItem.Key);

				UE_LOG(LogTemp, Warning, TEXT("%s Value - %f"), LocalForItem.Key.ID, LocalValue);
				if (LocalForItem.Key.ModifierBonuses.Contains("capacity"))
				{
					if (LocalValue < *LocalForItem.Key.ModifierBonuses.Find("capacity"))
					{
						LocalMenuInventory.Add(LocalForItem.Key, LocalForItem.Value);
					}
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("LocalMenuInventory Num %d"), LocalMenuInventory.Num());
			CreateInventoryWB(LocalMenuInventory);
		}
		
		LocalMenuStatus = 0;
		MenuStatus = 3;

		break;
	case 1:
		
		InventoryMenu->SetVisibility(ESlateVisibility::Visible);
		SkillMenu->SetVisibility(ESlateVisibility::Hidden);
		BUT_Skills->SetVisibility(ESlateVisibility::Hidden);
		BUT_Inventory->SetVisibility(ESlateVisibility::Hidden);
		BUT_Left->SetVisibility(ESlateVisibility::Visible);
		BUT_Right->SetVisibility(ESlateVisibility::Visible);

		UnSelectedItem();
		bIsChest = false;

		if (WindowID == "BeverageCapacityIA")
		{
			LocalMenuInventory.Empty();
			UE_LOG(LogTemp, Warning, TEXT("Inv Num %d"), Character->InventoryComponent->Inventory.Num());
			for (auto& LocalForItem : Character->InventoryComponent->Inventory)
			{
				LocalValue = Character->InventoryComponent->GetBeverageCapacityFree(LocalForItem.Key);

				UE_LOG(LogTemp, Warning, TEXT("%s Value - %f"), LocalForItem.Key.ID, LocalValue);
				if (LocalValue > 0.0f)
				{
					LocalMenuInventory.Add(LocalForItem.Key, LocalForItem.Value);
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("LocalMenuInventory Num %d"), LocalMenuInventory.Num());
			CreateInventoryWB(LocalMenuInventory);
		}

		LocalMenuStatus = 1;
		MenuStatus = 3;

		break;
	}
}