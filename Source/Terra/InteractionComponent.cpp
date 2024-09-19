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

	// Beverage Source

	TMap<FSItem, float> RemoveItems;
	TMap<FSItem, float> AddItems;

	float LocalValue = 0.0f;

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
				if (LocalCharacter->InventoryComponent->IDThatRot.Contains(LocalItemID))
				{
					LocalCharacter->InventoryComponent->IDThatRot.Remove(LocalItemID);
					ItemsID.AddUnique(LocalItemID);
				}
			}

			LocalCharacter->ChangeCharacterInteractionStatus(1);
			GetWorld()->GetTimerManager().SetTimer(LocalCharacter->InteractionTimerHandle, [LocalCharacter, this]() { AfterInteractionEffect(LocalCharacter); }, HerbDataFromDT->GatherDuration, false);

			float LocalTimerDuration = HerbDataFromDT->GatherDuration + 0.5f;
			GetWorld()->GetTimerManager().SetTimer(LocalCharacter->InteractionAddRotTimerHandle, [LocalCharacter, this]() { ItemsMustRot(LocalCharacter); }, LocalTimerDuration, false);
		}

		break;

	case EInteractID::BevearageSource:

		LocalCharacterMenu = Cast<ATerraPlayerController>(LocalCharacter->GetController())->CharacterMenu;
		LocalCharacterMenu->InteractableActor = ActorREF;
		LocalCharacterMenu->ChangeUIStatus();
		LocalCharacterMenu->UpdateUI();
		for (auto& LocalForItem : LocalCharacter->InventoryComponent->Inventory)
		{
			LocalValue = LocalCharacter->InventoryComponent->GetBeverageCapacityFree(LocalForItem.Key);

			UE_LOG(LogTemp, Warning, TEXT("%s Value - %f"), LocalForItem.Key.ID, LocalValue);
			if (LocalValue > 0.0f)
			{
				LocalCharacterMenu->LocalMenuInventory.Add(LocalForItem.Key, LocalForItem.Value);
			}
		}

		LocalCharacterMenu->ChangeUIVisibleParts(3);



		break;

	case EInteractID::BeverageContainer:

		LocalCharacterMenu = Cast<ATerraPlayerController>(LocalCharacter->GetController())->CharacterMenu;
		LocalCharacterMenu->WindowID = "BeverageCapacityIA";
		LocalCharacterMenu->InteractableActor = ActorREF;
		LocalCharacterMenu->ChangeUIStatus();
		LocalCharacterMenu->UpdateUI();
		LocalCharacterMenu->ChangeUIVisibleParts(3);
		LocalCharacterMenu->LeftRightButtonEffect(0);


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

