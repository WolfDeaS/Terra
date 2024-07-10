// Fill out your copyright notice in the Description page of Project Settings.

#include "ProgressionComponent.h"
#include "TerraCharacter.h"

void UProgressionComponent::Init()
{
	// Ініціалізація всіх атрибутів

	TArray<FName> AllAttributesID;
	AllAttributesID = DT_Attributes->GetRowNames();

	for (auto& RowID : AllAttributesID)
	{
		FSAttribute LocalAttribute;
		LocalAttribute.ID = RowID;
		LocalAttribute.Level = 0;
		LocalAttribute.XP = 50;
		LocalAttribute.Point = 0;

		LocalAttribute.ActivedSkills.Reserve(DT_Attributes->FindRow<FSAttributeCreation>(RowID, NULL, true)->Skills.Num());

		Attributes.Add(LocalAttribute);
	}
}

bool UProgressionComponent::IsSkillActive(FName SkillID)
{
	for (auto& LocalAttribute : Attributes)
	{
		for (auto& LocalSkill : LocalAttribute.ActivedSkills)
		{
			if (LocalSkill == SkillID)
			{
				return true;
			}
		}
	}
	return false;
}

// Додає досвід до атрибута
// Систему збільшення рівня додавати саме сюди
void UProgressionComponent::AddXPToAttribute(FName Attribute, int XP)
{
	
	FSAttribute* LocalAttributeREF;
	LocalAttributeREF = FindAttribute(Attribute);
	NewLevelXPRequired.Num();
	
	if (NewLevelXPRequired.Num() > LocalAttributeREF->Level + 1)
	{
		if (LocalAttributeREF->XP + XP < NewLevelXPRequired[LocalAttributeREF->Level + 1])
		{
			LocalAttributeREF->XP = LocalAttributeREF->XP + XP;
		}
		else
		{
			while (LocalAttributeREF->XP + XP > NewLevelXPRequired[LocalAttributeREF->Level + 1])
			{

				
				XP = XP - (NewLevelXPRequired[LocalAttributeREF->Level + 1] - LocalAttributeREF->XP);
				LocalAttributeREF->XP = 0;
				LevelUp(LocalAttributeREF);
				if (NewLevelXPRequired.Num() - 1 <= LocalAttributeREF->Level)
				{
					return;
				}
				
			}
			LocalAttributeREF->XP = XP;
		}
	}
	
}

void UProgressionComponent::LevelUp(FSAttribute* Attribute)
{

	Attribute->Level++;

	FSAttributeCreation* LocalDTAttribute;
	LocalDTAttribute = DT_Attributes->FindRow<FSAttributeCreation>(Attribute->ID, NULL, true);

	TArray<FName> ModifierKeys;
	LocalDTAttribute->ModifierBonuses.GetKeys(ModifierKeys);

	for (auto& LocalKey : ModifierKeys)
	{
		Character->Modifiers.Add(LocalKey) = float(*Character->Modifiers.Find(LocalKey) + *LocalDTAttribute->ModifierBonuses.Find(LocalKey));
	}

	Character->InventoryComponent->RecalculateInventoryModifierBonuses();

	for (auto& LocalValue : LocalDTAttribute->OnWhichLevelsAddPoint)
	{
		if (LocalValue == Attribute->Level)
		{
			Attribute->Point++;
			return;
		}
	}
}

bool UProgressionComponent::IsAttributesOnLevel(TMap<FName, uint8> ConditionsAttributes)
{
	for (auto& Pair : ConditionsAttributes)
	{
		if (FindAttribute(Pair.Key)->Level < Pair.Value)
		{
			return false;
		}
	}
	return true;
}

float UProgressionComponent::GetAttributeOnLevelModifier(TMap<FName, uint8> ConditionsAttributes)
{
	float ModifierReturn;
	ModifierReturn = 1.0f;
	for (auto& Pair : ConditionsAttributes)
	{
		float LocalModifier;
		LocalModifier = (1.0f / *ConditionsAttributes.Find(Pair.Key)) * FindAttribute(Pair.Key)->Level;
		
		if (LocalModifier < ModifierReturn)
		{
			ModifierReturn = LocalModifier;
		}
	}
	return ModifierReturn;
}

// Пошуки
FSAttribute* UProgressionComponent::FindAttribute(FName Attribute)
{
	for (auto& LocalAttribute : Attributes)
	{
		if (LocalAttribute.ID == Attribute)
		{
			return &LocalAttribute;
		}
	}
	return 0;
}

FSAttributeCreation* UProgressionComponent::FindDTAttributeRow(FName Attribute)
{
	TArray<FName> LocalRows;
	LocalRows = DT_Attributes->GetRowNames();
	

	return 0;
}

