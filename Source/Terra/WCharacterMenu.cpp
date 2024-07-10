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

		if (!LocalInventory.IsEmpty())
		{
			TArray<FSItem> LocalItems;
			LocalInventory.GenerateKeyArray(LocalItems);


			for (auto& LocalItem : LocalItems)
			{
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
	switch (Status)
	{
	case 0: 
		InventoryMenu->SetVisibility(ESlateVisibility::Visible);
		SkillMenu->SetVisibility(ESlateVisibility::Hidden);
		CreateInventoryWB(Character->InventoryComponent->Inventory);
		UnSelectedItem();
		bIsChest = false;

		break;
	case 1:
		SkillMenu->SetVisibility(ESlateVisibility::Visible);
		InventoryMenu->SetVisibility(ESlateVisibility::Hidden);
		UnSelectedItem();

		break;
	case 2:
		SkillMenu->SetVisibility(ESlateVisibility::Hidden);
		BUT_Skills->SetVisibility(ESlateVisibility::Hidden);
		BUT_Chest->SetVisibility(ESlateVisibility::Visible);
		CreateObjectInventoryWB();
		UnSelectedItem();
		bIsChest = true;
		bIsChestOpen = true;

		break;
	default:
		break;
	}
}

void UWCharacterMenu::ChangeSelectedItem(UWInventorySizeBox* NewSelectedItemSB)
{
	if (SelectedInventorySizeBox)
	{
		SelectedInventorySizeBox->ChangeButtonStatus(0);
	}
	SelectedInventorySizeBox = NewSelectedItemSB;
	SelectedInventorySizeBox->ChangeButtonStatus(1);
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