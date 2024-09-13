// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/UserDefinedEnum.h"
#include "InventoryComponent.generated.h"

class ATerraCharacter;
class AInteractableActor;

UENUM(BlueprintType)
enum class EItemType
{
	Weapon, 
	Armour,
	Food,
	BeverageContainer,
};

UENUM(BlueprintType)
enum class EEquipmentType 
{
	No,
	Helmet,
	MeleeWeapon,
};

USTRUCT(BlueprintType)
struct FSBeverageCreation : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FName, float> ModifierBonuses;

};

USTRUCT(BlueprintType)
struct FSItemCreation : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EItemType> Type; // Це чисто для інтерфейса потім

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EEquipmentType> EquipmentType;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* Icon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FName, float> ModifierBonuses;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FName, float> ModifierBonusesOnLowDurability;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FName, float> StaticModifierBonuses;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int EffectDuration;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FName, uint8> LevelRequirement;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Weight;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MaxDurability; // Також свіжість

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool IsDurabilityFreshness;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMesh* ItemMesh;

};

USTRUCT(BlueprintType)
struct FSItem : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName ID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FName, float> ModifierBonuses;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FName, float> CapacityModifier;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Durability; // Також свіжість

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool Active;


	bool operator==(const FSItem& Other) const
	{
		if (ModifierBonuses.Num() != Other.ModifierBonuses.Num())
		{
			return false;
		}
		
		for (const auto& Pair : ModifierBonuses)
		{
			const FName& Key = Pair.Key; 
			const float& Value1 = Pair.Value; 

			// Check if the key exists in Map2
			if (Other.ModifierBonuses.Contains(Key))
			{
				const float* Value2 = Other.ModifierBonuses.Find(Key);
				if (Value2 == nullptr || *Value2 != Value1)
				{
					// Either the key doesn't exist in Map2 or the values are not equal
				}
			}
			else
			{
			}
		}

		for (const auto& Pair : CapacityModifier)
		{
			const FName& Key = Pair.Key;
			const float& Value1 = Pair.Value;

			// Check if the key exists in Map2
			if (Other.CapacityModifier.Contains(Key))
			{
				const float* Value2 = Other.CapacityModifier.Find(Key);
				if (Value2 == nullptr || *Value2 != Value1)
				{
					// Either the key doesn't exist in Map2 or the values are not equal
					return false;
				}
			}
			else
			{
				return false;
			}
		}

		return ID == Other.ID && Durability == Other.Durability;
	}


	friend uint32 GetTypeHash(const FSItem& MyStruct)
	{
		uint32 Hash;
		Hash = HashCombine(GetTypeHash(MyStruct.ID), GetTypeHash(MyStruct.Durability));
		Hash = HashCombine(Hash, GetTypeHash(MyStruct.Active));
		for (const auto& Pair : MyStruct.ModifierBonuses)
		{
			Hash = HashCombine(Hash, GetTypeHash(Pair.Key));
			Hash = HashCombine(Hash, GetTypeHash(Pair.Value));
		}
		for (const auto& Pair : MyStruct.CapacityModifier)
		{
			Hash = HashCombine(Hash, GetTypeHash(Pair.Key));
			Hash = HashCombine(Hash, GetTypeHash(Pair.Value));
		}

		return Hash;
	}

	FSItem& operator=(const FSItem& other) {
		if (this != &other) { // Avoid self-assignment
			// Copy data members from other
			ID = other.ID;
			ModifierBonuses = other.ModifierBonuses; // Consider deep copy for TMap
			CapacityModifier = other.CapacityModifier;
			Durability = other.Durability;
			Active = other.Active;
		}
		return *this;
	}

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TERRA_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Called every frame

	UInventoryComponent();

	void AddItemToInventory(FSItem Item, int Quantity, bool bIgnoreCargo = false);
	FSItem CreateItem(FName ID, int Quantity, bool bIgnoreCargo = false);
	void RemoveItem(FSItem Item, int Quantity, bool bIgnoreCargo = false);
	void ThrowItem(FSItem Item);
	void UseItem(FSItem Item);
	void RemoveItemEffect(FSItem LocalItem);
	void UseHighPriorityItemByTag(FName LocalName, FName AdditionalTag = "None");

	/// RECALCULATE MODIFERS ///

	void RecalculateInventoryModifierBonuses();
	FSItem CalculateModifierBonuses(FSItem Item);
	float CalculateModifiersBasedOnLevelRequirement(FSItem Item);
	TMap<FName, float> CalculateModifiersBasedOnDurability(FSItem Item);
	void CalculateCharactersModifiers(TMap<FName, float> Modifiers, bool bSubtractInsteadOfAdd = false);

	TMap<FName, float> CalculateItemModifiersDurabilityBasedOnSeconds(FName ItemID, float Seconds); // Return Modifiers which based on durability minus Seconds

	TMap<FName, float> GetUnitedModifierBonusesByItemID(FName ItemID);

	FSItem AddBeverage(FSItem Item, FName BeverageID, float Value);
	FSItem AddSeveralBeverage(FSItem Item, TMap<FName, float> Beverage, bool bIgnoreCargo = false);
	TMap<FName, float> RemoveAndApplyBeverageModifier(TMap<FName, float> CapacityModifier, float BeverageValue, bool bApplyModifiers = false);

	////////////////////////////

	/// FIND ///
	
	FSItem FindItemByID(FName LocalID, TArray<FSItem> ItemArray);
	FSItem FindItemWithHighsetDelicious(TArray<FSItem> ItemArray);
	TArray<FSItem> FindItemArrayWithModifier(TArray<FSItem> ItemArray, FName Modifier);
	float GetBeverageCapacityUseBasedOnStatusNeed(TMap<FName, float> Capacity, TMap<FName, float> StatusNeed);
	float GetBeverageCapacityFree(FSItem LocalItem);
	float GetBeverageCapacityFill(FSItem LocalItem);

	////////////

	void RecalculateCargoBasedOnItem(FName ID, int PrevQuantity, int NewQuantity);
	void OnSecondCalculations();

	void Init();

	UDataTable* DT_Items;
	UDataTable* DT_Beverage;

	ATerraCharacter* Character;

	// Самі предмети
	UPROPERTY()
	TMap<FSItem, int> Inventory;

	TMap<TEnumAsByte<EEquipmentType>, FSItem> Equipment;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AInteractableActor> ItemObjectOnThrowClass;
	UPROPERTY()
	class AInteractableActor* ItemObjectOnThrow;

	FSItem NoneItem;

	TArray<FName> IDThatRot;
};
