// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WaterBodyRiverActor.h"
#include "InteractionComponent.h"
#include "TerraCharacter.h"
#include "WaterSplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "InteractableRiver.generated.h"

/**
 * 
 */
UCLASS()
class TERRA_API AInteractableRiver : public AWaterBodyRiver
{
	GENERATED_BODY()

public: 
	AInteractableRiver();
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditAnywhere)
	class UInteractionComponent* InteractionComponent;

	UPROPERTY(EditAnywhere)
	class USplineMeshComponent* GlobalSplineMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (RowType = "DT Beverage"));
	UDataTable* DT_Beverage;
};
