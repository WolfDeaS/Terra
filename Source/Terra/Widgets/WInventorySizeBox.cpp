// Fill out your copyright notice in the Description page of Project Settings.

#include "WInventorySizeBox.h"
#include "WCharacterMenu.h"
#include "Terra/TerraCharacter.h"

void UWInventorySizeBox::NativeConstruct()
{
	Super::NativeConstruct();

	UnSelectButton = FLinearColor(0.1f, 0.1f, 0.1f, 1.0f);
	SelectButton = FLinearColor(1.0f, 0.8f, 0.0f, 1.0f);
	ActiveButton = FLinearColor(0.0f, 0.0f, 1.0f, 1.0f);
}


void UWInventorySizeBox::Init(FSItem* Item, uint32 LocalQuantity, UDataTable* DT_Items)
{
	FSItemCreation* ItemDataFromDT;
	ItemDataFromDT = DT_Items->FindRow<FSItemCreation>(Item->ID, TEXT("none"), false);

	LocalItem = *Item;

	FString LocalString;
	LocalString = ItemDataFromDT->Name.ToString();
	LocalString.Append("(");
	LocalString.Append(FString::FromInt(Item->Active));
	LocalString.Append(")");
	Name->SetText(FText::FromString(LocalString));

	
	Icon->SetBrushFromTexture(ItemDataFromDT->Icon, false);
	LocalString = 0;
	LocalString = FString::FromInt(LocalQuantity);
	Quantity->SetText(FText::FromString(LocalString));

	float ItemDurability;
	ItemDurability = Item->Durability;
	float MaxDurability;
	MaxDurability = ItemDataFromDT->MaxDurability;

	float Percent;
	Percent = ItemDurability / MaxDurability;
	Percent *= 100.0f;
	LocalString = 0;
	LocalString.Append(FString::SanitizeFloat(Percent));

	Durability->SetText(FText::FromString(LocalString));
}

void UWInventorySizeBox::BUT_Use()
{
	Parent->SelectedItemStruct = LocalItem;
	Parent->ChangeSelectedItem(this);
}

void UWInventorySizeBox::ChangeButtonStatus(uint8 Status)
{
	switch (Status)
	{
	case 0:
		Button->SetBackgroundColor(UnSelectButton);
		break;
	case 1:
		Button->SetBackgroundColor(SelectButton);
		break;
	default:
		break;
	}
}

/*
void UWInventorySizeBox::BUT_SkillEnact()
{
	ATerraCharacter* Character;
	Character = Cast<ATerraCharacter>(GetOwningPlayer()->GetPawn());

	if (CanBeEnacted && !Active && Character->ProgressionComponent->FindAttribute(LocalAttribute)->Point >= 1)
	{
		

		TArray<FName> LocalKeys;
		LocalSkill->ModifierBonuses.GenerateKeyArray(LocalKeys);

		for (auto& LocalSkillKey : LocalKeys)
		{
			Character->Modifiers.Add(LocalSkillKey) = float(*Character->Modifiers.Find(LocalSkillKey) + *LocalSkill->ModifierBonuses.Find(LocalSkillKey));
		}

		Character->ProgressionComponent->FindAttribute(LocalAttribute)->ActivedSkills.Add(LocalSkillID);
		Character->ProgressionComponent->FindAttribute(LocalAttribute)->Point--;
	}
}
*/