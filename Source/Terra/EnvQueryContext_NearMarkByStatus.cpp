// Fill out your copyright notice in the Description page of Project Settings.

#include "EnvQueryContext_NearMarkByStatus.h"
#include "TimeActor.h"


void UEnvQueryContext_NearMarkByStatus::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);

	
	ATerraCharacter* Character = Cast<ATerraCharacter>((QueryInstance.Owner).Get());
	FName Status = "Hunger";

	TArray<AActor*> TimeActorArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATimeActor::StaticClass(), TimeActorArray);
	ATimeActor* TimeActor = Cast<ATimeActor>(TimeActorArray[0]);

	FSStatusCreation* StatusDataFromDT;
	StatusDataFromDT = TimeActor->DT_Statuses->FindRow<FSStatusCreation>(Status, TEXT("none"), false);

	FSActivitiesCreation* ActvitiesDataFromDT;

	TArray<EMarkType> MarksTypeArray;

	for (auto& LocalActivity : StatusDataFromDT->ActivitiesToRestore)
	{
		ActvitiesDataFromDT = TimeActor->DT_Acitivites->FindRow<FSActivitiesCreation>(LocalActivity, TEXT("none"), false);

		MarksTypeArray.Append(ActvitiesDataFromDT->MarkForActivities);
	}

	TArray<AActor*> LocalActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInteractionMark::StaticClass(), LocalActors);

	TArray<AActor*> MatchingMark;
	for (auto& LocalActor : LocalActors)
	{
		if (MarksTypeArray.Contains(Cast<AInteractionMark>(LocalActor)->MarkType))
		{
			MatchingMark.Add(LocalActor);
		}
	}

	AActor* LocalActor = MatchingMark[0];
	
	if (MatchingMark.Num() > 1)
	{
		LocalActor = Character->FindClosetstActorFromArray(MatchingMark);
	}

	UEnvQueryItemType_Actor::SetContextHelper(ContextData, LocalActor);
}
	