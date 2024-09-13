// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Components/Button.h"
#include "Components/WrapBox.h"
#include "Components/ProgressBar.h"
#include "Terra/ProgressionComponent.h"
#include "WSkillSizeBox.h"
#include "WAttributeSizeBox.generated.h"
/**
 *
 */

class UWCharacterMenu;

UCLASS(Abstract)
class TERRA_API UWAttributeSizeBox : public UUserWidget
{
	GENERATED_BODY()
public:

	virtual void NativeConstruct();
	void Init(FSAttribute* LocalAttribute, UDataTable* DT_Attributes, UDataTable* DT_Skills, TArray<int> NewLevelXPRequired);
	void UpdateSkillsWB();

	UFUNCTION(BlueprintCallable)
	void BUT_Use();

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* Name;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UImage* Icon;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Button;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* Level;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UProgressBar* PB;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* XP;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWrapBox* WB_Skills;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class USizeBox* SizeBox;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UWSkillSizeBox> SkillSizeBoxClass;
	UPROPERTY()
	class UWSkillSizeBox* SkillSizeBox;

	float StandatHeightOverride;
	UWCharacterMenu* Parent;
	bool SkillsWBOpen;
	FSAttribute* Attribute;
	UDataTable* Local_DT_Attributes;
	UDataTable* Local_DT_Skills;
	FSAttributeCreation* LocalAttributeData;
	
	uint8 AttributeLevel;

	/*
	void Init();

	UFUNCTION(BlueprintCallable)
	void BUT_ProdSphereClicked();
	*/


};