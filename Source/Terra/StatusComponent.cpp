// Fill out your copyright notice in the Description page of Project Settings.


#include "StatusComponent.h"
#include "TerraCharacter.h"
#include "TerraPlayerController.h"

// Sets default values for this component's properties
UStatusComponent::UStatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UStatusComponent::Init()
{
	FString LocalString;

	Statuses = DT_Statuses->GetRowNames();

	for (auto& LocalStatus : Statuses)
	{
		LocalString = LocalStatus.ToString();
		LocalString.Append("MaxBonusValue");
		Character->Modifiers.Add(*LocalString, 0.0f);

		LocalString = LocalStatus.ToString();
		LocalString.Append("MaxBonusModifier");
		Character->Modifiers.Add(*LocalString, 1.0f);
		

		LocalString = LocalStatus.ToString();
		LocalString.Append("DecayBonusValue");
		Character->Modifiers.Add(*LocalString, 0.0f);
		
		LocalString = LocalStatus.ToString();
		LocalString.Append("DecayBonusModifier");
		Character->Modifiers.Add(*LocalString, 1.0f);

		FSStatusCreation* StatusDataFromDT;
		StatusDataFromDT = DT_Statuses->FindRow<FSStatusCreation>(LocalStatus, TEXT("none"), false);

		if (StatusDataFromDT->bIsNegative)
		{
			NegativeStatuses.Add(LocalStatus);
		}
	}

	UpdateStatuses();

	for (auto& LocalStatus : Statuses)
	{
		if (!NegativeStatuses.Contains(LocalStatus))
		{
			LocalString = LocalStatus.ToString();
			LocalString.Append("DebuffsStarts");
			if (Character->Modifiers.Contains(*LocalString))
			{
				Character->Modifiers.Add(LocalStatus, *Character->Modifiers.Find(*LocalString));
			}
			else
			{
				LocalString = LocalStatus.ToString();
				LocalString.Append("Max");

				Character->Modifiers.Add(LocalStatus, *Character->Modifiers.Find(*LocalString));
			}
		}
		else
		{
			Character->Modifiers.Add(LocalStatus, 0.0f);
		}
	}
}

void UStatusComponent::OnSecondCalculations()
{
	float PREVLocalValue;
	float LocalValue;
	FString LocalString;
	for (auto& LocalStatus : Statuses)
	{
		LocalValue = *Character->Modifiers.Find(LocalStatus);
		PREVLocalValue = LocalValue;

		LocalString = LocalStatus.ToString();
		LocalString.Append("Decay");
		LocalValue -= *Character->Modifiers.Find(*LocalString);

		LocalString = LocalStatus.ToString();
		LocalString.Append("Max");
		LocalValue = FMath::Clamp(LocalValue, 0.0f, *Character->Modifiers.Find(*LocalString));

		if (LocalValue != PREVLocalValue)
		{
			RemoveStatusModifiers(LocalStatus, PREVLocalValue);
			AddStatusModifiers(LocalStatus, LocalValue);
		}

		Character->Modifiers.Add(LocalStatus, LocalValue);
	}
	if (Character->GetController()->IsPlayerController())
	{
		Cast<ATerraPlayerController>(Character->GetController())->HUD->UpdateStatusPB();
	}
}

void UStatusComponent::UpdateStatuses()
{
	float LocalValue;
	FString LocalString;
	for (auto& LocalStatus : Statuses)
	{
		FSStatusCreation* StatusDataFromDT;
		StatusDataFromDT = DT_Statuses->FindRow<FSStatusCreation>(LocalStatus, TEXT("none"), false);
		
		LocalValue = StatusDataFromDT->BaseMax;

		LocalString = LocalStatus.ToString();
		LocalString.Append("MaxBonusValue");
		LocalValue += *Character->Modifiers.Find(*LocalString);

		LocalString = LocalStatus.ToString();
		LocalString.Append("MaxBonusModifier");
		LocalValue *= *Character->Modifiers.Find(*LocalString);

		LocalString = LocalStatus.ToString();
		LocalString.Append("Max");
		Character->Modifiers.Add(*LocalString, LocalValue);
		
		if (StatusDataFromDT->DebuffsStarts != -1)
		{
			LocalString = LocalStatus.ToString();
			LocalString.Append("Max");

			LocalValue = *Character->Modifiers.Find(*LocalString) * (StatusDataFromDT->DebuffsStarts / StatusDataFromDT->BaseMax);

			LocalString = LocalStatus.ToString();
			LocalString.Append("DebuffsStarts");
			Character->Modifiers.Add(*LocalString, LocalValue);
		}

		if (StatusDataFromDT->DebuffsEnd != -1)
		{
			LocalString = LocalStatus.ToString();
			LocalString.Append("Max");

			LocalValue = *Character->Modifiers.Find(*LocalString) * (StatusDataFromDT->DebuffsEnd / StatusDataFromDT->BaseMax);

			LocalString = LocalStatus.ToString();
			LocalString.Append("DebuffsEnd");
			Character->Modifiers.Add(*LocalString, LocalValue);
		}

		LocalValue = StatusDataFromDT->BaseDecay;

		LocalString = LocalStatus.ToString();
		LocalString.Append("DecayBonusValue");
		LocalValue += *Character->Modifiers.Find(*LocalString);

		LocalString = LocalStatus.ToString();
		LocalString.Append("DecayBonusModifier");
		LocalValue *= *Character->Modifiers.Find(*LocalString);

		LocalString = LocalStatus.ToString();
		LocalString.Append("Decay");
		Character->Modifiers.Add(*LocalString, LocalValue);
	}
}

