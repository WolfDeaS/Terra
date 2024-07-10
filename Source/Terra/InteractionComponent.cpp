// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"
#include "InteractableActor.h"
#include "TerraCharacter.h"
#include "TerraPlayerController.h"

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInteractionComponent::OnInteractionBegan(ATerraCharacter* LocalCharacter)
{
	FSItem ToAddItem;
	UWCharacterMenu* LocalCharacterMenu;

	switch (InteractID)
	{
	case EInteractID::Standart:
		
		break;

	case EInteractID::Chest:
	
		LocalCharacterMenu = Cast<ATerraPlayerController>(LocalCharacter->GetController())->CharacterMenu;
		LocalCharacterMenu->InteractableActor = ActorREF;
		LocalCharacterMenu->ChangeUIStatus();
		LocalCharacterMenu->UpdateUI();
		LocalCharacterMenu->ChangeUIVisibleParts(2);

		break;

	case EInteractID::Item:

		ToAddItem = LocalCharacter->InventoryComponent->CalculateModifierBonuses(LocalItem);
		LocalCharacter->InventoryComponent->AddItemToInventory(ToAddItem, 1);
		ActorREF->DestroyIA();
		break;

	default:

		break;
	}

	if (GainXP = EBool::Yes)
	{
		LocalCharacter->ProgressionComponent->AddXPToAttribute(AttributeID, AttributeXPAmount);
	}
}

void UInteractionComponent::OnInteractionEnd()
{

}

void UInteractionComponent::TurningOutlineOn()
{

}

void UInteractionComponent::TurningOutlineOff()
{

}

void UInteractionComponent::OnSecondCalculations()
{
	if (bIsDurabilityDecay && LocalItem.Durability > 0)
	{
		LocalItem.Durability--;
	}
}