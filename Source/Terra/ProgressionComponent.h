// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProgressionComponent.generated.h"



class ATerraCharacter;
USTRUCT(BlueprintType)
struct FSAttributeCreation : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* Icon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FName, uint8> Skills;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FName, float> ModifierBonuses;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<int> OnWhichLevelsAddPoint;

};

USTRUCT(BlueprintType)
struct FSSkillsCreation : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* Icon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FName, float> ModifierBonuses;
};

USTRUCT(BlueprintType)
struct FSAttribute : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName ID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 Level;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 XP;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FName> ActivedSkills;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Point;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TERRA_API UProgressionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	TArray<FSAttribute> Attributes;

	UDataTable* DT_Attributes;
	UDataTable* DT_Skills;

	ATerraCharacter* Character;

	UPROPERTY(EditAnywhere)
	TArray<int> NewLevelXPRequired;

public:	
	// Called every frame
	void Init();

	void AddXPToAttribute(FName Attribute, int XP);
	void LevelUp(FSAttribute* Attribute);
	
	bool IsSkillActive(FName SkillID);
	bool IsAttributesOnLevel(TMap<FName, uint8> ConditionsAttributes);
	float GetAttributeOnLevelModifier(TMap<FName, uint8> ConditionsAttributes);
	

	// Пошуки
	FSAttribute* FindAttribute(FName Attribute);
	FSAttributeCreation* FindDTAttributeRow(FName Attribute);
};
