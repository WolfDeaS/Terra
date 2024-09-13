// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Components/Button.h"
#include "Components/WrapBox.h"
#include "Terra/ProgressionComponent.h"
#include "Containers/UnrealString.h"
#include "WSkillSizeBox.generated.h"
/**
 *
 */

UCLASS(Abstract)
class TERRA_API UWSkillSizeBox : public UUserWidget
{
	GENERATED_BODY()
public:

	virtual void NativeConstruct();
	void Init(FSSkillsCreation* Skill, bool IsSkillActive, bool CanBeActivated, FName Attribute, FName SkillID);

	UFUNCTION(BlueprintCallable)
	void BUT_SkillEnact();

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* Name;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UImage* Icon; 
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Button;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class USizeBox* SizeBox;
	
	FSSkillsCreation* LocalSkill;
	FName LocalAttribute;
	FName LocalSkillID;
	bool Active;
	bool CanBeEnacted;
};