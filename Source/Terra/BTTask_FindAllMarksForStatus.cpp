// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindAllMarksForStatus.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TerraCharacter.h"
#include "InteractionMark.h"
#include "SocialCell.h"
#include "AITypes.h"

UBTTask_FindAllMarksForStatus::UBTTask_FindAllMarksForStatus()
{
	NodeName = "Find All Marks For Status";
}

EBTNodeResult::Type UBTTask_FindAllMarksForStatus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ATerraCharacter* LocalCharacter = Cast<ATerraCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(CharacterKey.SelectedKeyName));
	
	FName Status = OwnerComp.GetBlackboardComponent()->GetValueAsName(StatusKey.SelectedKeyName);

	FSStatusCreation* StatusDataFromDT;
	StatusDataFromDT = LocalCharacter->StatusComponent->DT_Statuses->FindRow<FSStatusCreation>(Status, TEXT("none"), false);

	FSActivitiesCreation* ActvitiesDataFromDT;

	TArray<EMarkType> MarksTypeArray;

	for (auto& LocalActivity : StatusDataFromDT->ActivitiesToRestore)
	{
		ActvitiesDataFromDT = LocalCharacter->SocialCell->DT_Acitivites->FindRow<FSActivitiesCreation>(LocalActivity, TEXT("none"), false);

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

	LocalCharacter->LocalArray = MatchingMark;

	return EBTNodeResult::Succeeded;
}