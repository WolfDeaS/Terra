// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableActor.h"
#include "InteractionMark.generated.h"


// Firstly, the brand will be memorized by bots. Which will then "visit" it again
// Secondly, certain objects will be spawned within a certain radius: for example, animals.

// But the type is technically needed to select the necessary conditions for this. 
// There will be a separate Hunting activity that will go through these points.
// And so that in this case, the bot with the highest Hunting level is selected.

UENUM(BlueprintType)
enum EMarkType
{
	Herbs,
	Animals,
};

UCLASS()
class TERRA_API AInteractionMark : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractionMark();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame

	TSubclassOf<class AInteractableActor> SpawnActorClass;
	AInteractableActor* SpawnActor;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EMarkType> MarkType;
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class AInteractableActor>> ActorsToSpawn; // Game will chose one of them, but all must appear on the map
};
