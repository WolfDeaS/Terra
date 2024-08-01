// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionMark.h"


AInteractionMark::AInteractionMark()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInteractionMark::BeginPlay()
{
	Super::BeginPlay();

	// Only for Herb
	
	SpawnActorClass = TakeOneFromActorArray();
	if (SpawnActorClass)
	{
		SpawnActor = GetWorld()->SpawnActor<AInteractableActor>(SpawnActorClass, this->GetTargetLocation(), this->GetActorRotation());
		SpawnActor->InteractionComponent->ReferencedActor = this;

		ThisMarkActors.Add(SpawnActor);
		/*
		SpawnActor->InteractionComponent->InteractID = EInteractID::Herb;


		// TODO TimeActor
		ATimeActor* LocalTimeActor;
		LocalTimeActor = this->TimeActor;

		LocalTimeActor->ThrowedItems.Add(ItemObjectOnThrow);
		*/
	}
    
}

TSubclassOf<class AInteractableActor> AInteractionMark::TakeOneFromActorArray()
{
	// We take Num of Array, and -1 cause this way we get all indexes of ActorsToSpawn array
	int LocalInt = ActorsToSpawn.Num();
	LocalInt--;

	return ActorsToSpawn[FMath::RandRange(0, LocalInt)];
}