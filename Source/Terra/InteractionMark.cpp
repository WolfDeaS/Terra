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
	
	SpawnActorClass = ActorsToSpawn[0];
	if (SpawnActorClass)
	{
		SpawnActor = GetWorld()->SpawnActor<AInteractableActor>(SpawnActorClass, this->GetTargetLocation(), this->GetActorRotation());
		/*
		SpawnActor->InteractionComponent->InteractID = EInteractID::Herb;


		// TODO TimeActor
		ATimeActor* LocalTimeActor;
		LocalTimeActor = this->TimeActor;

		LocalTimeActor->ThrowedItems.Add(ItemObjectOnThrow);
		*/
	}
    
}

