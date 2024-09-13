#pragma once

#include "BT_IsHaveStatusMark.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Terra/TerraCharacter.h"
#include "Terra/SocialCell.h"
#include "Terra/TimeActor.h"
#include "AIController.h"

UBT_IsHaveStatusMark::UBT_IsHaveStatusMark()
{
	NodeName = "Is Character Has Needed Mark with Modifier (ONLY FOR THE HERB) (ONLY FOR THE STATUSES)";
}

bool UBT_IsHaveStatusMark::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bSuccess = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	if (!bSuccess) return false;

	ATerraCharacter* LocalCharacter = Cast<ATerraCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(CharacterKey.SelectedKeyName));

	FName Modifier = OwnerComp.GetBlackboardComponent()->GetValueAsName(ModifierKey.SelectedKeyName);

	TArray<FName> MarksIDArray;
	LocalCharacter->MarksActor.GenerateKeyArray(MarksIDArray);

	TArray<FString> ParseValue;
	TArray<FName> LocalItemArray;
	TArray<FName> LocalModifierArray;
	TArray<FName> UsedItemArray;

	TArray<AActor*> LocalActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATimeActor::StaticClass(), LocalActors);
	ATimeActor* LocalTimeActor = Cast<ATimeActor>(LocalActors[0]);

	bool bHaveAllMarkInfo = false;

	if(!LocalCharacter->MarksActor.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Is Have Status Mark: MarksActor Num - %d"), LocalCharacter->MarksActor.Num());
		for (auto& LocalPair : LocalTimeActor->GetNumberOfMarkThatRestoreStats(Modifier))
		{
			UE_LOG(LogTemp, Warning, TEXT("Is Have Status Mark: Name - %s, Value - %f"), LocalPair.Key, LocalPair.Value);
			if (LocalCharacter->MarksActor.Contains(LocalPair.Key))
			{
				if (LocalCharacter->MarksActor.Find(LocalPair.Key)->Num() != LocalPair.Value)
				{
					bHaveAllMarkInfo = true;
					break;
				}
			}
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Is Have Status Mark:Go Forward"));

	if (bHaveAllMarkInfo)
	{
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("Is Have Status Mark: Check This"));

	if (!MarksIDArray.IsEmpty())
	{
		for (auto& LocalName : MarksIDArray)
		{
			for (auto& LocalMark : *LocalCharacter->MarksActor.Find(LocalName))
			{
				if (LocalCharacter->SocialCell->GetEfficencyMarkValue(LocalMark, 1, Modifier))
				{
					return true;
				}
			}
		}
	}
	

	/*
	for (auto& LocalMark : MarksArray)
	{
		LocalMark.ToString().ParseIntoArray(ParseValue, TEXT(";"), true);
		
		FSHerbCreation* HerbDataFromDT;
		HerbDataFromDT = LocalCharacter->DT_Herbs->FindRow<FSHerbCreation>(*ParseValue[1], TEXT("none"), false);
		
		HerbDataFromDT->ItemsGiven.GenerateKeyArray(LocalItemArray);

		for (auto& LocalItem : UsedItemArray)
		{
			LocalItemArray.Remove(LocalItem);
		}

		if (LocalCharacter->SocialCell->GetEfficencyMarkValue(LocalMark,1,Modifier))
		{
			return true;
		}
		
		for (auto& LocalItem : LocalItemArray)
		{
			FSItemCreation* ItemDataFromDT;
			ItemDataFromDT = LocalCharacter->DT_Items->FindRow<FSItemCreation>(LocalItem, TEXT("none"), false);

			UsedItemArray.Add(LocalItem);


			for (auto& Pair : ItemDataFromDT->ModifierBonuses)
			{
				if (Pair.Value > 0)
				{
					if (Modifier == Pair.Key)
					{
						
					}
				}
			}
		}
	}*/

	return false;
} 




