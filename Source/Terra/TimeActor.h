// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/GameplayStatics.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerraCharacter.h"
#include "InteractableActor.h"
#include "TimeActor.generated.h"

UCLASS()
class TERRA_API ATimeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATimeActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame

	void OneInGameSecond();
	void AddCharacterToArray(ATerraCharacter* TerraCharacter);

	TArray<AInteractionMark*> GetAllMarksForStatus(FName Status);
	TMap<FName, int> GetNumberOfMarkThatRestoreStats(FName Status);

	UPROPERTY(EditAnywhere)
	int DayLength; 

	const float DayLengthInRealSeconds = 1440.0f;

	float OneInGameSecondValue;

	TArray<ATerraCharacter*> Characters;
	TArray<AInteractableActor*> ThrowedItems;
	TArray<AInteractableActor*> Chests;

	uint8 TimeHour;
	uint16 TimeMinute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (RowType = "DT Acitivites"));
	UDataTable* DT_Acitivites;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (RowType = "DT Statuses"));
	UDataTable* DT_Statuses;
};
