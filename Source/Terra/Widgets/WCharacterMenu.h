// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Components/Button.h"
#include "Components/WrapBox.h"
#include "Terra/TerraCharacter.h"
#include "WAttributeSizeBox.h"
#include "WInventorySizeBox.h"
#include "Terra/InteractableActor.h"
#include "WCharacterMenu.generated.h"
/**
 *
 */

UENUM(BlueprintType)
enum ECharacterMenuID
{
	BeverageContainerMenu,
};

UCLASS(Abstract)
class TERRA_API UWCharacterMenu : public UUserWidget
{
	GENERATED_BODY()
public:

	virtual void NativeConstruct();

	void ChangeUIStatus();
	void UpdateUI();
	void CreateAttributeWB();
	void CreateInventoryWB(TMap<FSItem, int> LocalInventory);
	void UpdateAttributeWB();
	void UnSelectedItem();
	void SetDesc();
	

	void UpdateStats(); //Дана функція потрібна щоб візуально цифери оновляти

	void ChangeSelectedItem(UWInventorySizeBox* NewSelectedItemSB); 

	UFUNCTION(BlueprintCallable)
	void CreateObjectInventoryWB();

	UFUNCTION(BlueprintCallable)
	void ChangeUIVisibleParts(uint8 Status);

	// 0 - Left, 1 - Right
	UFUNCTION(BlueprintCallable)
	void LeftRightButtonEffect(uint8 ButtonID);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWrapBox* WB_Attributes;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWrapBox* WB_Inventory;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* Cargo;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* TXT_Desc;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UCanvasPanel* SkillMenu;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UCanvasPanel* InventoryMenu;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* BUT_Skills;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* BUT_Chest;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* BUT_Inventory;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* BUT_Left;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* TXT_Left;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* BUT_Right;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* TXT_Right;
	

	bool StatusUI;
	ATerraCharacter* Character;

	// Тут всі Віджети для WB
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UWAttributeSizeBox> AttributeSizeBoxClass;
	UPROPERTY()
	class UWAttributeSizeBox* AttributeSizeBox;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UWInventorySizeBox> InventorySizeBoxClass;
	UPROPERTY()
	class UWInventorySizeBox* InventorySizeBox;

	FSItem SelectedItemStruct;
	UWInventorySizeBox* SelectedInventorySizeBox;

	AInteractableActor* InteractableActor;

	bool bIsChest;
	bool bIsChestOpen;

	FName WindowID;
	uint8 LocalMenuStatus;

	TMap<FSItem, int> LocalMenuInventory;
	uint8 MenuStatus;
	TEnumAsByte<ECharacterMenuID> CharacterMenuID;
};