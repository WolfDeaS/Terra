// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusComponent.generated.h"

USTRUCT(BlueprintType)
struct FSStatusCreation : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsNegative;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DebuffsEnd;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DebuffsStarts;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FName, float> DebuffsOnEnd;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FName, float> DebuffsOnStarts;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float BaseMax;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float BaseDecay;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Priority;

	// For Activities

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsRoamingNeededToRestore;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FName> ActivitiesToRestore;
};

class ATerraCharacter;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TERRA_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatusComponent();

	void Init();
	void OnSecondCalculations();
	void UpdateStatuses();
	void RemoveStatusModifiers(FName LocalStatus, float Value);
	void AddStatusModifiers(FName LocalStatus, float Value);

	FName GetMostPriorityDebuffStatus();
	TArray<FName> GetDebuffStatuses();

	TArray<FName> Statuses;
	TArray<FName> NegativeStatuses;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (RowType = "DT Statuses"));
	UDataTable* DT_Statuses;

	ATerraCharacter* Character;
};
