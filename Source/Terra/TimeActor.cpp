// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeActor.h"
#include "SocialCell.h"

// Sets default values
ATimeActor::ATimeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATimeActor::BeginPlay()
{
	Super::BeginPlay();

	OneInGameSecondValue = float(DayLength / DayLengthInRealSeconds);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([=, this]() { OneInGameSecond(); }), OneInGameSecondValue, true);

	
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInteractableActor::StaticClass(), FoundActors);

	for (auto& LocalChest : FoundActors)
	{
		if (Cast<AInteractableActor>(LocalChest)->InteractionComponent->InteractID == EInteractID::Chest)
		{
			Chests.Add(Cast<AInteractableActor>(LocalChest));
		}
	}
}

void ATimeActor::OneInGameSecond()
{

	if (TimeMinute + 1 > 1440)
	{
		TimeMinute = 1;
	}
	else
	{
		TimeMinute++;
	}
	float LocalHour = TimeMinute / 60;
	LocalHour = FMath::Floor(LocalHour);

	TimeHour = LocalHour;
	uint8 LocalMinute = TimeMinute - (TimeHour * 60);

	FString LocalString;
	LocalString.Append(FString::FromInt(TimeHour));
	LocalString.Append(":");
	LocalString.Append(FString::FromInt(LocalMinute));


	if (!Characters.IsEmpty())
	{
		for (auto& LocalCharacter : Characters)
		{
			LocalCharacter->InventoryComponent->OnSecondCalculations();
			LocalCharacter->StatusComponent->OnSecondCalculations();
		}
	}
	for (auto& LocalThrowedItem : ThrowedItems)
	{
		LocalThrowedItem->InteractionComponent->OnSecondCalculations();
	}
	for (auto& LocalChest : Chests)
	{
		LocalChest->InventoryComponent->OnSecondCalculations();
	}
}

void ATimeActor::AddCharacterToArray(ATerraCharacter* TerraCharacter)
{
	Characters.Empty();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATerraCharacter::StaticClass(), FoundActors);

	for (auto& LocalCharacter : FoundActors)
	{
		Characters.Add(Cast<ATerraCharacter>(LocalCharacter));
	}
}


TArray<AInteractionMark*> ATimeActor::GetAllMarksForStatus(FName Status)
{

	FSStatusCreation* StatusDataFromDT;
	StatusDataFromDT = Characters[0]->StatusComponent->DT_Statuses->FindRow<FSStatusCreation>(Status, TEXT("none"), false);

	FSActivitiesCreation* ActvitiesDataFromDT;

	TArray<EMarkType> MarksTypeArray;

	for (auto& LocalActivity : StatusDataFromDT->ActivitiesToRestore)
	{
		TArray<AActor*> SCArray;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASocialCell::StaticClass(), SCArray);

		ASocialCell* LocalSC = Cast<ASocialCell>(SCArray[0]);

		ActvitiesDataFromDT = LocalSC->DT_Acitivites->FindRow<FSActivitiesCreation>(LocalActivity, TEXT("none"), false);

		MarksTypeArray.Append(ActvitiesDataFromDT->MarkForActivities);
	}

	TArray<AActor*> LocalActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInteractionMark::StaticClass(), LocalActors);

	TArray<AInteractionMark*> MatchingMark;
	for (auto& LocalActor : LocalActors)
	{
		if (MarksTypeArray.Contains(Cast<AInteractionMark>(LocalActor)->MarkType))
		{
			MatchingMark.Add(Cast<AInteractionMark>(LocalActor));
		}
	}

	return MatchingMark;
}

TMap<FName, int> ATimeActor::GetNumberOfMarkThatRestoreStats(FName Status)
{
	TMap<FName, int> OutputMap;

	FSStatusCreation* StatusDataFromDT;
	StatusDataFromDT = Characters[0]->StatusComponent->DT_Statuses->FindRow<FSStatusCreation>(Status, TEXT("none"), false);

	FSActivitiesCreation* ActvitiesDataFromDT;

	TArray<EMarkType> MarksTypeArray;

	for (auto& LocalActivity : StatusDataFromDT->ActivitiesToRestore)
	{
		TArray<AActor*> SCArray;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASocialCell::StaticClass(), SCArray);

		ASocialCell* LocalSC = Cast<ASocialCell>(SCArray[0]);

		ActvitiesDataFromDT = LocalSC->DT_Acitivites->FindRow<FSActivitiesCreation>(LocalActivity, TEXT("none"), false);

		MarksTypeArray.Append(ActvitiesDataFromDT->MarkForActivities);
	}

	TArray<AActor*> LocalActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInteractionMark::StaticClass(), LocalActors);

	FString LocalString;
	TArray<AInteractionMark*> MatchingMark;

	for (auto& LocalActor : LocalActors)
	{
		if (MarksTypeArray.Contains(Cast<AInteractionMark>(LocalActor)->MarkType))
		{
			LocalString.Append(UEnum::GetValueAsString(Cast<AInteractionMark>(LocalActor)->MarkType));
			LocalString.Append(Cast<AInteractionMark>(LocalActor)->ThisMarkActors[0]->InteractionComponent->AdditionalInfo.ToString());
			
			if (OutputMap.Contains(*LocalString))
			{
				OutputMap.Add(*LocalString, *OutputMap.Find(*LocalString) + 1);
			}
			else
			{
				OutputMap.Add(*LocalString, 1);
			}
		}
	}

	return OutputMap;
}