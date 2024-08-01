// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"
#include "InteractableActor.h"
#include "InteractionMark.h"
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

	UE_LOG(LogTemp, Warning, TEXT("On Interaction Began: Begin"));

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

	case EInteractID::Herb:
		
		UE_LOG(LogTemp, Warning, TEXT("On Interaction Began: Herb"));

		if (!LocalCharacter->bIsInteracted)
		{
			UE_LOG(LogTemp, Warning, TEXT("On Interaction Began: Herb Check Succeed"));

			FSHerbCreation* HerbDataFromDT;
			HerbDataFromDT = Cast<AInteractableActor>(ActorREF)->DT_Herbs->FindRow<FSHerbCreation>(AdditionalInfo, TEXT("none"), false);

			TArray<FName> LocalKeys;
			HerbDataFromDT->ItemsGiven.GenerateKeyArray(LocalKeys);

			
			for (auto& LocalItemID : LocalKeys)
			{
				if(LocalCharacter->InventoryComponent->IDThatRot.Contains(LocalItemID))
				{
					LocalCharacter->InventoryComponent->IDThatRot.Remove(LocalItemID);
					ItemsID.AddUnique(LocalItemID);
				}
			}
			
			LocalCharacter->ChangeCharacterInteractionStatus(1);
			GetWorld()->GetTimerManager().SetTimer(LocalCharacter->InteractionTimerHandle, [LocalCharacter, this]() { HerbTimer(LocalCharacter); }, HerbDataFromDT->GatherDuration, false);

			float LocalTimerDuration = HerbDataFromDT->GatherDuration + 0.5f;
			GetWorld()->GetTimerManager().SetTimer(LocalCharacter->InteractionAddRotTimerHandle, [LocalCharacter, this]() { ItemsMustRot(LocalCharacter); }, LocalTimerDuration , false);
		}
		
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

void UInteractionComponent::HerbTimer(ATerraCharacter* LocalCharacter)
{
	FSHerbCreation* HerbDataFromDT;
	HerbDataFromDT = Cast<AInteractableActor>(ActorREF)->DT_Herbs->FindRow<FSHerbCreation>(AdditionalInfo, TEXT("none"), false);

	for (auto& Pair : HerbDataFromDT->ItemsGiven)
	{
		LocalCharacter->InventoryComponent->CreateItem(Pair.Key, Pair.Value);
	}

	UE_LOG(LogTemp, Warning, TEXT("Herb Timer: Succeed"));

	LocalCharacter->ChangeCharacterInteractionStatus(0);
}

void UInteractionComponent::ItemsMustRot(ATerraCharacter* LocalCharacter)
{
	for (auto& ItemID : ItemsID)
	{
		FSItemCreation* ItemDataFromDT;
		ItemDataFromDT = LocalCharacter->InventoryComponent->DT_Items->FindRow<FSItemCreation>(ItemID, TEXT("none"), false);

		if (ItemDataFromDT->IsDurabilityFreshness)
		{
			LocalCharacter->InventoryComponent->IDThatRot.Add(ItemID);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Items Must Rot: Succeed"));
}