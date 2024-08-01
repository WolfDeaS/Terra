// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindNearestActor.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TerraCharacter.h"
#include "ActorsArray.h"
#include "AITypes.h"

UBTTask_FindNearestActor::UBTTask_FindNearestActor()
{
	NodeName = "Find Nearest Actor Locataion";
}

EBTNodeResult::Type UBTTask_FindNearestActor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ATerraCharacter* LocalCharacter = Cast<ATerraCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(CharacterKey.SelectedKeyName));
	
	TArray<AActor*> LocalActorArray;
	LocalActorArray.Append(LocalCharacter->LocalArray);
	
	AActor* ClosetActor;
	ClosetActor = LocalCharacter->FindClosetstActorFromArray(LocalActorArray);
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(NearestActorOutputKey.SelectedKeyName, Cast<UObject>(ClosetActor));

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(NearestActorLocationOutputKey.SelectedKeyName, ClosetActor->GetTargetLocation());
	
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}