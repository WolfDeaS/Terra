// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Components/Button.h"
#include "Components/WrapBox.h"
#include "WHUD.generated.h"
/**
 *
 */

class ATerraCharacter;
UCLASS(Abstract)
class TERRA_API UWHUD : public UUserWidget
{
	GENERATED_BODY()
public:

	virtual void NativeConstruct();

	void Init();

	UFUNCTION(BlueprintCallable)
	void UpdateStatusPB();


	///////////////////// PB /////////////////////

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UProgressBar* PB_Health;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UProgressBar* PB_Hunger;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UProgressBar* PB_Thirst;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UProgressBar* PB_Energy;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UProgressBar* PB_Stress;

	///// TB /////

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* TB_Hunger;

	//////////////

	//////////////////////////////////////////////

	UPROPERTY(BlueprintReadWrite)
	ATerraCharacter* Character;
	UPROPERTY(BlueprintReadWrite)
	TArray<FName> Statuses;
};