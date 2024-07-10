// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.h"
#include "InteractionComponent.generated.h"

class ATerraCharacter;
class AInteractableActor;

UENUM(BlueprintType)
enum EInteractID
{
	Standart,
	Chest,
	Item,
	Herb,
};

UENUM(BlueprintType)
enum EBool
{
	No,
	Yes,
};


UCLASS(ClassGroup = (Component), meta = (BlueprintSpawnableComponent))
class TERRA_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnInteractionBegan(ATerraCharacter* LocalCharacter);
	virtual void OnInteractionEnd();

	void TurningOutlineOn();
	void TurningOutlineOff();

	void OnSecondCalculations();

	/// Vars ///

	


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEnumAsByte<EInteractID> InteractID;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "InteractID == EInteractID::Chest", EditConditionHides))
	uint16 AmountOfSlots; 

	UPROPERTY(EditAnywhere, meta = (EditCondition = "InteractID == EInteractID::Item", EditConditionHides))
	FSItem Item;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEnumAsByte<EBool> GainXP;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "GainXP == EBool::Yes", EditConditionHides))
	FName AttributeID;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "GainXP == EBool::Yes", EditConditionHides))
	int AttributeXPAmount;


	AInteractableActor* ActorREF;
	FSItem LocalItem;
	bool bIsDurabilityDecay;
};
