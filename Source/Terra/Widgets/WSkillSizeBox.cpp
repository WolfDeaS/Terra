// Fill out your copyright notice in the Description page of Project Settings.

#include "WSkillSizeBox.h"
#include "Terra/TerraCharacter.h"

void UWSkillSizeBox::NativeConstruct()
{
	Super::NativeConstruct();
}


void UWSkillSizeBox::Init(FSSkillsCreation* Skill, bool IsSkillActive, bool CanBeActivated, FName Attribute, FName SkillID)
{

	LocalSkill = Skill;
	LocalAttribute = Attribute;
	LocalSkillID = SkillID;
	Active = IsSkillActive;

	FString LocalString;
	LocalString.Append(Skill->Name.ToString());
	LocalString.Append("(");
	
	int LocalInt;
	LocalInt = Active;

	CanBeEnacted = CanBeActivated;

	LocalString.Append(FString::FromInt(LocalInt));
	LocalString.Append(")");

	Name->SetText(FText::FromString(LocalString));
	Icon->SetBrushFromTexture(Skill->Icon, false);
}

void UWSkillSizeBox::BUT_SkillEnact()
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