FName UStatusComponent::GetMostPriorityDebuffStatus()
{
	TArray<FName> StatusesWithDebuffs = GetDebuffStatuses();

	int32 LocalPriority = -1;
	FName ReturnStatus;

	for (auto& LocalStatus : StatusesWithDebuffs)
	{
		FSStatusCreation* StatusDataFromDT;
		StatusDataFromDT = DT_Statuses->FindRow<FSStatusCreation>(LocalStatus, TEXT("none"), false);

		if (StatusDataFromDT->Priority > LocalPriority)
		{
			LocalPriority = StatusDataFromDT->Priority;
			ReturnStatus = LocalStatus;
		}
	}

	return ReturnStatus;
}

TArray<FName> UStatusComponent::GetDebuffStatuses()
{
	TArray<FName> StatusesWithDebuffs;

	for (auto& LocalStatus : Statuses)
	{
		FSStatusCreation* StatusDataFromDT;
		StatusDataFromDT = DT_Statuses->FindRow<FSStatusCreation>(LocalStatus, TEXT("none"), false);

		if (*Character->Modifiers.Find(LocalStatus) < StatusDataFromDT->DebuffsEnd)
		{
			StatusesWithDebuffs.Add(LocalStatus);
		}
	}

	return StatusesWithDebuffs;
}

void UStatusComponent::RemoveStatusModifiers(FName LocalStatus, float Value)
{
	FSStatusCreation* StatusDataFromDT;
	StatusDataFromDT = DT_Statuses->FindRow<FSStatusCreation>(LocalStatus, TEXT("none"), false);

	FString LocalString = LocalStatus.ToString();
	LocalString.Append("DebuffsEnd");

	if (Character->Modifiers.Contains(*LocalString))
	{
		if (Value <= *Character->Modifiers.Find(*LocalString))
		{
			for (auto& Pair : StatusDataFromDT->DebuffsOnEnd)
			{
				LocalString = LocalStatus.ToString();
				LocalString.Append("DebuffsEnd");

				float LocalMax = *Character->Modifiers.Find(*LocalString);

				float Modifier;
				Modifier = 1.0f - (Value / LocalMax);
				float LocalValue = *Character->Modifiers.Find(Pair.Key) - (Pair.Value * Modifier);

				Character->Modifiers.Add(Pair.Key, LocalValue);
			}
		}
	}
	

	LocalString = LocalStatus.ToString();
	LocalString.Append("DebuffsStarts");

	FString AnotherLocalString = LocalStatus.ToString();
	AnotherLocalString.Append("Max");

	if (Character->Modifiers.Contains(*LocalString))
	{
		if (Value >= *Character->Modifiers.Find(*LocalString))
		{
			for (auto& Pair : StatusDataFromDT->DebuffsOnStarts)
			{
				float LocalValue = Value;
				LocalValue -= *Character->Modifiers.Find(*LocalString);

				LocalString = LocalStatus.ToString();
				LocalString.Append("Max");

				float LocalMax = *Character->Modifiers.Find(*LocalString);
				LocalMax -= StatusDataFromDT->DebuffsStarts;

				float Modifier;
				Modifier = LocalValue / LocalMax;

				LocalValue = *Character->Modifiers.Find(Pair.Key) - (Pair.Value * Modifier);


				Character->Modifiers.Add(Pair.Key, LocalValue);
			}
		}
	}

	if (Character->GetController()->IsPlayerController())
	{
		Cast<ATerraPlayerController>(Character->GetController())->HUD->UpdateStatusPB();
	}
}

void UStatusComponent::AddStatusModifiers(FName LocalStatus, float Value)
{
	FSStatusCreation* StatusDataFromDT;
	StatusDataFromDT = DT_Statuses->FindRow<FSStatusCreation>(LocalStatus, TEXT("none"), false);

	FString LocalString = LocalStatus.ToString();
	LocalString.Append("DebuffsEnd");

	if (Character->Modifiers.Contains(*LocalString))
	{
		if (Value <= *Character->Modifiers.Find(*LocalString))
		{
			for (auto& Pair : StatusDataFromDT->DebuffsOnEnd)
			{
				LocalString = LocalStatus.ToString();
				LocalString.Append("DebuffsEnd");

				float LocalMax = *Character->Modifiers.Find(*LocalString);

				float Modifier;
				Modifier = 1.0f - (Value / LocalMax);
				float LocalValue = *Character->Modifiers.Find(Pair.Key) + (Pair.Value * Modifier);

				Character->Modifiers.Add(Pair.Key, LocalValue);
			}
		}
	}


	LocalString = LocalStatus.ToString();
	LocalString.Append("DebuffsStarts");

	FString AnotherLocalString = LocalStatus.ToString();
	AnotherLocalString.Append("Max");

	if (Character->Modifiers.Contains(*LocalString))
	{
		if (Value >= *Character->Modifiers.Find(*LocalString))
		{
			for (auto& Pair : StatusDataFromDT->DebuffsOnStarts)
			{
				float LocalValue = Value;
				LocalValue -= *Character->Modifiers.Find(*LocalString);

				LocalString = LocalStatus.ToString();
				LocalString.Append("Max");

				float LocalMax = *Character->Modifiers.Find(*LocalString);
				LocalMax -= StatusDataFromDT->DebuffsStarts;

				float Modifier;
				Modifier = LocalValue / LocalMax;

				LocalValue = *Character->Modifiers.Find(Pair.Key) + (Pair.Value * Modifier);


				Character->Modifiers.Add(Pair.Key, LocalValue);
			}
		}
	}

	if (Character->GetController()->IsPlayerController())
	{
		Cast<ATerraPlayerController>(Character->GetController())->HUD->UpdateStatusPB();
	}
}