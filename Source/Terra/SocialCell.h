// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "InteractionComponent.h"
#include "StatusComponent.h"
#include "InteractionMark.h"
#include "InteractableActor.h"
#include "SocialCell.generated.h"

UENUM(BlueprintType)
enum ESCType
{
	Forest,
	City,
};

USTRUCT(BlueprintType)
struct FSNameArrays : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FName> Array;
};

USTRUCT(BlueprintType)
struct FSActivitiesCreation : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FName> Tags;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<TEnumAsByte<ESCType>, FSNameArrays> PossibleActivities;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FName, float> SkillsPriority;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TEnumAsByte<EMarkType>> MarkForActivities;
};

UCLASS()
class TERRA_API ASocialCell : public AActor
{
	GENERATED_BODY()
	
public:	
	// SC same as SocialCell
	ASocialCell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void InitCell();
	void CalculateAndAddToRequestStatus(FName Status);
	void GiveTasks();
	ATerraCharacter* FindBetterCharacterForActivity(FName Activity, FName AdditionalInfo = "None");

	UPROPERTY(EditAnywhere)
	USceneComponent* LocalSceneComponent;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UBoxComponent* LocalCollisionBox;

	TEnumAsByte<ESCType> SCType;

	TMap<FName, TArray<AInteractableActor*>> InteractableActors;
	TMap<FName, TArray<AInteractionMark*>> MarksActor;
	
	TMap<FName, TArray<EInteractID>> ActorsTypeThatResotreStatuses;

	TMap<FName, float> RequestMap;
	TMap<FName, float> GiveRequestMap;

	TArray<ATerraCharacter*> Characters;
	TMap<FName, TArray<ATerraCharacter*>> CharactersActivities;
	TArray<ATerraCharacter*> CharactersOutOfActivities;

	UPROPERTY(EditAnywhere)
	UDataTable* DT_Acitivites;
};