void UInteractionComponent::AfterInteractionEffect(ATerraCharacter* LocalCharacter)
{
	UWCharacterMenu* LocalCharacterMenu;
	float LocalValue = 0.0f;
	float AnotherLocalValue = 0.0f;
	TArray<FSItem> LocalItems;
	float BeverageContainerFree;
	float BeverageContainerFill;
	float LocalItemFree;
	float LocalItemFill;

	switch (InteractID)
	{

	default:
		
		break;

	case EInteractID::Herb:
		FSHerbCreation* HerbDataFromDT;
		HerbDataFromDT = Cast<AInteractableActor>(ActorREF)->DT_Herbs->FindRow<FSHerbCreation>(AdditionalInfo, TEXT("none"), false);

		for (auto& Pair : HerbDataFromDT->ItemsGiven)
		{
			LocalCharacter->InventoryComponent->CreateItem(Pair.Key, Pair.Value);
		}

		UE_LOG(LogTemp, Warning, TEXT("Herb Timer: Succeed"));

		LocalCharacter->ChangeCharacterInteractionStatus(0);

		break;

	case EInteractID::BevearageSource:
		if (!LocalCharacter->bIsInteracted)
		{
			LocalCharacter->ChangeCharacterInteractionStatus(1);
			
			LocalValue = 1.0f - ((*LocalItem.ModifierBonuses.Find("Capacity") - LocalCharacter->InventoryComponent->GetBeverageCapacityFree(LocalItem)) / *LocalItem.ModifierBonuses.Find("Capacity"));

			if (LocalItem.ModifierBonuses.Contains("MinTimeToFill"))
			{
				AnotherLocalValue = *LocalItem.ModifierBonuses.Find("MinTimeToFill");
			}
			UE_LOG(LogTemp, Warning, TEXT("AfterInteractionEffect;BevearageSource: LocalValue_1: %f"), LocalValue);
			LocalValue *= (*LocalItem.ModifierBonuses.Find("TimeToFill") - AnotherLocalValue);
			LocalValue += AnotherLocalValue;
			UE_LOG(LogTemp, Warning, TEXT("AfterInteractionEffect;BevearageSource: LocalValue_1: %f"), LocalValue);
			UE_LOG(LogTemp, Warning, TEXT("AfterInteractionEffect;BevearageSource: LocalValue_2: %f"), (*LocalItem.ModifierBonuses.Find("TimeToFill") - AnotherLocalValue));
			UE_LOG(LogTemp, Warning, TEXT("AfterInteractionEffect;BevearageSource: MinTimeToFill: %f"), AnotherLocalValue);

			GetWorld()->GetTimerManager().SetTimer(LocalCharacter->InteractionTimerHandle, [LocalCharacter, this]() { AfterInteractionEffect(LocalCharacter); }, LocalValue, false);

			UE_LOG(LogTemp, Warning, TEXT("AfterInteractionEffect;BevearageSource: Begin Fill Capacity: %f"), LocalValue);
		}
		else
		{
			LocalCharacter->ChangeCharacterInteractionStatus(0);

			LocalCharacterMenu = Cast<ATerraPlayerController>(LocalCharacter->GetController())->CharacterMenu;

			LocalValue = LocalCharacter->InventoryComponent->GetBeverageCapacityFree(LocalItem);

			UE_LOG(LogTemp, Warning, TEXT("AfterInteractionEffect;BevearageSource: ItemID: %s"), LocalItem.ID);
			UE_LOG(LogTemp, Warning, TEXT("AfterInteractionEffect;BevearageSource: LocalCharacter Inv Num: %d"), LocalCharacter->InventoryComponent->Inventory.Num());
			LocalCharacter->InventoryComponent->AddBeverage(LocalItem, AdditionalInfo, LocalValue);

			UE_LOG(LogTemp, Warning, TEXT("AfterInteractionEffect;BevearageSource: End Fill Capacity"));
		}

		break;

	// 0 - To ид, 1 - To IA
	case EInteractID::BeverageContainer:
		

		LocalCharacterMenu = Cast<ATerraPlayerController>(LocalCharacter->GetController())->CharacterMenu;

		if (!LocalCharacter->bIsInteracted)
		{
			LocalCharacter->ChangeCharacterInteractionStatus(1);
			LocalNewItem = LocalCharacterMenu->SelectedItemStruct;

			ActorREF->InventoryComponent->Inventory.GenerateKeyArray(LocalItems);
			LocalItem = LocalItems[0];

			if (LocalCharacterMenu->LocalMenuStatus == 0)
			{
				LocalValue = 1.0f - ((LocalCharacter->InventoryComponent->GetBeverageCapacityFill(LocalNewItem)) / *LocalNewItem.ModifierBonuses.Find("Capacity"));

				if (LocalNewItem.ModifierBonuses.Contains("MinTimeToFill"))
				{
					AnotherLocalValue = *LocalNewItem.ModifierBonuses.Find("MinTimeToFill");
				}
				LocalValue *= (*LocalNewItem.ModifierBonuses.Find("TimeToFill") - AnotherLocalValue);
				LocalValue += AnotherLocalValue;
			}
			else
			{
				LocalValue = 1.0f - ((LocalCharacter->InventoryComponent->GetBeverageCapacityFree(LocalNewItem)) / *LocalNewItem.ModifierBonuses.Find("Capacity"));

				if (LocalNewItem.ModifierBonuses.Contains("MinTimeToFill"))
				{
					AnotherLocalValue = *LocalNewItem.ModifierBonuses.Find("MinTimeToFill");
				}
				LocalValue *= (*LocalNewItem.ModifierBonuses.Find("TimeToFill") - AnotherLocalValue);
				LocalValue += AnotherLocalValue;
			}

			UE_LOG(LogTemp, Warning, TEXT("AfterInteractionEffect;BeverageContainer: Start Capacity - %f"), LocalValue);

			GetWorld()->GetTimerManager().SetTimer(LocalCharacter->InteractionTimerHandle, [LocalCharacter, this]() { AfterInteractionEffect(LocalCharacter); }, LocalValue, false);
		}
		else
		{
			LocalCharacter->ChangeCharacterInteractionStatus(0);

			BeverageContainerFill = ActorREF->InventoryComponent->GetBeverageCapacityFill(LocalItem);
			BeverageContainerFree = ActorREF->InventoryComponent->GetBeverageCapacityFree(LocalItem);
			LocalItemFill = ActorREF->InventoryComponent->GetBeverageCapacityFill(LocalNewItem);
			LocalItemFree = ActorREF->InventoryComponent->GetBeverageCapacityFree(LocalNewItem);

			if (LocalCharacterMenu->LocalMenuStatus == 0)
			{
				if (BeverageContainerFree >= LocalItemFill)
				{
					ActorREF->InventoryComponent->AddSeveralBeverage(LocalItem, LocalNewItem.CapacityModifier, true);

					LocalCharacter->InventoryComponent->RemoveItem(LocalNewItem, 1);
					LocalNewItem.CapacityModifier = LocalCharacter->InventoryComponent->RemoveAndApplyBeverageModifier(LocalNewItem.CapacityModifier, LocalItemFill);
					LocalCharacter->InventoryComponent->AddItemToInventory(LocalNewItem, 1);
				}
				else
				{
					LocalValue = LocalItemFill - BeverageContainerFree;

					ActorREF->InventoryComponent->AddSeveralBeverage(LocalItem, LocalCharacter->InventoryComponent->RemoveAndApplyBeverageModifier(LocalNewItem.CapacityModifier, LocalValue), true);

					LocalCharacter->InventoryComponent->RemoveItem(LocalNewItem, 1);
					LocalNewItem.CapacityModifier = LocalCharacter->InventoryComponent->RemoveAndApplyBeverageModifier(LocalNewItem.CapacityModifier, BeverageContainerFree);
					LocalCharacter->InventoryComponent->AddItemToInventory(LocalNewItem, 1);
				}
			}
			else if (LocalCharacterMenu->LocalMenuStatus == 1)
			{
				if (LocalItemFree >= BeverageContainerFill)
				{
					LocalCharacter->InventoryComponent->AddSeveralBeverage(LocalNewItem, LocalItem.CapacityModifier);

					ActorREF->InventoryComponent->RemoveItem(LocalItem, 1, true);
					LocalItem.CapacityModifier = ActorREF->InventoryComponent->RemoveAndApplyBeverageModifier(LocalItem.CapacityModifier, BeverageContainerFill);
					ActorREF->InventoryComponent->AddItemToInventory(LocalItem, 1, true);
				}
				else
				{
					LocalValue = BeverageContainerFill - LocalItemFree;

					LocalCharacter->InventoryComponent->AddSeveralBeverage(LocalNewItem, ActorREF->InventoryComponent->RemoveAndApplyBeverageModifier(LocalItem.CapacityModifier, LocalValue));

					ActorREF->InventoryComponent->RemoveItem(LocalItem, 1, true);
					LocalItem.CapacityModifier = ActorREF->InventoryComponent->RemoveAndApplyBeverageModifier(LocalItem.CapacityModifier, LocalItemFree);
					ActorREF->InventoryComponent->AddItemToInventory(LocalItem, 1, true);
				}
			}

			UE_LOG(LogTemp, Warning, TEXT("AfterInteractionEffect;BeverageContainer: End Fill Capacity"));
		}

		break;
	}
}