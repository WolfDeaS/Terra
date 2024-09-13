// Fill out your copyright notice in the Description page of Project Settings.


#include "WAttributeSizeBox.h"
#include "WCharacterMenu.h"

void UWAttributeSizeBox::NativeConstruct()
{
	Super::NativeConstruct();

	StandatHeightOverride = SizeBox->HeightOverride;
}


void UWAttributeSizeBox::Init(FSAttribute* LocalAttribute, UDataTable* DT_Attributes, UDataTable* DT_Skills, TArray<int> NewLevelXPRequired)
{
	Local_DT_Attributes = DT_Attributes;
	Local_DT_Skills = DT_Skills;
	Attribute = LocalAttribute;

	FSAttributeCreation* AttributeDataFromtDT;
	AttributeDataFromtDT = DT_Attributes->FindRow<FSAttributeCreation>(LocalAttribute->ID, TEXT("none"), false);

	LocalAttributeData = AttributeDataFromtDT;

	FString LocalString;
	LocalString.Append(AttributeDataFromtDT->Name.ToString());
	LocalString.Append("(");
	LocalString.Append(FString::FromInt(LocalAttribute->Point));
	LocalString.Append(")");
	Name->SetText(FText::FromString(LocalString));
	Icon->SetBrushFromTexture(AttributeDataFromtDT->Icon);
	
	Level->SetText(FText::FromString(FString::FromInt(LocalAttribute->Level)));

	AttributeLevel = LocalAttribute->Level;

	float LocalPBPercent;
	float LocalXPRequired;
	float LocalAttributeXP;
	LocalAttributeXP = LocalAttribute->XP;
	if (NewLevelXPRequired.Num() > LocalAttribute->Level + 1)
	{
		LocalXPRequired = NewLevelXPRequired[LocalAttribute->Level + 1];
	}
	else
	{
		LocalXPRequired = NewLevelXPRequired[LocalAttribute->Level];
	}
	LocalPBPercent = LocalAttributeXP / LocalXPRequired;
	PB->SetPercent(LocalPBPercent);

	LocalString = FString::FromInt(LocalAttribute->XP);
	LocalString.Append(" / ");
	LocalString.Append(FString::FromInt(LocalXPRequired));
	XP->SetText(FText::FromString(LocalString));

	UpdateSkillsWB();
}

void UWAttributeSizeBox::UpdateSkillsWB()
{
	if (SkillsWBOpen)
	{
		WB_Skills->ClearChildren();

		TArray<FName> LocalKeys;
		LocalAttributeData->Skills.GenerateKeyArray(LocalKeys);
		for (auto& LocalSkill : LocalKeys)
		{
			FSSkillsCreation* AttributeDataFromtDT;
			AttributeDataFromtDT = Local_DT_Skills->FindRow<FSSkillsCreation>(LocalSkill, TEXT("none"), false);

			SkillSizeBox = CreateWidget<UWSkillSizeBox>(this, SkillSizeBoxClass);
			
			bool CanBeActivated;
			CanBeActivated = 0;
			if (AttributeLevel >= *LocalAttributeData->Skills.Find(LocalSkill))
			{
				CanBeActivated = 1;
			}

			bool IsActive;
			IsActive = 0;
			if (Attribute->ActivedSkills.Find(LocalSkill) != -1)
			{
				IsActive = 1;
			}
			SkillSizeBox->Init(AttributeDataFromtDT, IsActive, CanBeActivated, Attribute->ID, LocalSkill);
			WB_Skills->AddChildToWrapBox(SkillSizeBox);
		}
		if (WB_Skills->HasAnyChildren())
		{
			float ChildrenSBSize;
			ChildrenSBSize = Cast<UWSkillSizeBox>(WB_Skills->GetAllChildren()[0])->SizeBox->HeightOverride;
			ChildrenSBSize = ChildrenSBSize * WB_Skills->GetAllChildren().Num();
			SizeBox->SetHeightOverride(StandatHeightOverride + ChildrenSBSize);
		}
	}
}

void UWAttributeSizeBox::BUT_Use()
{
	if (SkillsWBOpen)
	{
		SkillsWBOpen = 0;
		WB_Skills->ClearChildren();
		SizeBox->SetHeightOverride(StandatHeightOverride);
	}
	else
	{
		SkillsWBOpen = 1;
		UpdateSkillsWB();
	}
	Parent->UpdateAttributeWB();
}