// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Components/Button.h"
#include "Components/WrapBox.h"
#include "ProgressionComponent.h"
#include "Containers/UnrealString.h"
#include "InventoryComponent.h"
#include "WInventorySizeBox.generated.h"
/**
 *
 */

class UWCharacterMenu;
UCLASS(Abstract)
class TERRA_API UWInventorySizeBox : public UUserWidget
{
	GENERATED_BODY()
public:

	virtual void NativeConstruct();
	void Init(FSItem* Item, uint32 Quantity, UDataTable* DT_Items);
	void ChangeButtonStatus(uint8 Status);

	UFUNCTION(BlueprintCallable)
	void BUT_Use();


	/*
	UFUNCTION(BlueprintCallable)
	void BUT_SkillEnact();
	*/

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* Name;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* Quantity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* Durability;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UImage* Icon; 
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Button;
	
	FSItem LocalItem;

	UWCharacterMenu* Parent;


	FLinearColor UnSelectButton;
	FLinearColor SelectButton;
	FLinearColor ActiveButton;
};