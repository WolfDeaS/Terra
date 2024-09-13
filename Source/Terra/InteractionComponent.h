// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.h"
#include "InteractionComponent.generated.h"


class ATerraCharacter;
class AInteractableActor;
class AInteractableRiver;

UENUM(BlueprintType)
enum EInteractID
{
	Standart,
	Chest,
	Item,
	Herb,
	BevearageSource,
	BeverageContainer,
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
	void HerbTimer(ATerraCharacter* LocalCharacter);
	void AfterInteractionEffect(ATerraCharacter* LocalCharacter);
	void ItemsMustRot(ATerraCharacter* LocalCharacter);

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

	UPROPERTY(EditAnywhere, meta = (EditCondition = "InteractID == EInteractID::BeverageContainer", EditConditionHides))
	float Capacity;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "InteractID == EInteractID::BeverageContainer", EditConditionHides))
	float TimeToFill;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "InteractID == EInteractID::BeverageContainer", EditConditionHides))
	float MinTimeToFill;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "InteractID == EInteractID::BeverageContainer", EditConditionHides))
	TMap<FName,float> BasicCapacityModifier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEnumAsByte<EBool> GainXP;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "GainXP == EBool::Yes", EditConditionHides))
	FName AttributeID;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "GainXP == EBool::Yes", EditConditionHides))
	int AttributeXPAmount;

	// Mark, or other Actors that can be affect on. For example, lever and gate will REFActor
	AActor* ReferencedActor;

	UPROPERTY(EditDefaultsOnly)
	FName AdditionalInfo;

	// Itself
	AInteractableActor* ActorREF;
	AInteractableRiver* ActorRiverREF;
	
	FSItem LocalItem;
	TArray<FName> ItemsID;
	bool bIsDurabilityDecay;
};
